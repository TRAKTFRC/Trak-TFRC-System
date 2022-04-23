#include "ext_mem_eeprom.h"
#include "24c64.h"
#include <string.h>
#include "rtc.h"

uint16_t getNextAddress ()
{
    uint16_t nex_adr = 0;
    nex_adr = EEReadByte (EXT_EROM_NEX_ADR_LOW);
    nex_adr |= (EEReadByte (EXT_EROM_NEX_ADR_HIGH) << 8);
    return nex_adr;
}

void updateNextAddress (uint16_t nex_adr)
{
    uint8_t temp_byte = 0;
    temp_byte = nex_adr & 0xFF;
    EEWriteByte (EXT_EROM_NEX_ADR_LOW, temp_byte);
    temp_byte = (nex_adr >> 8) & 0xFF;
    EEWriteByte (EXT_EROM_NEX_ADR_HIGH, temp_byte);
}


void sotreData (char * pkt)
{
    /*
    Write packet sequence:
    - Read next write addr from EEPROM
    - Check if full Pkt can fit with this address
        - If It cannot fit, change address to start packet
        - If it can fit, continue
    - Write whole string into the EEPROM while maintaining address counter
    - Update the next address in the EEPROM
    */
   // Get the start address
   uint16_t nex_adr = getNextAddress ();

   // Checking if pkt fits in the given address
   // Get pkt length
   char pkt_len = strlen (pkt);
   if (pkt_len > MAX_PKT_STORE_LEN) return;
   // Checking if packet fits in the address
   if ((pkt_len + nex_adr) >= EXT_EROM_HIGHEST_ADR)
   {
       // Move to start address if it dosen't
       nex_adr = EXT_EROM_PKT_START_ADR;
   }

    // Write string from the address
    EEWriteString (nex_adr, pkt, pkt_len);

    // Update the address
    nex_adr = nex_adr + pkt_len;
    updateNextAddress (nex_adr);
    return;
}

void generateStorePacket (TinyGPSPlus &gps_data, struct tm * time, char * ptr_pkt)
{
    	ptr_pkt = gps_data.location.getLatStr (ptr_pkt); // Adding Latitude
		*(ptr_pkt++) = ','; // Adding seperator
		ptr_pkt = gps_data.location.getLonStr (ptr_pkt); // Adding Longitude
		*(ptr_pkt++) = ','; // Adding seperator
		ptr_pkt = gps_data.getHDOPStr (ptr_pkt); // Adding HDOP
		*(ptr_pkt++) = ','; // Adding seperator
        // Adding Time stamp
		ptr_pkt += sprintf (ptr_pkt, "%d:%d:%d", time->hour, time->min, time->sec);

		*(ptr_pkt++) = ';'; // Adding Packet end
        *(ptr_pkt) = 0; // Adding Null Character
}

void storeDataPointInEEPROM (TinyGPSPlus &gps_data, struct tm * time)
{
    char pkt [MAX_PKT_STORE_LEN];
    generateStorePacket (gps_data, time, pkt);
    sotreData (pkt);
}
