#ifndef _24C64_H_
#define _24C64_H_

#include "common.h"

#define FALSE 0
#define TRUE 1

void EEOpen();
uint8_t EEWriteByte(uint16_t,uint8_t);
uint8_t EEReadByte(uint16_t address);
uint8_t EEWriteString (uint16_t start_address, char * data, uint8_t data_len);

#endif
