#ifndef __TIMER_US_H
#define __TIMER_US_H

class SoftTimeruS
{
public:
	void start (unsigned int us_dur);
	bool checkElapsed (); // If it returns SET, then given timer is over

private:
	unsigned int _start_time = 0;
	unsigned int _time_to_check = 0;
};

#endif