/*
 * ssp-receiver.c
 *
 *  Created on: 1 янв. 2017 г.
 *      Author: dalexies
 */

#include "ssp.h"
#include <stddef.h>

#define CRC8_SEED           0xFF

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

void ssp_reset_receiver(void)
{
	ssp_receiver_buffer.size = 0;
	//ticks_last_receive = ssp_get_time_ms();
}

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
