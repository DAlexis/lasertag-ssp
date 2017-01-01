/*
 * ssp-receiver.c
 *
 *  Created on: 1 янв. 2017 г.
 *      Author: dalexies
 */

#include "ssp-receiver.h"

#define RECEIVER_TIMEOUT	100

// Public variables
SSP_Receiver_Buffer ssp_receiver_buffer;

// Private variables
static uint32_t ticks_last_receive = 0;

void ssp_receive_byte(uint8_t byte)
{
	ticks_last_receive = ssp_get_time_ms();
	if (ssp_receiver_buffer.size == SSP_MAX_INPUT_BUFFER_SIZE - 1)
		return; // We do not support messages that are longer than SSP_MAX_INPUT_BUFFER_SIZE
	ssp_receiver_buffer.buffer[ssp_receiver_buffer.size++] = byte;
}

uint8_t ssp_is_receiving_timeouted(void)
{
	if (ssp_get_time_ms() - ticks_last_receive > RECEIVER_TIMEOUT)
		return 1;
	else
		return 0;
}

uint8_t ssp_receiver_task_tick(void)
{
	if (ssp_is_receiving_timeouted())
	{
		ssp_reset_receiver();
	}
}

void ssp_reset_receiver(void)
{
	ssp_receiver_buffer.size = 0;
}
