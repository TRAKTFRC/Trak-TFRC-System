#include "command_layer.h"
#include <string.h>
#include <stdio.h>
#include "rtc.h"
#include "common.h"

extern bool rtc_time_set_flag;

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
	printf ("cmd.detect->Processing command\r\n");
	this->_read_counter = 1;

	if (this->buff_count !=  PKT_LEN_TIME)
	{
		printf ("cmd.detect->Wrong command length: %d\r\n", this->buff_count);
		return;
	}
	
	switch (this->buff [this->_read_counter])
	{
	case 'R': // Packet been read here: {R,09:33:60,17/12/21}
		READ_COUNT_HANDLE (2);
		rcv_time.hour = (this->buff [this->_read_counter] - '0') * 10;
		READ_COUNT_HANDLE (1);
		rcv_time.hour = this->buff [this->_read_counter] - '0';
		READ_COUNT_HANDLE (2);

		rcv_time.min = (this->buff [this->_read_counter] - '0') * 10;
		READ_COUNT_HANDLE (1);
		rcv_time.min = this->buff [this->_read_counter] - '0';
		READ_COUNT_HANDLE (2);

		rcv_time.sec = (this->buff [this->_read_counter] - '0') * 10;
		READ_COUNT_HANDLE (1);
		rcv_time.sec = this->buff [this->_read_counter] - '0';
		READ_COUNT_HANDLE (2);

		rcv_time.mday = (this->buff [this->_read_counter] - '0') * 10;
		READ_COUNT_HANDLE (1);
		rcv_time.mday = this->buff [this->_read_counter] - '0';
		READ_COUNT_HANDLE (2);

		rcv_time.mon = (this->buff [this->_read_counter] - '0') * 10;
		READ_COUNT_HANDLE (1);
		rcv_time.mon = this->buff [this->_read_counter] - '0';
		READ_COUNT_HANDLE (2);

		rcv_time.year = (this->buff [this->_read_counter] - '0') * 10;
		READ_COUNT_HANDLE (1);
		rcv_time.year = this->buff [this->_read_counter] - '0';

		rtc_set_time (&rcv_time);
		rtc_time_set_flag = true;
		printf ("cmd.detect-RTC Time set\r\n");

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
