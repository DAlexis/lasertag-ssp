/*
 * ssp-master.part.c
 *
 *  Created on: 31 дек. 2016 г.
 *      Author: dalexies
 */

#include "ssp-master-part.h"

#include "ssp-receiver.h"

#include "stddef.h"

// Private variables
static SSP_IR_Buffer ir_queue[SSP_IR_QUEUE_SIZE];
static uint16_t ir_queue_begin = 0;
static uint16_t ir_queue_end = 0;

// Private functions
static void M2S_Header_Struct_Init(SSP_M2S_Header* header);
static uint8_t is_current_package_valid();
static void parse_package();
static SSP_IR_Buffer* pop_ir();

void ssp_master_init(void)
{
	ssp_drivers_init();
	ssp_reset_receiver();
}

void ssp_master_task_tick(void)
{
	if (is_current_package_valid())
	{
		parse_package();
		ssp_reset_receiver();
	}

	if (ssp_is_receiving_timeouted())
	{
		ssp_reset_receiver();
	}
}

void ssp_push_ir_request(Sensor_Address target_device)
{
	SSP_M2S_Header header;
	M2S_Header_Struct_Init(&header);
	header.command = SSP_M2S_GET_IR_BUFFER;
	header.target = target_device;
	ssp_send_data((uint8_t*) &header, sizeof(SSP_M2S_Header));
}

SSP_IR_Buffer* ssp_get_next_ir_buffer()
{
	if (ir_queue_begin != ir_queue_end)
	{
		return &(ir_queue[ir_queue_begin++]);
	} else
		return NULL;
}


void M2S_Header_Struct_Init(SSP_M2S_Header* header)
{
	header->start_byte = SSP_START_BYTE_M2S;
	header->target = SSP_BROADCAST_ADDRESS;
	header->command = SSP_M2S_NOPE;
	header->size = 0;
}

uint8_t is_current_package_valid()
{
	if (ssp_receiver_buffer.size < sizeof(SSP_S2M_Header))
		return 0;

	// We can read header
	SSP_S2M_Header *incoming = (SSP_S2M_Header *) ssp_receiver_buffer.buffer;

	if (ssp_receiver_buffer.size < sizeof(SSP_S2M_Header) + incoming->package_size)
		return 0;

	return 1;
}

void parse_package()
{
	SSP_S2M_Header *incoming = (SSP_S2M_Header *) ssp_receiver_buffer.buffer;

	if (incoming->start_byte == SSP_START_BYTE_DEBUG)
	{
		ssp_write_debug(ssp_receiver_buffer.buffer + sizeof(SSP_S2M_Header), incoming->package_size);
		return;
	}
	if (incoming->start_byte != SSP_START_BYTE_S2M)
		return; // Package is not for sensor


	switch(incoming->package_type)
	{
	case SSP_S2M_PACKAGE_TYPE_IR_DATA:
		// Todo: process IR data
		return;
	}
}

void push_ir(SSP_IR_Buffer* buffer)
{
	if (ir_queue_end - ir_queue_begin > SSP_IR_QUEUE_SIZE-1)
		return; // No free space in queue

	ir_queue[ir_queue_end] = *buffer;

	if (ir_queue_end == SSP_IR_QUEUE_SIZE-1)
		ir_queue_end = 0;
	else
		ir_queue_end++;
}