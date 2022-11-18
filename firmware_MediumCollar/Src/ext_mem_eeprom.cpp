#include "ext_mem_eeprom.h"
#include "24c64.h"
#include <string.h>
#include "rtc.h"
#include "battery.h"

uint16_t getNextAddress ()
{
    uint16_t in_cntr, nex_adr;
    in_cntr = EEReadByte (EXT_EROM_DATA_IN_CNTR_ADR);
    in_cntr |= (EEReadByte (EXT_EROM_DATA_IN_CNTR_ADR + 1) << 8);
    nex_adr = (in_cntr * PKT_STORE_LEN)+ EXT_EROM_PKT_START_ADR;
    return nex_adr;
}

void incrInCntr ()
{
    uint8_t temp_byte = 0;
    uint16_t in_cntr, nex_adr;
    // Get existing In Counter from EEPROM
    in_cntr = EEReadByte (EXT_EROM_DATA_IN_CNTR_ADR);
    in_cntr |= (EEReadByte (EXT_EROM_DATA_IN_CNTR_ADR + 1) << 8);
    // Increament the counter such that is does not exceed PKT_MAX_COUNT
    in_cntr = (in_cntr + 1) % PKT_MAX_COUNT;
    // Break the value into 2 8 bits and write to EEPROM
    temp_byte = in_cntr & 0xFF;
    EEWriteByte (EXT_EROM_DATA_IN_CNTR_ADR, temp_byte);
    temp_byte = (in_cntr >> 8) & 0xFF;
    EEWriteByte (EXT_EROM_DATA_IN_CNTR_ADR + 1, temp_byte);
}

void incrOutCntr ()
{
    uint8_t temp_byte = 0;
    uint16_t out_cntr, in_cntr, nex_adr;
    // Get existing In Counter from EEPROM
    in_cntr = EEReadByte (EXT_EROM_DATA_IN_CNTR_ADR);
    in_cntr |= (EEReadByte (EXT_EROM_DATA_IN_CNTR_ADR + 1) << 8);
    // Get existing Out Counter from EEPROM
    out_cntr = EEReadByte (EXT_EROM_DATA_OUT_CNTR_ADR);
    out_cntr |= (EEReadByte (EXT_EROM_DATA_OUT_CNTR_ADR + 1) << 8);
    // Increament the counter such that is does not exceed PKT_MAX_COUNT
    if (in_cntr > out_cntr) out_cntr = (out_cntr + 1) % PKT_MAX_COUNT;
    else if (in_cntr < out_cntr) out_cntr = out_cntr + 1;
    // Break the value into 2 8 bits and write to EEPROM
    temp_byte = out_cntr & 0xFF;
    EEWriteByte (EXT_EROM_DATA_OUT_CNTR_ADR, temp_byte);
    temp_byte = (out_cntr >> 8) & 0xFF;
    EEWriteByte (EXT_EROM_DATA_OUT_CNTR_ADR + 1, temp_byte);
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

void generateStorePacket (TinyGPSPlus * gps_data, char * ptr_pkt)
{
    char * p_pkt = ptr_pkt;
    uint8_t vccx10 = readVccVoltage ();
    ptr_pkt = gps_data->location.getLatStr (ptr_pkt); // Adding Latitude
    *(ptr_pkt++) = ','; // Adding seperator
    ptr_pkt = gps_data->location.getLonStr (ptr_pkt); // Adding Longitude
    *(ptr_pkt++) = ','; // Adding seperator
    ptr_pkt = gps_data->getHDOPStr (ptr_pkt); // Adding HDOP
    *(ptr_pkt++) = ','; // Adding seperator
    // Adding Time stamp
    ptr_pkt += sprintf (ptr_pkt, "%02d:%02d:%02d",  gps_data->time.hour(), gps_data->time.minute(), gps_data->time.second());
    *(ptr_pkt++) = ','; // Adding seperator

    // Adding Date stamp
    ptr_pkt += sprintf (ptr_pkt, "%02d:%02d:%02d",  gps_data->date.day(), gps_data->date.month(), gps_data->date.year());
    *(ptr_pkt++) = ','; // Adding seperator

    ptr_pkt += sprintf (ptr_pkt, "%02d", vccx10); // Adding the Vcc Voltage
    *(ptr_pkt++) = ','; // Adding seperator

    // GPS State
    ptr_pkt += sprintf (ptr_pkt, "GOK");

    *(ptr_pkt) = 0; // Adding Null Character
    printf ("EEROM Pkt: %s\r\n", p_pkt);
}

void storeDataPointInEEPROM (TinyGPSPlus &gps_data)
{
    char pkt [MAX_PKT_STORE_LEN];
    // Generate the packet
    generateStorePacket (&gps_data, pkt);
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
    char * ptr_pkt = ret_pkt;
	uint16_t byte_count = 0, pkt_len;

    in_cntr = EEReadByte (EXT_EROM_DATA_IN_CNTR_ADR);
    in_cntr |= (EEReadByte (EXT_EROM_DATA_IN_CNTR_ADR + 1) << 8);
    out_cntr = EEReadByte (EXT_EROM_DATA_OUT_CNTR_ADR);
    out_cntr |= (EEReadByte (EXT_EROM_DATA_OUT_CNTR_ADR + 1) << 8);
    if (in_cntr != out_cntr)
    {
        *(ptr_pkt++) = PKT_SOH; // Start of header

        // Name of device
        *(ptr_pkt++) = DEVICE_CODE;
        byte_count = sprintf (ptr_pkt, "%02d", dev_id);
        ptr_pkt += byte_count;
        *(ptr_pkt++) = ','; // Adding seperator

        // Loading data from EEPROM
        pkt_adr = (out_cntr * PKT_STORE_LEN) + EXT_EROM_PKT_START_ADR;
        for (uint16_t loop_count = 0; loop_count < PKT_STORE_LEN; loop_count++)
        {
            uint8_t temp_byte = EEReadByte (pkt_adr + loop_count);
            *(ptr_pkt++) = temp_byte;
        }
        *(ptr_pkt++) = PKT_EOH; // End of header
        *(ptr_pkt) = 0; // Adding NULL

        pkt_len = strlen (ret_pkt);
        printf ("In count: %d, Out count: %d\r\n", in_cntr, out_cntr);
        printf ("Pkt gen: %s   %d \r\n", ret_pkt, pkt_len);
        return true;
    }
    return false;
}

void checkEEROMInit ()
{
    uint8_t init_flag = EEReadByte (EXT_EROM_INIT_FLAG_ADR);
    
    if (init_flag == EXT_EROM_INIT_DONE_VAL) return;

    EEWriteByte (EXT_EROM_DATA_OUT_CNTR_ADR, 0);
    EEWriteByte (EXT_EROM_DATA_OUT_CNTR_ADR + 1, 0);
    EEWriteByte (EXT_EROM_DATA_IN_CNTR_ADR, 0);
    EEWriteByte (EXT_EROM_DATA_IN_CNTR_ADR + 1, 0);

    EEWriteByte (EXT_EROM_INIT_FLAG_ADR, EXT_EROM_INIT_DONE_VAL);
}