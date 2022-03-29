#include "command_layer.h"
#include <string.h>
#include <stdio.h>
#include "rtc.h"
#include "common.h"
#include "eeprom_hal.h"
#include "scheduling.h"

extern bool rtc_time_set_flag;
extern SchedulingManage schedule;

#define READ_COUNT_HANDLE(c) if ((this->_readCountManage (c)) == FAILURE) return;

void CmdProcess::init ()
{
}

/**
 * @brief
 * @details
 */
void CmdProcess::detect ()
{
	struct tm rcv_time;
	uint8_t temp_dev_id;

	//printf ("cmd.detect->Processing command\r\n");
	this->_read_counter = 1;
	
	switch (this->buff [this->_read_counter])
	{
	case 'R': // Packet been read here: {R,09:33:40,17/12/21}
		READ_COUNT_HANDLE (2);
		rcv_time.hour = (this->buff [this->_read_counter] - '0') * 10;
		READ_COUNT_HANDLE (1); rcv_time.hour = this->buff [this->_read_counter] - '0';
		READ_COUNT_HANDLE (2);
		rcv_time.min = (this->buff [this->_read_counter] - '0') * 10;
		READ_COUNT_HANDLE (1); rcv_time.min = this->buff [this->_read_counter] - '0';
		READ_COUNT_HANDLE (2);
		rcv_time.sec = (this->buff [this->_read_counter] - '0') * 10;
		READ_COUNT_HANDLE (1); rcv_time.sec = this->buff [this->_read_counter] - '0';

		READ_COUNT_HANDLE (2);
		rcv_time.mday = (this->buff [this->_read_counter] - '0') * 10;
		READ_COUNT_HANDLE (1); rcv_time.mday = this->buff [this->_read_counter] - '0';
		READ_COUNT_HANDLE (2);
		rcv_time.mon = (this->buff [this->_read_counter] - '0') * 10;
		READ_COUNT_HANDLE (1); rcv_time.mon = this->buff [this->_read_counter] - '0';
		READ_COUNT_HANDLE (2);
		rcv_time.year = (this->buff [this->_read_counter] - '0') * 10;
		READ_COUNT_HANDLE (1); rcv_time.year = this->buff [this->_read_counter] - '0';

		rtc_set_time (&rcv_time);
		rtc_time_set_flag = true;
		printf ("cmd.detect-RTC set\r\n");
		break;

	case 'S': // Packet been read here: {R,09:33:40,20:30:40,00:30:00}
		READ_COUNT_HANDLE (2);
		schedule.start_time.hour = (this->buff [this->_read_counter] - '0') * 10;
		READ_COUNT_HANDLE (1); schedule.start_time.hour = this->buff [this->_read_counter] - '0';
		READ_COUNT_HANDLE (2);
		schedule.start_time.min = (this->buff [this->_read_counter] - '0') * 10;
		READ_COUNT_HANDLE (1); schedule.start_time.min = this->buff [this->_read_counter] - '0';
		READ_COUNT_HANDLE (2);
		schedule.start_time.sec = (this->buff [this->_read_counter] - '0') * 10;
		READ_COUNT_HANDLE (1); schedule.start_time.sec = this->buff [this->_read_counter] - '0';
		storeTimeInEEPROM (&(schedule.start_time), EEPROM_ADDR_START_TIME_HR);
		redTimeFromEEPROM (&(schedule.start_time), EEPROM_ADDR_START_TIME_HR);
		printf ("cmd.detect Start Time: %d : %d : %d\r\n", schedule.start_time.hour, 
													   schedule.start_time.min,
													   schedule.start_time.sec);

		READ_COUNT_HANDLE (2);
		schedule.end_time.hour = (this->buff [this->_read_counter] - '0') * 10;
		READ_COUNT_HANDLE (1); schedule.end_time.hour = this->buff [this->_read_counter] - '0';
		READ_COUNT_HANDLE (2);
		schedule.end_time.min = (this->buff [this->_read_counter] - '0') * 10;
		READ_COUNT_HANDLE (1); schedule.end_time.min = this->buff [this->_read_counter] - '0';
		READ_COUNT_HANDLE (2);
		schedule.end_time.sec = (this->buff [this->_read_counter] - '0') * 10;
		READ_COUNT_HANDLE (1); schedule.end_time.sec = this->buff [this->_read_counter] - '0';
		storeTimeInEEPROM (&(schedule.end_time), EEPROM_ADDR_END_TIME_HR);
		redTimeFromEEPROM (&(schedule.end_time), EEPROM_ADDR_END_TIME_HR);
		printf ("cmd.detect End Time: %d : %d : %d\r\n", schedule.end_time.hour, 
													   schedule.end_time.min,
													   schedule.end_time.sec);

		READ_COUNT_HANDLE (2);
		schedule.send_interval.hour = (this->buff [this->_read_counter] - '0') * 10;
		READ_COUNT_HANDLE (1); schedule.send_interval.hour = this->buff [this->_read_counter] - '0';
		READ_COUNT_HANDLE (2);
		schedule.send_interval.min = (this->buff [this->_read_counter] - '0') * 10;
		READ_COUNT_HANDLE (1); schedule.send_interval.min = this->buff [this->_read_counter] - '0';
		READ_COUNT_HANDLE (2);
		schedule.send_interval.sec = (this->buff [this->_read_counter] - '0') * 10;
		READ_COUNT_HANDLE (1); schedule.send_interval.sec = this->buff [this->_read_counter] - '0';
		storeTimeInEEPROM (&(schedule.send_interval), EEPROM_ADDR_INTRVL_TIME_HR);
		redTimeFromEEPROM (&(schedule.send_interval), EEPROM_ADDR_INTRVL_TIME_HR);
		printf ("cmd.detect Intvl Time: %d : %d : %d\r\n", schedule.send_interval.hour, 
													   schedule.send_interval.min,
													   schedule.send_interval.sec);

		EEPROM_write (EEPROM_SCH_FLAG, ID_SET_FLAG);
		break;

	case 'I':
		READ_COUNT_HANDLE (2);
		temp_dev_id = (this->buff [this->_read_counter] - '0') * 10;
		READ_COUNT_HANDLE (1); temp_dev_id = this->buff [this->_read_counter] - '0';
		
		if ((temp_dev_id > 0) && (temp_dev_id < 128))
		{
			EEPROM_write (EEPROM_ADDR_ID, temp_dev_id);
			EEPROM_read (EEPROM_ADDR_ID, &dev_id);
			EEPROM_write (EEPROM_ADDR_ID_FLAG, ID_SET_FLAG);
			printf ("cmd.detect New dev_id: %d", dev_id);
		}
		else
		{
			printf ("cmd.detect dev_id: %d", temp_dev_id);
		}
		break;

	default:
		printf ("cmd.detect No Packet\r\n");

	}

	printf ("cmd.detect->Exiting\r\n");
	return;
}




/**
 * @brief
 * @details
 */
void CmdProcess::start_storing (char rec_char)
{
	this->buff_count = 0;
	this->store (rec_char);
	return;
}

/**
 * @brief
 * @details
 */
bool CmdProcess::incrBuffCount ()
{
	if (this->buff_count >= (PACKET_BUFF_SIZE - 1))
	{
		return FAILURE;
	}
	this->buff_count ++;
	return SUCCESS;
}

/**
 * @brief
 * @details
 */
bool CmdProcess::store (char rec_char)
{
	this->buff[this->buff_count] = rec_char;
	return this->incrBuffCount ();
}

/**
 * @brief
 * @details
 */
bool CmdProcess::_readCountManage (int count)
{
	if ((this->_read_counter + count) < PACKET_BUFF_SIZE)
	{
		this->_read_counter += count;
		return SUCCESS;
	}
	return FAILURE;
}
