/*
 * ssp-slave.h
 *
 *  Created on: 9 янв. 2017 г.
 *      Author: dalexies
 */

#ifndef SMART_SENSOR_LASERTAG_SSP_SSP_INC_SSP_SLAVE_H_
#define SMART_SENSOR_LASERTAG_SSP_SSP_INC_SSP_SLAVE_H_

#include <stdint.h>

/**
 * This header contains all functions needed by SSP slave device
 * (except drivers)
 */

/* Initialize SSP for slave device */
void ssp_sensor_init(void);

/* Send debug message over SSP.
 * Attention: this function should not be used when more than one device connected to master! */
void ssp_send_debug_msg(char *ptr, int len);

/* Should be called as frequently as possible from main loop or OS thread */
void ssp_sensor_task_tick(void);

/* Returns current state of leds and vibro according to animation tasks received from master */
void ssp_get_leds_vibro_state(uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* vibro);

#endif /* SMART_SENSOR_LASERTAG_SSP_SSP_INC_SSP_SLAVE_H_ */
