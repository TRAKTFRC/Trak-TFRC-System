#ifndef __MEDIUM_COLLAR_H
#define __MEDIUM_COLLAR_H

#define RELEASE_VOLT	31

void releaseHandler (uint8_t wake_batt_volt);
void firstTimeMOtorRoutine ();

#define MOTOR_RUN_DELAY		3500

#endif