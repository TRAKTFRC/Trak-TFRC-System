#ifndef __PIN_MAP_H
#define __PIN_MAP_H

/* GPS Pins Mapping */
#define GPS_ON_OFF_PULSE_PORT       'D'
#define GPS_ON_OFF_PULSE_PIN       	4
#define	LORA_SS_PORT				'C'
#define LORA_SS_PIN					0
#define	LORA_RST_PORT				'C'
#define LORA_RST_PIN				1
#define SOFTUART_BAUD_RATE      	9600

// Motor Section
#define MOTOR_PWR_PORT				'D'
#define MOTOR_PWR_PIN				7
#define MOTOR_IN2_PORT				'D'
#define MOTOR_IN2_PIN				6
#define MOTOR_IN1_PORT				'D'
#define MOTOR_IN1_PIN				5

// Soft UART Related stuff
#define SOFTUART_RXPIN   PINB
#define SOFTUART_RXDDR   DDRB
#define SOFTUART_RXBIT   PB1

#define SOFTUART_TXPORT  PORTB
#define SOFTUART_TXDDR   DDRB
#define SOFTUART_TXBIT   PB0

#endif
