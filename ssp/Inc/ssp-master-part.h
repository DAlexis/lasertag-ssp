/*
 * ssp-master-part.hpp
 *
 *  Created on: 30 дек. 2016 г.
 *      Author: dalexies
 */

#ifndef SMART_SENSOR_LASERTAG_SSP_SSP_INC_SSP_MASTER_PART_H_
#define SMART_SENSOR_LASERTAG_SSP_SSP_INC_SSP_MASTER_PART_H_

#include "ssp.h"

#ifdef __cplusplus
	extern "C" {
#endif

#define SSP_IR_QUEUE_SIZE        5
#define SSP_MAX_SENSORS_COUNT    20


typedef struct {
	SSP_S2M_IR_Buffer buffer;
	SSP_Address sensor_address;
} SSP_IR_Buffer;

typedef struct {
	SSP_Address address[SSP_MAX_SENSORS_COUNT];
	uint16_t size;
} SSP_Registered_Addrs_List;

void ssp_master_init(void);

/* Should be called as frequently as possible from main loop or OS thread */
void ssp_master_task_tick(void);

void ssp_push_ir_request(SSP_Address target_device);

SSP_IR_Buffer* ssp_get_next_ir_buffer();

void ssp_push_animation_task(SSP_Address target, SSP_Sensor_Animation_Task* task);

void ssp_start_address_discovering();
uint8_t ssp_is_address_discovering_now();
void ssp_stop_address_discovering();

uint8_t ssp_is_busy();

extern SSP_Registered_Addrs_List ssp_registered_addrs;

#ifdef __cplusplus
	} // end of extern "C"
#endif

#endif /* SMART_SENSOR_LASERTAG_SSP_SSP_INC_SSP_MASTER_PART_H_ */
