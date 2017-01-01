/*
 * ssp-receiver.h
 *
 *  Created on: 1 янв. 2017 г.
 *      Author: dalexies
 */

#ifndef SMART_SENSOR_LASERTAG_SSP_SSP_INC_SSP_RECEIVER_H_
#define SMART_SENSOR_LASERTAG_SSP_SSP_INC_SSP_RECEIVER_H_

#include "ssp-driver.h"
#include <stdint.h>

#define SSP_MAX_INPUT_BUFFER_SIZE    70

/*
 * This header contains code that is identical for master and sensor side
 * and does all things for data receiving and timeout control
 */

typedef struct {
	uint8_t buffer[SSP_MAX_INPUT_BUFFER_SIZE];
	uint16_t size;
} SSP_Receiver_Buffer;

uint8_t ssp_is_receiving_timeouted(void);
uint8_t ssp_receiver_task_tick(void);
void ssp_reset_receiver(void);

extern SSP_Receiver_Buffer ssp_receiver_buffer;

#endif /* SMART_SENSOR_LASERTAG_SSP_SSP_INC_SSP_RECEIVER_H_ */
