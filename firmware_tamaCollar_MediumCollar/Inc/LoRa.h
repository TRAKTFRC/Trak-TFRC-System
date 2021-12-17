#ifndef LORA_H
#define LORA_H

#include "spi.h"

/*@{*/ //SETUP
// SPI
#define LORA_DEFAULT_SPI_FREQUENCY  8000000UL

// registers
#define REG_FIFO                    0x00
#define REG_OP_MODE                 0x01
#define REG_FRF_MSB                 0x06
#define REG_FRF_MID                 0x07
#define REG_FRF_LSB                 0x08
#define REG_PA_CONFIG               0x09
#define REG_LNA                     0x0c
#define REG_FIFO_ADDR_PTR           0x0d
#define REG_FIFO_TX_BASE_ADDR       0x0e
#define REG_FIFO_RX_BASE_ADDR       0x0f
#define REG_FIFO_RX_CURRENT_ADDR    0x10
#define REG_IRQ_FLAGS               0x12
#define REG_RX_NB_BYTES             0x13
#define REG_PKT_SNR_VALUE           0x19
#define REG_PKT_RSSI_VALUE          0x1a
#define REG_MODEM_CONFIG_1          0x1d
#define REG_MODEM_CONFIG_2          0x1e
#define REG_PREAMBLE_MSB            0x20
#define REG_PREAMBLE_LSB            0x21
#define REG_PAYLOAD_LENGTH          0x22
#define REG_MODEM_CONFIG_3          0x26
#define REG_FREQ_ERROR_MSB          0x28
#define REG_FREQ_ERROR_MID          0x29
#define REG_FREQ_ERROR_LSB          0x2a
#define REG_RSSI_WIDEBAND           0x2c
#define REG_DETECTION_OPTIMIZE      0x31
#define REG_DETECTION_THRESHOLD     0x37
#define REG_SYNC_WORD               0x39
#define REG_DIO_MAPPING_1           0x40
#define REG_VERSION                 0x42
// modes
#define MODE_LONG_RANGE_MODE        0x80
#define MODE_SLEEP                  0x00
#define MODE_STDBY                  0x01
#define MODE_TX                     0x03
#define MODE_RX_CONTINUOUS          0x05
#define MODE_RX_SINGLE              0x06
// PA config
#define PA_BOOST                    0x80
// IRQ masks
#define IRQ_TX_DONE_MASK            0x08
#define IRQ_PAYLOAD_CRC_ERROR_MASK  0x20
#define IRQ_RX_DONE_MASK            0x40
// package
#define MAX_PKT_LENGTH              255
/*@}*/

void LoRaSendSleep (const char *buffer, uint8_t size);

uint8_t begin(uint32_t frequency);
void LoRaend ();

uint8_t beginPacket(uint8_t implicitHeader);
uint8_t endPacket();

uint8_t parsePacket(uint8_t size);
int8_t packetRssi();
uint8_t packetSnr();
int32_t packetFrequencyError();

uint8_t write(const char *buffer, uint8_t size);

int8_t available();
int8_t read();
int8_t peek();

void receive(int size);

void idle();
void sleep();

void setTxPower(uint8_t level);
void setFrequency(uint32_t frequency);
void setSpreadingFactor(uint8_t sf);
void setSignalBandwidth(uint32_t sbw);
void setCodingRate4(uint8_t denominator);
void setPreambleLength(uint16_t length);
void setSyncWord(uint8_t sw);
void enableCrc();
void disableCrc();

void _explicitHeaderMode();
void _implicitHeaderMode();
uint8_t _getSpreadingFactor();
uint32_t _getSignalBandwidth();
void _setLdoFlag();
uint8_t _readRegister(uint8_t address);
void _writeRegister(uint8_t address, uint8_t value);
uint8_t _singleTransfer(uint8_t address, uint8_t value);
#endif
