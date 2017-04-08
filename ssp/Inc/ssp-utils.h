/*
 * ssp-base.h
 *
 *  Created on: 7 янв. 2017 г.
 *      Author: dalexies
 */

#ifndef SMART_SENSOR_LASERTAG_SSP_SSP_INC_SSP_UTILS_H_
#define SMART_SENSOR_LASERTAG_SSP_SSP_INC_SSP_UTILS_H_

/**
 * This is internal header of SSP system. You should not include it into your program
 */

#include <ssp-protocol.h>
#include <stdint.h>

#ifdef __cplusplus
	extern "C" {
#endif


SSP_Package* get_package_if_ready();
void     send_package(SSP_Package* package);
uint8_t  ssp_is_receiving_timeouted(void);
uint8_t  ssp_is_transmitter_timeouted(void);
uint32_t ssp_get_time_from_last_package();
void     ssp_reset_receiver(void);

uint16_t ssp_bits_to_bytes(uint16_t bits);

extern SSP_Receiver_Buffer ssp_receiver_buffer;

#ifdef __cplusplus
	} // extern "C"
#endif

#endif /* SMART_SENSOR_LASERTAG_SSP_SSP_INC_SSP_UTILS_H_ */
