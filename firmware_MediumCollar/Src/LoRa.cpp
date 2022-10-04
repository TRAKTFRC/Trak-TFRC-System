#include <avr/io.h>
#include <util/delay.h>
#include "LoRa.h"
#include "stdio.h"
#include "mcu_call_functions.h"
#include "pin_map.h"

uint32_t frequency = 0;
uint8_t packetIndex = 0;
uint8_t implicitHeaderMode = 0;

void LoRaInit ()
{
    setPinModeOutput ('B', 2);
    setPinLow ('B', 2);
	spi_init (0, 1, 0, 0, 0);                // MSB, master, mode0, F/4, x1
    if (!begin (915E6)) 
	{
        printf ("Starting LoRa failed!\r\n");
        while (1);
    }
    setSignalBandwidth (31250);
}

void LoRaSendSleep (const char *buffer, uint8_t size)
{
    LoRaInit ();
    beginPacket (0);
    write (buffer, size);
    endPacket ();
    LoRaend ();
}

uint8_t begin(uint32_t frequency) 
{
    setPinModeOutput (LORA_SS_PORT, LORA_SS_PIN);
    setPinHigh (LORA_SS_PORT, LORA_SS_PIN);
    setPinModeOutput (LORA_RST_PORT, LORA_RST_PIN);
    setPinLow (LORA_RST_PORT, LORA_RST_PIN);
    _delay_ms(10);
    setPinHigh (LORA_RST_PORT, LORA_RST_PIN);
    _delay_ms(10);

    uint8_t version = _readRegister(REG_VERSION);           // check the version
    if (version != 0x12)
    {
        return 0;
    }
    sleep();                                                // put in sleep mode
    setFrequency(frequency);                                // set frequency
    _writeRegister(REG_FIFO_TX_BASE_ADDR, 0);               // set base addresses
    _writeRegister(REG_FIFO_RX_BASE_ADDR, 0);
    _writeRegister(REG_LNA, _readRegister(REG_LNA) | 0x03); // set LNA boost
    _writeRegister(REG_MODEM_CONFIG_3, 0x04);               // set auto AGC
    setTxPower(20);                                         // set power to 17 dBm
    idle();                                                 // put in standby mode
    return 1;
}

void LoRaend () {
    sleep();
}

uint8_t beginPacket(uint8_t implicitHeader) {
    idle();                                             // put in standby mode
    if (implicitHeader) _implicitHeaderMode();
    else                _explicitHeaderMode();
    _writeRegister(REG_FIFO_ADDR_PTR, 0);                // reset FIFO address
    _writeRegister(REG_PAYLOAD_LENGTH, 0);               // and paload length
    return 1;
}

uint8_t endPacket() {
    // put in TX mode
    _writeRegister(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_TX);
    // wait for TX done
    while ((_readRegister(REG_IRQ_FLAGS) & IRQ_TX_DONE_MASK) == 0)
    {
        _delay_ms (10);
    }
    // clear IRQ's
    _writeRegister(REG_IRQ_FLAGS, IRQ_TX_DONE_MASK);
    return 1;
}

uint8_t parsePacket(uint8_t size) {
    int packetLength = 0;
    int irqFlags = _readRegister(REG_IRQ_FLAGS);
    if (size > 0) {
        _implicitHeaderMode();
        _writeRegister(REG_PAYLOAD_LENGTH, size & 0xff);
    } else {
        _explicitHeaderMode();
    }
    // clear IRQ's
    _writeRegister(REG_IRQ_FLAGS, irqFlags);
    if ((irqFlags & IRQ_RX_DONE_MASK) && (irqFlags & IRQ_PAYLOAD_CRC_ERROR_MASK) == 0) {
        packetIndex = 0;                                // received a packet
        // read packet length
        if (implicitHeaderMode)
            packetLength = _readRegister(REG_PAYLOAD_LENGTH);
        else
            packetLength = _readRegister(REG_RX_NB_BYTES);
        // set FIFO address to current RX address
        _writeRegister(REG_FIFO_ADDR_PTR, _readRegister(REG_FIFO_RX_CURRENT_ADDR));
        // put in standby mode
        idle();
    } else if (_readRegister(REG_OP_MODE) != (MODE_LONG_RANGE_MODE | MODE_RX_SINGLE)) {
        // not currently in RX mode
        _writeRegister(REG_FIFO_ADDR_PTR, 0);            // reset FIFO address
        // put in single RX mode
        _writeRegister(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_RX_SINGLE);
    }
    return packetLength;
}

