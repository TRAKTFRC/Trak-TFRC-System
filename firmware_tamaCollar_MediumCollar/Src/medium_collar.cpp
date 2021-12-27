#include "mcu_call_functions.h"
#include "battery.h"
#include "pin_map.h"
#include "rtc.h"
#include "medium_collar.h"
#include "common.h"
#include "LoRa.h"

static struct tm release_time;

uint8_t generateReleasePkt (char * pkt, float vcc_v, struct tm * pkt_time)
{
	char * ptr_pkt = pkt;
	uint16_t byte_count = 0, pkt_len;

	*(ptr_pkt++) = PKT_SOH; // Start of header
	
	// Name of device
	*(ptr_pkt++) = 'C';
	byte_count = sprintf (ptr_pkt, "%d", COLLAR_NUMBER);
	ptr_pkt += byte_count;
	*(ptr_pkt++) = ','; // Adding seperator

	printf ("Main: Release Time: %d : %d : %d\r\n", pkt_time->hour, 
													pkt_time->min,
													pkt_time->sec);
	
	ptr_pkt += sprintf (ptr_pkt, "%d:%d:%d", pkt_time->hour, 
												pkt_time->min,
												pkt_time->sec); // Adding the release time
	*(ptr_pkt++) = ','; // Adding seperator
	ptr_pkt += sprintf (ptr_pkt, "%.1f", vcc_v); // Adding the Vcc Voltage

	*(ptr_pkt++) = PKT_EOH; // End of header
	*(ptr_pkt) = 0; // Adding NULL
}

void runMotor ()
{
	#ifdef MEDIUM_COLLAR
	setPinModeOutput (MOTOR_PWR_PORT, MOTOR_PWR_PIN);
	setPinHigh (MOTOR_PWR_PORT, MOTOR_PWR_PIN);
	_delay_ms (2000);
	setPinLow (MOTOR_PWR_PORT, MOTOR_PWR_PIN);
	#endif
}

void setReleaseTime ()
{
	release_time.hour = 19;
	release_time.min = 03;
	release_time.sec = 00;

	release_time.mday = 27;
	release_time.mon = 12;
	release_time.year = 21;
}

bool checkIfReleaseTime (struct tm * c_time)
{
	printf ("checkIfReleaseTime\r\n");
	printf ("Year :  %d  ----  %d\r\n", c_time->year, release_time.year);

	//if (c_time->year > release_time.year) return true;
	//else if (c_time->year < release_time.year) return false;
	//else if (c_time->year == release_time.year)
	//{
		printf ("Went past year\r\n");
		if (c_time->mon > release_time.mon) return true;
		else if (c_time->mon < release_time.mon) return false;
		else if (c_time->mon == release_time.mon)
		{
			printf ("Went past month\r\n");
			if (c_time->mday > release_time.mday) return true;
			else if (c_time->mday < release_time.mday) return false;
			else if (c_time->mday == release_time.mday)
			{
				printf ("Went past m-day\r\n");
				if (c_time->hour > release_time.hour) return true;
				else if (c_time->hour < release_time.hour) return false;
				else if (c_time->hour == release_time.hour)
				{
					printf ("Went past hour\r\n");
					if (c_time->min > release_time.min) return true;
					else if (c_time->min < release_time.min) return false;
					else if (c_time->min == release_time.min)
					{
						printf ("Went past min\r\n");
						if (c_time->sec > release_time.sec) return true;
						else if (c_time->sec < release_time.sec) return false;
						else if (c_time->sec == release_time.sec) return true;
					}
				}
			}
		}
	//}
}

void releaseHandler ()
{
	char temp_pkt [20];
	uint8_t temp_pkt_len;
	float vcc_v = readVccVoltage ();
	struct tm * c_time;
	setReleaseTime (); // Temp setting REMOVE IT
	c_time = rtc_get_time ();
	printf ("Year :  %d  ----  %d\r\n", c_time->year, release_time.year);
	printf ("Month :  %d  ----  %d\r\n", c_time->mon, release_time.mon);
	printf ("MDay :  %d  ----  %d\r\n", c_time->mday, release_time.mday);
	printf ("Hour :  %d  ----  %d\r\n", c_time->hour, release_time.hour);
/*	if (vcc_v <= RELEASE_VOLT)
	{
		printf ("releaseHandler->Voltage Low\r\n");
		runMotor ();
		temp_pkt_len = generateReleasePkt (temp_pkt, vcc_v, c_time);
		printf ("Sending: %s\r\n", temp_pkt);
		//LoRaSendSleep (temp_pkt, temp_pkt_len);
		while (1) sleepMode (); // Will stay in Sleep Forever now, untill reset
	}*/
	if (checkIfReleaseTime (c_time))
	{
		printf ("releaseHandler->It's release time\r\n");
		runMotor ();
		temp_pkt_len = generateReleasePkt (temp_pkt, vcc_v, c_time);
		printf ("Sending: %s", temp_pkt);
		//LoRaSendSleep (temp_pkt, temp_pkt_len);
		while (1) sleepMode (); // Will stay in Sleep Forever now, untill reset
	}
}