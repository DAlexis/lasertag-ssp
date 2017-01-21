/*
 * ssp-master.part.c
 *
 *  Created on: 31 дек. 2016 г.
 *      Author: dalexies
 */

#include "ssp-protocol.h"
#include "ssp-master-driver.h"
#include "ssp-master.h"
#include <ssp-utils.h>
#include <stddef.h>
#include <string.h>
//#include <stdio.h>

#define ADDRESS_DISCOVERING_PROB_STEP     UINT16_MAX / 32
#define ADDRESS_DISCOVERING_START_PROB    ADDRESS_DISCOVERING_PROB_STEP
#define ADDRESS_DISCOVERING_TIMEOUT       30

// Public variables
SSP_Registered_Addrs_List ssp_registered_addrs;

// Private variables
static SSP_IR_Message ir_queue[SSP_IR_QUEUE_SIZE];
static uint16_t ir_queue_begin = 0;
static uint16_t ir_queue_end = 0;
static uint8_t busy = 0;


static uint16_t addr_disc_last_prob = 0;

// Private functions
static void package_init(SSP_Package* package);
uint8_t is_package_for_me(SSP_Package* package);
static void parse_package();
static void push_ir(SSP_Address sender, SSP_IR_Buffer* buf);
static void request_address_probably(uint16_t prob);
static void set_address_resolving(SSP_Address addr, uint8_t value);
static void register_address(SSP_Address addr);


void ssp_master_init(void)
{
	ssp_drivers_init();
	ssp_reset_receiver();
}

void ssp_master_task_tick(void)
{
	// Fetching all packages one by one
	SSP_Package* package;
	while (NULL != (package = get_package_if_ready()))
	{
		if (is_package_for_me(package))
		{
			parse_package(package);
		}
	}

	if (!ssp_is_address_discovering_now())
		busy = 0;


	if (ssp_is_address_discovering_now())
	{
		// We are in address discovery mode
		if (ssp_get_time_from_last_package() > ADDRESS_DISCOVERING_TIMEOUT)
		{
			addr_disc_last_prob += ADDRESS_DISCOVERING_PROB_STEP;

			// Check if cycle done
			if (addr_disc_last_prob > UINT16_MAX - ADDRESS_DISCOVERING_PROB_STEP*2)
			{
				// We should stop discovering procedure
				ssp_stop_address_discovering();
				return;
			}
			request_address_probably(addr_disc_last_prob);
			return;
		}
	}

	if (ssp_is_receiving_timeouted())
	{
		busy = 0;
		ssp_reset_receiver();
	}

}

void ssp_push_ir_request(SSP_Address target_device)
{
	busy = 1;
	SSP_Package package;
	package_init(&package);
	package.header.command = SSP_M2S_GET_IR_BUFFER;
	package.header.target = target_device;
	send_package(&package);
}

SSP_IR_Message* ssp_get_next_ir_buffer()
{
	if (ir_queue_begin != ir_queue_end)
	{
		SSP_IR_Message* result = &(ir_queue[ir_queue_begin]);
		if (ir_queue_begin == SSP_IR_QUEUE_SIZE-1)
			ir_queue_begin = 0;
		else
			ir_queue_begin++;
		return result;
	} else
		return NULL;
}

void ssp_push_animation_task(SSP_Address target, SSP_Sensor_Animation_Task* task)
{
	busy = 1;

	SSP_Package package;
	package_init(&package);
	package.header.command = SSP_M2S_ADD_ANIMATION_TASK;
	package.header.target = target;
	package.header.size = sizeof(SSP_Sensor_Animation_Task);
	package.argument = (uint8_t*) task;
	send_package(&package);
}

void ssp_start_address_discovering()
{
	busy = 1;
	addr_disc_last_prob = ADDRESS_DISCOVERING_START_PROB;
	// Everybody can answer to address request
	set_address_resolving(SSP_BROADCAST_ADDRESS, 1);
	request_address_probably(addr_disc_last_prob);
}

uint8_t ssp_is_address_discovering_now()
{
	return !(addr_disc_last_prob == 0);
}

void ssp_stop_address_discovering()
{
	addr_disc_last_prob = 0;
}

uint8_t ssp_is_busy()
{
	return busy;
}

void package_init(SSP_Package* package)
{
	package->header.target = SSP_BROADCAST_ADDRESS;
	package->header.command = SSP_M2S_NOPE;
	package->header.sender = SSP_MASTER_ADDRESS;
	package->header.size = 0;
	package->argument = NULL;
}

void parse_package()
{
	SSP_Header *incoming = (SSP_Header *) ssp_receiver_buffer.buffer;
	uint8_t *data = ssp_receiver_buffer.buffer + sizeof(SSP_Header);
	switch(incoming->command)
	{
	case SSP_S2M_IR_DATA:
		if (incoming->size != 0)
			push_ir(incoming->sender, (SSP_IR_Buffer*) data);

		break;
	case SSP_S2M_DEBUG:
		ssp_write_debug(data, incoming->size);
		break;
	case SSP_S2M_ADDRESS_DISCOVERY:
		register_address( incoming->sender );
		break;
	}
}

uint8_t is_package_for_me(SSP_Package* package)
{
	return (/*package->header.target == SSP_BROADCAST_ADDRESS || */package->header.target == SSP_MASTER_ADDRESS);
}

void push_ir(SSP_Address sender, SSP_IR_Buffer* buf)
{
	if (ir_queue_end - ir_queue_begin > SSP_IR_QUEUE_SIZE-1)
		return; // No free space in queue

	ir_queue[ir_queue_end].buffer = *buf;
	ir_queue[ir_queue_end].sensor_address = sender;

	if (ir_queue_end == SSP_IR_QUEUE_SIZE-1)
		ir_queue_end = 0;
	else
		ir_queue_end++;
}

void request_address_probably(uint16_t prob)
{
	//printf("Requesting address with prob %d\n", (int) prob);
	SSP_Address_Request req;
	req.probability = prob;

	SSP_Package package;
	package_init(&package);
	package.header.command = SSP_M2S_SEND_ADDRESS_PROB;
	package.header.target = SSP_BROADCAST_ADDRESS;
	package.header.size = sizeof(SSP_Address_Request);
	package.argument = (uint8_t*) &req;
	send_package(&package);
}

void set_address_resolving(SSP_Address addr, uint8_t value)
{
	SSP_Package package;
	package_init(&package);
	package.header.command = SSP_M2S_ENABLE_SEND_ADDR_PROB;
	package.header.target = addr;
	package.header.size = 1;
	package.argument = &value;
	send_package(&package);
}

void register_address(SSP_Address addr)
{
	// Search if we already registered this
	for (int i=0; i<ssp_registered_addrs.size; i++)
		if (ssp_registered_addrs.address[i] == addr)
			return;

	if (ssp_registered_addrs.size == SSP_MAX_SENSORS_COUNT)
		return;
	ssp_registered_addrs.address[ssp_registered_addrs.size++] = addr;

	// Sender should not answer to address requests in the future
	set_address_resolving(addr, 0);
}