int8_t packetRssi() {
    return (_readRegister(REG_PKT_RSSI_VALUE) - (frequency < 868000000 ? 164 : 157));
}

uint8_t packetSnr() {
    return ((int8_t)_readRegister(REG_PKT_SNR_VALUE)) >> 2;
}

int32_t packetFrequencyError() {
    int32_t freqError = 0;
    freqError = (int32_t)(_readRegister(REG_FREQ_ERROR_MSB) & 0b111);
    freqError <<= 8L;
    freqError += (int32_t)(_readRegister(REG_FREQ_ERROR_MID));
    freqError <<= 8L;
    freqError += (int32_t)(_readRegister(REG_FREQ_ERROR_LSB));
    if (_readRegister(REG_FREQ_ERROR_MSB) & 0b1000)   // Sign bit is on
        freqError -= 524288;                        // 0b1000'0000'0000'0000'0000
    const float fXtal = 32000000; // FXOSC: crystal oscillator (XTAL) frequency (2.5. Chip Specification, p. 14)
    const float fError = (((float)freqError * (1L << 24)) / fXtal) * (_getSignalBandwidth() / 500000.0f); // p. 37
    return (int32_t)fError;
}

uint8_t write(const char *buffer, uint8_t size) {
    uint16_t currentLength = _readRegister(REG_PAYLOAD_LENGTH);
    if ((currentLength + size) > MAX_PKT_LENGTH)                // check size
        size = MAX_PKT_LENGTH - currentLength;
    for (uint8_t i=0; i<size; i++) {                         // write data
        _writeRegister(REG_FIFO, buffer[i]);
    }
    _writeRegister(REG_PAYLOAD_LENGTH, currentLength + size);    // update length
    return size;
}

int8_t available() {
    return (_readRegister(REG_RX_NB_BYTES) - packetIndex);
}

int8_t read() {
    if (!available())
        return -1;
    packetIndex++;
    return _readRegister(REG_FIFO);
}

int8_t peek() {
    if (!available())
        return -1;
    int currentAddress = _readRegister(REG_FIFO_ADDR_PTR); // store FIFO address
    uint8_t b = _readRegister(REG_FIFO);                   // read
    _writeRegister(REG_FIFO_ADDR_PTR, currentAddress);     // restore FIFO address
    return b;
}

void receive(int size) {
    if (size > 0) {
        _implicitHeaderMode();
        _writeRegister(REG_PAYLOAD_LENGTH, size & 0xff);
    } else {
        _explicitHeaderMode();
    }
        _writeRegister(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_RX_CONTINUOUS);
}

void idle() {
    _writeRegister(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_STDBY);
}

void sleep() {
    _writeRegister(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_SLEEP);
}

void setTxPower(uint8_t level) {
    if (level < 2)       level = 2;
    else if (level > 17) level = 17;
    _writeRegister(REG_PA_CONFIG, PA_BOOST | (level - 2));
}

void setFrequency(uint32_t freq) {
    frequency = freq;
    uint64_t frf = ((uint64_t)freq << 19) / 32000000;
    _writeRegister(REG_FRF_MSB, (uint8_t)(frf >> 16));
    _writeRegister(REG_FRF_MID, (uint8_t)(frf >> 8));
    _writeRegister(REG_FRF_LSB, (uint8_t)(frf >> 0));
}

void setSpreadingFactor(uint8_t sf) {
    if (sf < 6)       sf = 6;
    else if (sf > 12) sf = 12;

    if (sf == 6) {
        _writeRegister(REG_DETECTION_OPTIMIZE, 0xc5);
        _writeRegister(REG_DETECTION_THRESHOLD, 0x0c);
    } else {
        _writeRegister(REG_DETECTION_OPTIMIZE, 0xc3);
        _writeRegister(REG_DETECTION_THRESHOLD, 0x0a);
    }
    _writeRegister(REG_MODEM_CONFIG_2, (_readRegister(REG_MODEM_CONFIG_2) & 0x0f) | ((sf << 4) & 0xf0));
    _setLdoFlag();
}

