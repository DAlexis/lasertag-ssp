/*
 * ssp-master.h
 *
 *  Created on: 9 янв. 2017 г.
 *      Author: dalexies
 */

#ifndef SMART_SENSOR_LASERTAG_SSP_SSP_INC_SSP_MASTER_H_
#define SMART_SENSOR_LASERTAG_SSP_SSP_INC_SSP_MASTER_H_

#include "ssp-base.h"
#include "ssp-types.h"

#ifdef __cplusplus
	extern "C" {
#endif

#define SSP_IR_QUEUE_SIZE        5
#define SSP_MAX_SENSORS_COUNT    20

typedef struct {
	SSP_IR_Buffer buffer;
	SSP_Address sensor_address;
} SSP_IR_Message;

typedef struct {
	SSP_Address address[SSP_MAX_SENSORS_COUNT];
	uint16_t size;
} SSP_Registered_Addrs_List;

/* Initialize SSP for master device */
void ssp_master_init(void);

/* Should be called as frequently as possible from main loop or OS thread */
void ssp_master_task_tick(void);

/* Send IR buffer request to sensor with address specified. Result will be put into queue */
void ssp_push_ir_request(SSP_Address target_device);

/* Get IR buffer from queue and pop it. NULL if no data received */
SSP_IR_Message* ssp_get_next_ir_buffer();

/* Send animation task to sensor by address. Broadcast acceptable */
void ssp_push_animation_task(SSP_Address target, SSP_Sensor_Animation_Task* task);

/* Begin address discovering procedure */
void ssp_start_address_discovering();

/* Get addres discovering status. Non-zero if discovering in process */
uint8_t ssp_is_address_discovering_now();

/* Force stopping address discovering */
void ssp_stop_address_discovering();

/* Non-zero if ssp is busy now. This should be checked before any transmission over SSP */
uint8_t ssp_is_busy();

extern SSP_Registered_Addrs_List ssp_registered_addrs;

#ifdef __cplusplus
	} // end of extern "C"
#endif



#endif /* SMART_SENSOR_LASERTAG_SSP_SSP_INC_SSP_MASTER_H_ */
