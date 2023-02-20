#ifndef __USR_SCHEDULING_H
#define __USR_SCHEDULING_H

#include "rtc.h"

class SchedulingManage
{
private:
	void _calcNextAlarm (tm * given_time);
	struct tm _next_alarm;

public:
	void alarmHandler ();
	uint8_t checkIfInSendTime (tm * given_time);
	struct tm wakeup_time;
	struct tm start_time;
	struct tm end_time;
	struct tm send_interval;
};

#endif