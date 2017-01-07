/*
 * ssp-base.c
 *
 *  Created on: 7 янв. 2017 г.
 *      Author: dalexies
 */

#include "ssp-internal.h"

#include <stddef.h>
#include <string.h>
#include <stdio.h>

#define CRC8_SEED           0xFF

// Public variables
SSP_Receiver_Buffer ssp_receiver_buffer;
static SSP_Package ssp_user_package;
static uint8_t ssp_user_package_argument[SSP_MAX_ARGUMENT_SIZE];

// Private variables
static uint32_t ticks_last_bus_io = 0;


void ssp_receive_byte(uint8_t byte)
{
	ssp_receiver_buffer.was_anything_received = 1;
	ticks_last_bus_io = ssp_get_time_ms();
	if (ssp_receiver_buffer.size == SSP_MAX_INPUT_BUFFER_SIZE - 1)
		return; // We do not support messages that are longer than SSP_MAX_INPUT_BUFFER_SIZE
	ssp_receiver_buffer.buffer[ssp_receiver_buffer.size++] = byte;
}

SSP_Package* get_package_if_ready()
{
	if (ssp_receiver_buffer.size < sizeof(SSP_Header))
		return NULL;

	// Now we can read header
	SSP_Header* header = (SSP_Header*) ssp_receiver_buffer.buffer;

	uint16_t total_package_size = sizeof(SSP_Header) + header->size;

	if (ssp_receiver_buffer.size < total_package_size)
		return NULL;

	uint8_t* argument_inplace = ssp_receiver_buffer.buffer + sizeof(SSP_Header);

	// Now we can check CRC
	uint8_t crc8 = package_crc8(header, argument_inplace);

	if (crc8 != header->crc8)
	{
		/** We have package that seems valid, but its CRC failed
		 * Really we can do nothing except to be confused...
		 * We may delete this package from buffer, but it would not
		 * guarantee then next package will not be damaged so
		 * lets wait until timeout.
		 */
		return NULL;
	}

	// Buffer beginned from valid package, we should move it into userspace
	ssp_user_package.header = *header;
	ssp_user_package.argument = ssp_user_package_argument;
	memcpy(ssp_user_package_argument, argument_inplace, header->size);

	// User has valid copy of package, not we should shift input buffer in critical section:

	// Todo Critical section begin
	memmove(
			ssp_receiver_buffer.buffer,
			ssp_receiver_buffer.buffer + total_package_size,
			ssp_receiver_buffer.size - total_package_size
	);
	ssp_receiver_buffer.size -= total_package_size;
	// Todo Critical section end
	return &ssp_user_package;
}

void send_package(SSP_Package* package)
{
	// Check for obviously incorrect package
	if (package->header.size != 0 && package->argument == NULL)
		return;

	// Setting right crc8


	package->header.crc8 = package_crc8(&(package->header), package->argument);

	ssp_send_data((uint8_t*) &(package->header), sizeof(SSP_Header));
	ssp_send_data(package->argument, package->header.size);

	ticks_last_bus_io = ssp_get_time_ms();
}

uint8_t package_crc8(SSP_Header* header, uint8_t *argument)
{
	// Calculating by header (except crc8 field)
	uint8_t crc8 = ssp_crc8(
			((uint8_t*) header) + sizeof(SSP_Checksum),
			sizeof(SSP_Header) - sizeof(SSP_Checksum)
	);

	// Calculating by argument
	if (header->size != 0)
		crc8 = ssp_crc8_seed(crc8, (uint8_t*) argument, header->size);
	return crc8;
}

uint8_t ssp_is_receiving_timeouted(void)
{
	if (ssp_get_time_from_last_package() > RECEIVER_TIMEOUT)
		return 1;
	else
		return 0;
}

uint32_t ssp_get_time_from_last_package()
{
	return ssp_get_time_ms() - ticks_last_bus_io;
}

void ssp_reset_receiver(void)
{
	ssp_receiver_buffer.size = 0;
	ssp_receiver_buffer.was_anything_received = 0;
	//ticks_last_bus_io = ssp_get_time_ms();
}
/*
uint8_t ssp_is_package_valid(uint8_t* package, uint16_t size)
{
	if (size < sizeof(SSP_Header))
		return 0;

	// Now we can read header
	SSP_Header* header = (SSP_Header*) package;

	if (size != sizeof(SSP_Header) + header->size)
		return 0;

	// Now we can check CRC
	uint8_t crc8 = ssp_crc8(
			(uint8_t*) (package + sizeof(SSP_Checksum)),
			size - sizeof(SSP_Checksum)
	);
	if (crc8 != header->crc8)
		return 0;
	return 1;
}*/

uint8_t ssp_is_received_anything()
{
	return ssp_receiver_buffer.size != 0;
}

void ssp_set_crc8(SSP_Header* header, uint8_t *argument)
{
	// Calculating by header (except crc8 field)
	uint8_t crc8 = ssp_crc8_seed(
			CRC8_SEED,
			(uint8_t*) (header + sizeof(SSP_Checksum)),
			sizeof(SSP_Header) - sizeof(SSP_Checksum)
	);

	// Caalculating by argument
	if (header->size != 0 && argument != NULL)
		crc8 = ssp_crc8_seed(crc8, argument, header->size);

	header->crc8 = crc8;
}

uint8_t ssp_crc8(uint8_t *data, uint16_t len)
{
	return ssp_crc8_seed(CRC8_SEED, data, len);
}

uint8_t ssp_crc8_seed(uint8_t seed, uint8_t *data, uint16_t len)
{
	uint8_t crc = seed;
	uint16_t i;

	while (len--)
	{
		crc ^= *data++;

		for (i = 0; i < 8; i++)
			crc = crc & 0x80 ? (crc << 1) ^ 0x31 : crc << 1;
	}

	return crc;
}

void ssp_send_data_clear_timeout(uint8_t* data, uint16_t size)
{
	ssp_send_data(data, size);
	ticks_last_bus_io = ssp_get_time_ms();
}


uint16_t ssp_bits_to_bytes(uint16_t bits)
{
	uint16_t bytes = bits / 8;
	if (bits % 8 != 0)
		bytes++;
	return bytes;
}
