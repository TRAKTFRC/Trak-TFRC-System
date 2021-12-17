#include "mcu_call_functions.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "main.h"
#include "common.h"
#include "pin_map.h"
#include <stdarg.h>
#include <avr/sleep.h>

volatile unsigned long timer1_millis;
volatile unsigned char rcv_char;
unsigned char printf_en = 1;
static FILE * stream_uart_stdout = fdevopen(write_char, NULL);

void ra485_send_char (char out_ch);

void setPinModeInput (char port, char pin)
{
    switch (port)
    {
/*    case 'A':
        SET_PIN_MODE_INPUT(A,pin);
        break;*/
    
    case 'B':
        SET_PIN_MODE_INPUT(B,pin);
        break;

    case 'C':
        SET_PIN_MODE_INPUT(C,pin);
        break;

    case 'D':
        SET_PIN_MODE_INPUT(D,pin);
        break;

    default:
        break;
    }
    return;
}

void setPinModeOutput (char port, char pin)
{
    switch (port)
    {
/*    case 'A':
        SET_PIN_MODE_OUTPUT(A,pin);
        break;*/
    
    case 'B':
        SET_PIN_MODE_OUTPUT(B,pin);
        break;

    case 'C':
        SET_PIN_MODE_OUTPUT(C,pin);
        break;

    case 'D':
        SET_PIN_MODE_OUTPUT(D,pin);
        break;

    default:
        break;
    }
    return;
}

void setPinHigh (char port, char pin)
{
    switch (port)
    {
/*    case 'A':
        SET_PIN(A,pin);
        break;*/
    
    case 'B':
        SET_PIN(B,pin);
        break;

    case 'C':
        SET_PIN(C,pin);
        break;

    case 'D':
        SET_PIN(D,pin);
        break;

    default:
        break;
    }
    return;
}

void setPinLow (char port, char pin)
{
    switch (port)
    {
/*    case 'A':
        RESET_PIN(A,pin);
        break;*/
    
    case 'B':
        RESET_PIN(B,pin);
        break;

    case 'C':
        RESET_PIN(C,pin);
        break;

    case 'D':
        RESET_PIN(D,pin);
        break;

    default:
        break;
    }
    return;
}

void setPinState (char port, char pin, char to)
{
    switch (to)
    {
    case GPIO_PIN_RESET:
        setPinLow (port, pin);
        break;
    
    case GPIO_PIN_SET:
        setPinHigh (port, pin);
        break;

    default:
        break;
    }
}

char readPin (char port, char pin)
{
    switch (port)
    {
/*    case 'A':
        return READ_PIN(A,pin);*/

    case 'B':
        return READ_PIN(B,pin);

    case 'C':
        return READ_PIN(C,pin);

    case 'D':
        return READ_PIN(D,pin);

    default:
        break;
    }
    return 1;
}

void avrHALInit ()
{   
    // Start timers
//    startmSTimer ();
//    startuSTimer ();

    // Enable global interrupt
//    sei();
}

/*
F Clk 16 Mhz
It can do max 32768 uS
*/
void startuSTimer ()//(unsigned int time_is_us)
{
    // This is done because clock is 16 MHz and prescaler
    // is divide by 8, so the frequency are getting is 2 MHz
    // or 1 count in 0.5 uS, so to get 1 uS we need 2 counts
    //unsigned int timer_count = time_is_us;// * 2;
/*    printf ("timer_count : %d\r\n", timer_count);
    // Clear the timer
//    TCNT1L = 0;
    TCNT1H = 0;
    // Set on CTC Mode
//    TCCR1A |= (1<<WGM12);
    // Load the uS count value
    OCR1AL = timer_count;
    printf ("OCR1AL : %d\r\n",OCR1AL);
    OCR1AH = (timer_count >> 8);
    printf ("OCR1AH : %d\r\n",OCR1AH);
    // Setting up interrupt
    TIFR1 = 0;
    TIMSK1 |= (1<<OCIE1A);*/
    // Start Timer
    TCCR1B |= (1<<CS11);
}

