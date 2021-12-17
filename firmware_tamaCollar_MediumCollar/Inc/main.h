#ifndef __USR_MAIN_H
#define __USR_MAIN_H

#include "common.h"

// For this hader file itself
#define PKT_WAIT_SOH            0
#define PKT_WAIT_EOH            1

#define PKT_SOH                 '{'
#define PKT_EOH                 '}'

#define PKT_TIMEOUT             5000 // mS

// Command detection part
#define UART_ISR_BUFF_SIZE		50

class CmdUARTInterface
{
public:
    char rcv_char;
    char isr_buff [UART_ISR_BUFF_SIZE];
    int isr_in = 0;
    int isr_out = 0;
    void timerHandler ();
    void packetDetect ();

private:
    bool _timer_state = SET;
    long _timer_count = 0;
    char _packet_state;

};

void appMilliSecCallback ();
void appMicroSecCallback ();
void appExtITCallback (unsigned int GPIO_Pin);
void appRS485RcvCallback (char rcv_char);
void startRS485UARTIT ();
void startADC (unsigned long * buffer, int channel_count);

#endif