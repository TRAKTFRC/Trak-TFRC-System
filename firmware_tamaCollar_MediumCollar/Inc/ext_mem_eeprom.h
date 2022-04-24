#ifndef _EXT_MEM_EEPROM_H_
#define _EXT_MEM_EEPROM_H_

#include "common.h"
#include "TinyGPS++.h"

#define EXT_EROM_NEX_ADR_LOW        25
#define EXT_EROM_NEX_ADR_HIGH       26       

#define EXT_EROM_PKT_START_ADR      50

#define EXT_EROM_HIGHEST_ADR        16383
#define EXT_EROM_SLAVE_ADR          0b10100000
#define MAX_PKT_STORE_LEN           50

void storeDataPointInEEPROM (TinyGPSPlus &gps_data, struct tm * time);
void dumpEEPROMPkt ();

#endif
