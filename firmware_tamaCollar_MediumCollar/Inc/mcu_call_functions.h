#ifndef __MCU_CALL_FUNCTION_H
#define __MCU_CALL_FUNCTION_H

#include <avr/io.h>
#include <util/delay.h>
#include <avr/cpufunc.h> 
#include <stdio.h>
#include <avr/interrupt.h>

#ifndef F_CPU
#define F_CPU 4000000UL // 8 MHz clock speed
#endif

// GPIO
#define SET_PIN_MODE_OUTPUT(_PORT_,_PIN_)   DDR##_PORT_ |= 1 << _PIN_;
#define SET_PIN_MODE_INPUT(_PORT_,_PIN_)    DDR##_PORT_ &= ~(1 << _PIN_);

#define SET_PIN(_PORT_,_PIN_)               PORT##_PORT_ |= 1 << _PIN_;
#define RESET_PIN(_PORT_,_PIN_)             PORT##_PORT_ &= ~(1 << _PIN_);

#define READ_PIN(_PORT_,_PIN_)              ((PIN##_PORT_ &(1 << _PIN_ )) >> _PIN_ );

#define GPIO_PIN_RESET                      0
#define GPIO_PIN_SET                        1

// UART
// Define baud rate
#define USART_BAUDRATE                      19200 	// Changing here won't to anything, 
												   	// change prescaler manually 
												   	// or enable calculaion below 
//#define BAUD_PRESCALE                    	(((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

#if F_CPU == 4000000
#define BAUD_PRESCALE						25
#pragma message ("Baud Prescaler Value set to 25 for 4mHz")
#elif F_CPU == 8000000
#define BAUD_PRESCALE						51
#pragma message ("Baud Prescaler Value set to 51 for 8mHz")
#else
#error No Baud Prescaler available for given F_CPU
#endif

#define UART_CLR_STAT						0x02

#define TIMER_MAX_US						(0xFFFF / 2)

extern unsigned char printf_en;
extern volatile unsigned long timer1_millis;

// Main common function
void avrHALInit ();
// GPIO Related function
void setPinModeInput (char port, char pin);
void setPinModeOutput (char port, char pin);
void setPinHigh (char port, char pin);
void setPinLow (char port, char pin);
char readPin (char port, char pin);
void setPinState (char port, char pin, char to);
// UART Related functions
void USART_Init (void);
int write_char (char var, FILE *stream);
int printRS485 (const char *format, ...);
int printLen (char* data, int len);
void setRS485ToRcvMode ();
void setRS485ToSendMode ();
// uS Timer related functions
void startuSTimer ();//(unsigned int time_is_us);
unsigned int readuSTimer ();
// uM Timer related functions
void startmSTimer ();
void stopmSTimer ();
unsigned long millis (void);

void sleepMode ();

#endif