void setSignalBandwidth(uint32_t sbw) {
    uint8_t bw;
    if (sbw <= 7800)        bw = 0;
    else if (sbw <= 10400)  bw = 1;
    else if (sbw <= 15600)  bw = 2;
    else if (sbw <= 20800)  bw = 3;
    else if (sbw <= 31250)  bw = 4;
    else if (sbw <= 41700)  bw = 5;
    else if (sbw <= 62500)  bw = 6;
    else if (sbw <= 125000) bw = 7;
    else if (sbw <= 250000) bw = 8;
    else                    bw = 9;
    _writeRegister(REG_MODEM_CONFIG_1, (_readRegister(REG_MODEM_CONFIG_1) & 0x0f) | (bw << 4));
    _setLdoFlag();
}

void setCodingRate4(uint8_t denominator) {
    if (denominator < 5)      denominator = 5;
    else if (denominator > 8) denominator = 8;

    int cr = denominator - 4;
    _writeRegister(REG_MODEM_CONFIG_1, (_readRegister(REG_MODEM_CONFIG_1) & 0xf1) | (cr << 1));
}

void setPreambleLength(uint16_t length) {
    _writeRegister(REG_PREAMBLE_MSB, (uint8_t)(length >> 8));
    _writeRegister(REG_PREAMBLE_LSB, (uint8_t)(length >> 0));
}

void setSyncWord(uint8_t sw) {
    _writeRegister(REG_SYNC_WORD, sw);
}

void enableCrc() {
    _writeRegister(REG_MODEM_CONFIG_2, _readRegister(REG_MODEM_CONFIG_2) | 0x04);
}

void disableCrc() {
    _writeRegister(REG_MODEM_CONFIG_2, _readRegister(REG_MODEM_CONFIG_2) & 0xfb);
}

void _explicitHeaderMode() {
    implicitHeaderMode = 0;
    _writeRegister(REG_MODEM_CONFIG_1, _readRegister(REG_MODEM_CONFIG_1) & 0xfe);
}

void _implicitHeaderMode() {
    implicitHeaderMode = 1;
    _writeRegister(REG_MODEM_CONFIG_1, _readRegister(REG_MODEM_CONFIG_1) | 0x01);
}

uint8_t _getSpreadingFactor() {
    return _readRegister(REG_MODEM_CONFIG_2) >> 4;
}

uint32_t _getSignalBandwidth() {
    uint8_t bw = (_readRegister(REG_MODEM_CONFIG_1) >> 4);
    switch (bw) {
        case 0: return 7800;
        case 1: return 10400;
        case 2: return 15600;
        case 3: return 20800;
        case 4: return 31250;
        case 5: return 41700;
        case 6: return 62500;
        case 7: return 125000;
        case 8: return 250000;
        case 9: return 500000;
        default: return 500000;
    }
}

void _setLdoFlag() {
    // Section 4.1.1.5
    long symbolDuration = 1000 / ( _getSignalBandwidth() / (1L << _getSpreadingFactor()) ) ;
    // Section 4.1.1.6
    uint8_t config3 = _readRegister(REG_MODEM_CONFIG_3);
    if (symbolDuration > 16) config3 |= 1<<3;
    else                     config3 &= ~(1<<3);
    _writeRegister(REG_MODEM_CONFIG_3, config3);
}

uint8_t _readRegister(uint8_t address) {
    return _singleTransfer(address & 0x7f, 0x00);
}

void _writeRegister(uint8_t address, uint8_t value) {
    _singleTransfer(address | 0x80, value);
}

uint8_t _singleTransfer(uint8_t address, uint8_t value) {
    uint8_t response;
    setPinLow (LORA_SS_PORT, LORA_SS_PIN);
    spi_send(address);
    response = spi_send(value);
    setPinHigh (LORA_SS_PORT, LORA_SS_PIN);
    return response;
}
