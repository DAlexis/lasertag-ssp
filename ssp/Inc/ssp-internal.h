/*
 * ssp-base.h
 *
 *  Created on: 7 янв. 2017 г.
 *      Author: dalexies
 */

#ifndef SMART_SENSOR_LASERTAG_SSP_SSP_INC_SSP_INTERNAL_H_
#define SMART_SENSOR_LASERTAG_SSP_SSP_INC_SSP_INTERNAL_H_

/**
 * This is internal header of SSP system. You should not include int into your program
 */

#include <stdint.h>

#include "ssp.h"

#ifdef __cplusplus
	extern "C" {
#endif


SSP_Package* get_package_if_ready();
void send_package(SSP_Package* package);
uint8_t ssp_is_receiving_timeouted(void);
uint32_t ssp_get_time_from_last_package();
void ssp_reset_receiver(void);

// For unit tests only
uint8_t ssp_crc8(uint8_t *pcBlock, uint16_t len);
uint8_t ssp_crc8_seed(uint8_t seed, uint8_t *pcBlock, uint16_t len);
uint8_t package_crc8(SSP_Header* header, uint8_t *argument);



/*
uint8_t ssp_crc8(uint8_t *pcBlock, uint16_t len);
uint8_t ssp_crc8_seed(uint8_t seed, uint8_t *pcBlock, uint16_t len);
uint8_t ssp_is_received_anything();
uint8_t ssp_is_package_valid(uint8_t* package, uint16_t size);
void ssp_set_crc8(SSP_Header* header, uint8_t *argument);
void ssp_send_data_clear_timeout(uint8_t* data, uint16_t size);
*/
uint16_t ssp_bits_to_bytes(uint16_t bits);

#ifdef __cplusplus
	} // extern "C"
#endif

#endif /* SMART_SENSOR_LASERTAG_SSP_SSP_INC_SSP_INTERNAL_H_ */
