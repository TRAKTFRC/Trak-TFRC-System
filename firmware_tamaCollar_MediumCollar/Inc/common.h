#ifndef __COMMON_H
#define __COMMON_H

#include "mcu_call_functions.h"

#define SET                     1
#define RESET                   0

#define SUCCESS                 0
#define FAILURE                 1



#ifndef MEDIUM_COLLAR
#define DEVICE_CODE				'C'
#endif

#ifdef MEDIUM_COLLAR
#define DEVICE_CODE				'D'
#endif

#define PKT_SOH                 '{'
#define PKT_EOH                 '}'

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