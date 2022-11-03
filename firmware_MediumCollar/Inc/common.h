#ifndef __COMMON_H
#define __COMMON_H

#include "mcu_call_functions.h"

#define SET                     1
#define RESET                   0

#define SUCCESS                 0
#define FAILURE                 1

#define GPS_HDOP_REQ            400
#define GPS_EXTENDED_WAIT       30000
#define GPS_ACQUISITION_WAIT    60000

#define DEVICE_CODE				'D'

#define PKT_SOH                 '{'
#define PKT_EOH                 '}'

#define PKT_SRC_LORA            0
#define PKT_SRC_UART            1

#define SEN_PKT_BUFF_LEN        51 // Packet length is 50, + 1 for NULL Character

// Purpose for Rcv LoRa Pkt
#define PKT_RCV_PURPOSE_NONE    0
#define PKT_RCV_PURPOSE_ACK     1
#define PKT_RCV_PURPOSE_RTC     2

extern uint8_t dev_id;

// EEPROM Memory Map
// Device ID
#define EEPROM_ADDR_ID                  10
#define EEPROM_ADDR_ID_FLAG             11
#define EEPROM_SCH_FLAG                 12
// Schedule Start Time
#define EEPROM_ADDR_START_TIME_HR       20
#define EEPROM_ADDR_START_TIME_MIN      21
#define EEPROM_ADDR_START_TIME_SEC      22
// Schedule End Time
#define EEPROM_ADDR_END_TIME_HR         23
#define EEPROM_ADDR_END_TIME_MIN        24
#define EEPROM_ADDR_END_TIME_SEC        25
// Schedule Interval Time
#define EEPROM_ADDR_INTRVL_TIME_HR      26
#define EEPROM_ADDR_INTRVL_TIME_MIN     27
#define EEPROM_ADDR_INTRVL_TIME_SEC     28

#define ID_SET_FLAG                     02

#endif