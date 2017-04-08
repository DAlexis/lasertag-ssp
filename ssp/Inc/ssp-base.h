/*
 * ssp-base.h
 *
 *  Created on: 9 янв. 2017 г.
 *      Author: dalexies
 */

#ifndef SMART_SENSOR_LASERTAG_SSP_SSP_INC_SSP_BASE_H_
#define SMART_SENSOR_LASERTAG_SSP_SSP_INC_SSP_BASE_H_

#include <stdint.h>

#include "ssp-config.h"

#ifdef __cplusplus
	extern "C" {
#endif

/**
 * This header contains SSP definitions used for both Master and Slave device
 * and needed by user
 */

#pragma pack(push, 1)

typedef struct {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t	vibro;
} SSP_Sensor_Animation_State;

typedef struct {
	SSP_Sensor_Animation_State state;
	uint32_t ms_from_last_state;
} SSP_Sensor_Animation_Task;

typedef struct {
	uint8_t data[SSP_IR_BUFFER_MAX_SIZE];
	uint16_t bits_count;
} SSP_IR_Buffer;

#pragma pack(pop)

#ifdef __cplusplus
	} // end of extern "C"
#endif



#endif /* SMART_SENSOR_LASERTAG_SSP_SSP_INC_SSP_BASE_H_ */
