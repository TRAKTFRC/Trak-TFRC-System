#include "mcu_call_functions.h"
#include "battery.h"
#include "pin_map.h"
#include "rtc.h"
#include "medium_collar.h"
#include "common.h"
#include "LoRa.h"
#include <string.h>

static struct tm release_time;
bool release_done_flag = false;

uint16_t generateReleasePkt (char * pkt, float vcc_v, struct tm * pkt_time)
{
	char * ptr_pkt = pkt;
	uint16_t byte_count = 0, pkt_len;

	*(ptr_pkt++) = PKT_SOH; // Start of header
	
	// Name of device
	*(ptr_pkt++) = DEVICE_CODE;
	byte_count = sprintf (ptr_pkt, "%2d", dev_id);
	ptr_pkt += byte_count;
	*(ptr_pkt++) = ','; // Adding seperator

	byte_count = sprintf (ptr_pkt, "REL,");
	ptr_pkt += byte_count;
/*
	printf ("Main: Release Time: %d : %d : %d\r\n", pkt_time->hour, 
													pkt_time->min,
													pkt_time->sec);*/
	
	ptr_pkt += sprintf (ptr_pkt, "%2d:%2d:%2d", pkt_time->hour, 
												pkt_time->min,
												pkt_time->sec); // Adding the release time
	*(ptr_pkt++) = ','; // Adding seperator
	ptr_pkt += sprintf (ptr_pkt, "%.1f", vcc_v); // Adding the Vcc Voltage

	*(ptr_pkt++) = PKT_EOH; // End of header
	*(ptr_pkt) = 0; // Adding NULL
	pkt_len = strlen (pkt);
	return pkt_len;
}

void firstTimeMOtorRoutine ()
{
	printf ("First time motor run\r\n");

	// Set pin modes
	setPinModeOutput (MOTOR_PWR_PORT, MOTOR_PWR_PIN);
	setPinHigh (MOTOR_PWR_PORT, MOTOR_PWR_PIN);
	setPinModeOutput (MOTOR_IN1_PORT, MOTOR_IN1_PIN);
	setPinLow (MOTOR_IN1_PORT, MOTOR_IN1_PIN);
	setPinModeOutput (MOTOR_IN2_PORT, MOTOR_IN2_PIN);
	setPinLow (MOTOR_IN2_PORT, MOTOR_IN2_PIN);
	
	// Enable power for motor driver
	setPinLow (MOTOR_PWR_PORT, MOTOR_PWR_PIN);
	// Motor clockwise
	setPinHigh (MOTOR_IN1_PORT, MOTOR_IN1_PIN);
	setPinLow (MOTOR_IN2_PORT, MOTOR_IN2_PIN);
	_delay_ms (MOTOR_RUN_DELAY);
	setPinLow (MOTOR_IN1_PORT, MOTOR_IN1_PIN);
	setPinLow (MOTOR_IN2_PORT, MOTOR_IN2_PIN);
	
	_delay_ms (500);
	// Motor counter clockwise
	setPinLow (MOTOR_IN1_PORT, MOTOR_IN1_PIN);
	setPinHigh (MOTOR_IN2_PORT, MOTOR_IN2_PIN);
	_delay_ms (MOTOR_RUN_DELAY);
	setPinLow (MOTOR_IN1_PORT, MOTOR_IN1_PIN);
	setPinLow (MOTOR_IN2_PORT, MOTOR_IN2_PIN);

	// Disable power for motor driver
	setPinHigh (MOTOR_PWR_PORT, MOTOR_PWR_PIN);
}

void runMotor ()
{

	// Enable power for motor driver
	setPinLow (MOTOR_PWR_PORT, MOTOR_PWR_PIN);
	// Motor clockwise
	setPinHigh (MOTOR_IN1_PORT, MOTOR_IN1_PIN);
	setPinLow (MOTOR_IN2_PORT, MOTOR_IN2_PIN);
	_delay_ms (MOTOR_RUN_DELAY);
	setPinLow (MOTOR_IN1_PORT, MOTOR_IN1_PIN);
	setPinLow (MOTOR_IN2_PORT, MOTOR_IN2_PIN);
	// Disable power for motor driver
	setPinHigh (MOTOR_PWR_PORT, MOTOR_PWR_PIN);

}

void setReleaseTime ()
{
	release_time.hour = 7;
	release_time.min = 30;
	release_time.sec = 00;

	release_time.mday = 30;
	release_time.mon = 1;
	release_time.year = 22;
}

bool checkIfReleaseTime (struct tm * c_time)
{
	//if (c_time->year > release_time.year) return true;
	//else if (c_time->year < release_time.year) return false;
	//else if (c_time->year == release_time.year)
	//{
		if (c_time->mon > release_time.mon) return true;
		else if (c_time->mon < release_time.mon) return false;
		else if (c_time->mon == release_time.mon)
		{
			if (c_time->mday > release_time.mday) return true;
			else if (c_time->mday < release_time.mday) return false;
			else if (c_time->mday == release_time.mday)
			{
				if (c_time->hour > release_time.hour) return true;
				else if (c_time->hour < release_time.hour) return false;
				else if (c_time->hour == release_time.hour)
				{
					if (c_time->min > release_time.min) return true;
					else if (c_time->min < release_time.min) return false;
					else if (c_time->min == release_time.min)
					{
						if (c_time->sec > release_time.sec) return true;
						else if (c_time->sec < release_time.sec) return false;
						else if (c_time->sec == release_time.sec) return true;
					}
				}
			}
		}
	//}
	return false;
}

void releaseHandler (uint8_t wake_batt_volt)
{
	char temp_pkt [20];
	uint16_t temp_pkt_len;
	struct tm * c_time;
	setReleaseTime (); // Temp setting REMOVE IT
	if (release_done_flag) return;
	c_time = rtc_get_time ();
	if (wake_batt_volt <= RELEASE_VOLT)
	{
		printf ("releaseHandler->Volt Low\r\n");
		runMotor ();
		temp_pkt_len = generateReleasePkt (temp_pkt, wake_batt_volt, c_time);
		printf ("Sending: %s\r\n", temp_pkt);
		LoRaSendSleep (temp_pkt, temp_pkt_len);
		release_done_flag = true;
		//while (1) sleepMode (); // Will stay in Sleep Forever now, untill reset
	}
	if (checkIfReleaseTime (c_time))
	{
		printf ("releaseHandler->Release time\r\n");
		runMotor ();
		temp_pkt_len = generateReleasePkt (temp_pkt, wake_batt_volt, c_time);
		printf ("Sending: %s\r\n", temp_pkt);
		LoRaSendSleep (temp_pkt, temp_pkt_len);
		release_done_flag = true;
		//while (1) sleepMode (); // Will stay in Sleep Forever now, untill reset
	}
}