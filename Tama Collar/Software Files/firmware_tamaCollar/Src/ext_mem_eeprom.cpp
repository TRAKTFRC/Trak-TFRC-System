#include "ext_mem_eeprom.h"
#include "24c64.h"
#include <string.h>
#include "rtc.h"

uint16_t getNextAddress ()
{
    uint16_t in_cntr, nex_adr;
    in_cntr = EEReadByte (EXT_EROM_DATA_IN_CNTR_ADR);
    in_cntr |= (EEReadByte (EXT_EROM_DATA_IN_CNTR_ADR) << 8);
    nex_adr = (in_cntr * PKT_STORE_LEN)+ EXT_EROM_PKT_START_ADR_ADR;
    return nex_adr;
}

void incrInCntr ()
{
    uint8_t temp_byte = 0;
    uint16_t in_cntr, nex_adr;
    // Get existing In Counter from EEPROM
    in_cntr = EEReadByte (EXT_EROM_DATA_IN_CNTR_ADR);
    in_cntr |= (EEReadByte (EXT_EROM_DATA_IN_CNTR_ADR) << 8);
    // Increament the counter such that is does not exceed PKT_MAX_COUNT
    in_cntr = (in_cntr + 1) % PKT_MAX_COUNT;
    // Break the value into 2 8 bits and write to EEPROM
    temp_byte = in_cntr & 0xFF;
    EEWriteByte (EXT_EROM_DATA_IN_CNTR_ADR, temp_byte);
    temp_byte = (in_cntr >> 8) & 0xFF;
    EEWriteByte (EXT_EROM_DATA_IN_CNTR_ADR, temp_byte);
}


void sotreData (char * pkt)
{
    // Get the start address
    uint16_t nex_adr = getNextAddress ();
    // Write string from the address
    EEWriteString (nex_adr, pkt, PKT_STORE_LEN);
    // Increament in counter
    incrInCntr ();
    return;
}

void generateStorePacket (TinyGPSPlus &gps_data, uint8_t gps_ret, struct tm * time, char * ptr_pkt)
{
    char * p_pkt = ptr_pkt;
    uint8_t vccx10 = readVccVoltage ();
    ptr_pkt = gps_data.location.getLatStr (ptr_pkt); // Adding Latitude
    *(ptr_pkt++) = ','; // Adding seperator
    ptr_pkt = gps_data.location.getLonStr (ptr_pkt); // Adding Longitude
    *(ptr_pkt++) = ','; // Adding seperator
    ptr_pkt = gps_data.getHDOPStr (ptr_pkt); // Adding HDOP
    *(ptr_pkt++) = ','; // Adding seperator
    // Adding Time stamp
    ptr_pkt += sprintf (ptr_pkt, "%2d:%2d:%2d", time->hour, time->min, time->sec);
    *(ptr_pkt++) = ','; // Adding seperator

    ptr_pkt += sprintf (ptr_pkt, "%2d", vccx10); // Adding the Vcc Voltage
    *(ptr_pkt++) = ','; // Adding seperator

    // GPS State
    if (gps_ret == GPS_RET_SLEEP_FAIL)
        ptr_pkt += sprintf (ptr_pkt, "GFS");
    else
        ptr_pkt += sprintf (ptr_pkt, "GOK");

    *(ptr_pkt) = 0; // Adding Null Character
    printf ("EEROM Pkt: %s\r\n", p_pkt);
}

void storeDataPointInEEPROM (TinyGPSPlus &gps_data, uint8_t gps_ret, struct tm * time)
{
    char pkt [MAX_PKT_STORE_LEN];
    // Generate the packet
    generateStorePacket (gps_data, gps_ret, time, pkt);
    // Store the packet in memory while incrementing the in counter
    sotreData (pkt);
}

void dumpEEPROMPkt ()
{
    uint16_t nex_addr = getNextAddress ();
    uint8_t temp_byte;
    printf (("Next Address: "));
    printf ("%d\r\n", nex_addr);
    for (uint16_t loop_count = EXT_EROM_PKT_START_ADR; loop_count < EXT_EROM_HIGHEST_ADR; loop_count++)
    {
        temp_byte = EEReadByte (loop_count);
        printf ("%c", temp_byte);
        if (temp_byte == ';') printf ("\r\n");
        if (temp_byte == 255) break;
    }    
}

bool checkGetNextPkt (char * ret_pkt)
{
    uint16_t in_cntr, out_cntr, pkt_adr;
    in_cntr = EEReadByte (EXT_EROM_DATA_IN_CNTR_ADR);
    in_cntr |= (EEReadByte (EXT_EROM_DATA_IN_CNTR_ADR) << 8);
    out_cntr = EEReadByte (EXT_EROM_DATA_OUT_CNTR_ADR);
    out_cntr |= (EEReadByte (EXT_EROM_DATA_OUT_CNTR_ADR) << 8);
    if (in_cntr != out_cntr)
    {
        pkt_adr = (in_cntr * PKT_STORE_LEN) + EXT_EROM_PKT_START_ADR_ADR;
        for (uint16_t loop_count = 0; loop_count < PKT_STORE_LEN; loop_count++)
        {
            temp_byte = EEReadByte (pkt_adr + loop_count);
            *(ret_pkt + loop_count) = temp_byte;
        }
        return true;
    }
    return false;
}