void stopuSTimer ()
{
    TCCR1B = 0;
}
/*
ISR(TIMER1_COMPA_vect)
{
    appMicroSecCallback ();
    TIFR1 = 0; // Clear Interrupt status
    stopuSTimer ();
}*/

unsigned int readuSTimer ()
{
    unsigned int ret_val;
    ret_val = TCNT1L;
    ret_val |= (TCNT1H << 8);
    // Since each count is 0.5uS,
    // Need to send count half of the count to get uS
    return (ret_val / 2);
}

/*
F Clk 4 Mhz
Prescaller 128
Interrupt after 31 count, hence (4000000 / 128) / 31 = 1008 Hz ~ 1 mS

TCCR2A
    WGM21 // CTC mode. Gives TOP IT when TCNT0 reach OCR0A 

TCCR2B
    CS20 // No Prescaller

OCR2A = 31 // Setting the compare value

TIFR2 = 0 // Clear Interrupt status

TIMSK2
    OCIE2A // Enable interrupt for CTC
*/
void startmSTimer ()
{
    TCNT2 = 0; // Clear timer
    TCCR2A |= (1<<WGM21);
    OCR2A = 31;
    TIFR2 = 0;
    TIMSK2 |= (1<<OCIE2A);
    TCCR2B |= (1<<CS20) | (1<<CS22); // Start timer
}

void stopmSTimer ()
{
    TCCR2B = 0;
}

ISR(TIMER2_COMPA_vect)
{
    appMilliSecCallback ();
    TIFR2 = 0; // Clear Interrupt status
    timer1_millis ++;
}


unsigned long millis (void)
{
  unsigned long millis_return;
  
  millis_return = timer1_millis;
  return millis_return;
}


/* Interrupt Service Routine for Receive Complete 
NOTE: vector name changes with different AVRs see AVRStudio -
Help - AVR-Libc reference - Library Reference - <avr/interrupt.h>: Interrupts
for vector names other than USART_RXC_vect for ATmega32 */
ISR(USART_RX_vect)
{
    rcv_char = UDR0;
    appRS485RcvCallback (rcv_char);
    UCSR0A = UART_CLR_STAT; // Clear the UASRT status register
}

void USART_Init(void)
{
    stdout = stream_uart_stdout;
        
    // Set baud rate
    UBRR0L = BAUD_PRESCALE;// Load lower 8-bits into the low byte of the UBRR register
    UBRR0H = (BAUD_PRESCALE >> 8); 
	 /* Load upper 8-bits into the high byte of the UBRR register
        Default frame format is 8 data bits, no parity, 1 stop bit
        to change use UCSRC, see AVR datasheet*/ 

    // Enable receiver and transmitter and receive complete interrupt 
    UCSR0B = ((1<<TXEN0) | (1<<RXEN0) | (1<<RXCIE0));
    UCSR0A = UART_CLR_STAT;                                   // Clear the UASRT status register
    //printf ("UART Started \r\n");

    return;
}

int write_char (char var, FILE *stream)
{
    if (printf_en)
    {
        ra485_send_char (var);
        return SUCCESS;
    }
    return FAILURE;
}

void ra485_send_char (char out_ch)
{
    while (!( UCSR0A & (1<<UDRE0))); /* Wait for empty transmit buffer       */
    UDR0 = out_ch;					 /* Put data into buffer, sends the data */
    return;
}

int printLen (char* data, int len)
{
    for (int loop_counter=0; loop_counter < len; loop_counter++)
    {
        ra485_send_char (data[loop_counter]);
    }
    return len;
}

void sleepMode ()
{
    uint8_t mcucr2, mcucr1;
    sleep_enable ();
    set_sleep_mode (SLEEP_MODE_PWR_DOWN);
    sei ();
    sleep_cpu ();
    sleep_disable ();
}

ISR (INT0_vect)          //External interrupt_zero ISR
{
    EIFR |= (1 << INT0);
}