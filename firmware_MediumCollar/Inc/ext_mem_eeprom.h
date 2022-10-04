#ifndef _EXT_MEM_EEPROM_H_
#define _EXT_MEM_EEPROM_H_

#include "common.h"
#include "TinyGPS++.h"


#define PKT_STORE_LEN                   43
#define PKT_MAX_COUNT                   450

#define EXT_EROM_DATA_IN_CNTR_ADR       25
#define EXT_EROM_DATA_OUT_CNTR_ADR      27
#define EXT_EROM_PKT_START_ADR          50

#define EXT_EROM_HIGHEST_ADR            16383
#define EXT_EROM_SLAVE_ADR              0b10100000

#define MAX_PKT_STORE_LEN               50

void storeDataPointInEEPROM (TinyGPSPlus &gps_data);
void dumpEEPROMPkt ();
void incrOutCntr ();
bool checkGetNextPkt (char * ret_pkt);

#endif
