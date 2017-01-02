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

typedef struct {
	uint8_t data[SSP_IR_BUFFER_MAX_SIZE];
	uint16_t size;
	SSP_Address sensor_address;
} SSP_IR_Buffer;

void ssp_master_init(void);

/* Should be called as frequently as possible from main loop or OS thread */
void ssp_master_task_tick(void);

void ssp_push_ir_request(SSP_Address target_device);

SSP_IR_Buffer* ssp_get_next_ir_buffer();

void ssp_push_animation_task(SSP_Address target, SSP_Sensor_Animation_Task* task);

#ifdef __cplusplus
	} // end of extern "C"
#endif

#endif /* SMART_SENSOR_LASERTAG_SSP_SSP_INC_SSP_MASTER_PART_H_ */
