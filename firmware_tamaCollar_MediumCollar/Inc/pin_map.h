#ifndef __PIN_MAP_H
#define __PIN_MAP_H

/* GPS Pins Mapping */
#ifndef MEDIUM_COLLAR
#define GPS_ON_OFF_PULSE_PORT       'D'
#define GPS_ON_OFF_PULSE_PIN       	3
#define	LORA_SS_PORT				'D'
#define LORA_SS_PIN					7
#define	LORA_RST_PORT				'B'
#define LORA_RST_PIN				2
#define SOFTUART_BAUD_RATE      	4800
#endif

#ifdef MEDIUM_COLLAR
#define GPS_ON_OFF_PULSE_PORT       'D'
#define GPS_ON_OFF_PULSE_PIN       	4
#define	LORA_SS_PORT				'C'
#define LORA_SS_PIN					0
#define	LORA_RST_PORT				'C'
#define LORA_RST_PIN				1
#define SOFTUART_BAUD_RATE      	9600
#define MOTOR_PWR_PORT				'D'
#define MOTOR_PWR_PIN				7
#endif

#endif
