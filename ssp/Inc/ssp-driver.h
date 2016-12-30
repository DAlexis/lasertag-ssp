/*
 * ssp-driver.h
 *
 *  Created on: 30 дек. 2016 г.
 *      Author: dalexies
 */

#ifndef SMART_SENSOR_INC_SSP_DRIVER_H_
#define SMART_SENSOR_INC_SSP_DRIVER_H_

#include <stdint.h>

/***********************************************
 * This functions must be implemented externally:
 */

void ssp_drivers_init(void);

/* SSP will call it for transmission */
void ssp_send_data(uint8_t* data, uint16_t size);

/* SSP will call it to check if receiving is timeouted.
 * If returns true, SSP will be forced to handle received data
 */
uint8_t ssp_is_receiving_timeouted(void);

uint8_t ssp_is_ir_data_ready(void);

void ssp_get_ir_data(uint8_t** data, uint16_t* size);

/***********************************************
 * This functions implemented by SSP:
 */

/* Should be called when data byte is ready.
 * It is safe to call it from in interrupt
 */
void ssp_receive_byte(uint8_t byte);

/* Should be called as frequently as possible from main loop or OS thread */
void ssp_task_tick(void);

#endif /* SMART_SENSOR_INC_SSP_DRIVER_H_ */
