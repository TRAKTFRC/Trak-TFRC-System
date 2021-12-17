#include "command_layer_pods.h"
#include <string.h>
#include <stdio.h>
#include "rtc.h"


#define READ_COUNT_HANDLE(c) if ((this->_readCountManage (c)) == FAILURE) return;

void CmdProcessPods::init ()
{
	printf ("\r\ncmd.init");
	task.init ();
}

/**
 * @brief
 * @details
 */
void CmdProcessPods::detect ()
{
	printf ("cmd.detect->Processing command\r\n");
	this->_read_counter = 1;
	
	if (this->buffer_count !=  PKT_LEN_TIME)
	{
		printf ("cmd.detect->Wrong command length: %d\r\n", this->buffer_count);
		return;
	}
	
	while (this->read_count < (PKT_LEN_TIME - 1))
	{

		READ_COUNT_HANDLE (1);
	}



	// Store which pod
	this->_pod = this->buff [this->_read_counter];
	READ_COUNT_HANDLE (1);
	printf ("cmd.detect->Pods code : %c\r\n", this->_pod);

	if (this->buff [this->_read_counter] == '.')
	{
		printf ("cmd.detect->Full dispense detected\r\n");
		this->_exec_state = EXEC_FULL_DISPENSE;
	}
	else if (!(strcmp (&this->buff[this->_read_counter],(char *)"align.")))
	{
		printf ("cmd.detect->Inside align\r\n");
		READ_COUNT_HANDLE (5); // Length of "align" is 5
		if (this->buff [this->_read_counter] == '.')
		{
			printf ("cmd.detect->Install mode detected\r\n");
			this->_exec_state = EXEC_INSTALL_MODE;
		}
		// Incomplete call
	}
	
	printf ("cmd.detect->Exiting\r\n");
	return;
}

/**
 * @brief
 * @details
 */
void CmdProcessPods::start_storing (char rec_char)
{
	this->buff_count = 0;
	this->store (rec_char);
	return;
}

/**
 * @brief
 * @details
 */
bool CmdProcessPods::incrBuffCount ()
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
bool CmdProcessPods::store (char rec_char)
{
	this->buff[this->buff_count] = rec_char;
	return this->incrBuffCount ();
}

/**
 * @brief
 * @details
 */
bool CmdProcessPods::_readCountManage (int count)
{
	printf ("Count increament in\r\n");
	if ((this->_read_counter + count) < PACKET_BUFF_SIZE)
	{
		this->_read_counter += count;
		return SUCCESS;
	}
	printf ("Count increament out\r\n");
	return FAILURE;
}
