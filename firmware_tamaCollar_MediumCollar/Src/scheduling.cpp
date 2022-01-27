#include "scheduling.h"
#include "mcu_call_functions.h"

uint8_t SchedulingManage::checkIfInSendTime (tm * given_time)
{
	uint8_t inside_time_flag = 0;
	uint8_t end_time_next_day = 0;

	if (this->start_time.hour < this->end_time.hour)
	{
		end_time_next_day = 0;
	}
	else if (this->start_time.hour > this->end_time.hour)
	{
		// Next day
		end_time_next_day = 1;
	}
	else if (this->start_time.hour == this->end_time.hour)
	{
		if (this->start_time.min < this->end_time.min)
		{
			end_time_next_day = 0;
		}
		else if (this->start_time.min > this->end_time.min)
		{
			// Next day
			end_time_next_day = 1;
		}
	}

	if (end_time_next_day)
	{
		// Logic to check if in time when end time is next day
		if ((given_time->hour >= this->start_time.hour) &&
			(given_time->hour <= 23))
		{
			// Still in Send time
			inside_time_flag = 1;
		}
		else if (given_time->hour < this->end_time.hour)
		{
			// Still in Send time
			inside_time_flag = 1;
		}
		else if ((given_time->hour == this->end_time.hour) &&
				 (given_time->min < this->end_time.min))
		{
			// Still in Send time
			inside_time_flag = 1;
		}
		else
		{
			// Not in send time
			inside_time_flag = 0;
		}
	}
	else 
	{
		if ((given_time->hour >= this->start_time.hour) &&
			(given_time->hour < this->end_time.hour))
		{
			// Still in Send time
			inside_time_flag = 1;
		}
		else if ((given_time->hour == this->end_time.hour) &&
					(given_time->min < this->end_time.min))
		{
			// Still in Send time
			inside_time_flag = 1;
		}
		else
		{
			// Not in send time
			inside_time_flag = 0;
		}
	}
	return inside_time_flag;
}


void SchedulingManage::_calcNextAlarm (tm * given_time)
{
	this->_next_alarm.hour = (given_time->hour + this->send_interval.hour) % 24;
	this->_next_alarm.min = (given_time->min + this->send_interval.min) % 60;

	if ((given_time->min + this->send_interval.min) >= 60)
		this->_next_alarm.hour = (1 + this->_next_alarm.hour) % 24;
}


void SchedulingManage::alarmHandler ()
{
	this->_calcNextAlarm (&(this->wakeup_time));
	printf ("schedule->alarmHandler:Next alarm for: %d / %d\r\n", this->_next_alarm.hour, this->_next_alarm.min);
	if (this->checkIfInSendTime (&(this->_next_alarm)))
	{
		rtc_set_alarm_s (this->_next_alarm.hour, this->_next_alarm.min, 0);
		printf ("schedule->alarmHandler: True Setting alarm for: %d / %d\r\n", this->_next_alarm.hour, this->_next_alarm.min);
	}
	else
	{
		rtc_set_alarm_s (this->start_time.hour, this->start_time.min, 0);
		printf ("schedule->alarmHandler: False Setting alarm for: %d / %d\r\n", this->start_time.hour, this->start_time.min);
	}
}
