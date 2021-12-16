#include "timer_us.h"
#include "common.h"

void SoftTimeruS::start (unsigned int us_dur)
{
	this->_start_time = readuSTimer ();
	this->_time_to_check = us_dur;
	return; 
}

bool SoftTimeruS::checkElapsed ()
{
	unsigned int current_time = readuSTimer ();
	// Check if timer over ran and came back from bottom
	if (this->_time_to_check)
	{
		if (current_time > this->_start_time)
		{
			if ((current_time - this->_start_time) >= this->_time_to_check)
				return SET;
			else
				return RESET;	
		}
		else
		{
			if ((((TIMER_MAX_US) - this->_start_time) + current_time) >= this->_time_to_check)
				return SET;
			else
				return RESET;
		}
	}
	return SET;
}

