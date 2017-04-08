/*
 * ssp-driver.h
 *
 *  Created on: 30 дек. 2016 г.
 *      Author: dalexies
 */

#ifndef SMART_SENSOR_INC_SSP_DRIVER_H_
#define SMART_SENSOR_INC_SSP_DRIVER_H_

#include "ssp-base.h"
#include "ssp-types.h"

#include <stdint.h>

#ifdef __cplusplus
	extern "C" {
#endif

/***********************************************
 * This functions must be implemented externally:
 */

/** Will be called from ssp_sonsor_init/ssp_master_init,
 * should initialize hardware, i. e. half duplex UART*/
void ssp_drivers_init(void);

/** Should return valid time in ticks.
 * Tick time must be the same on master and slave device.
 * Currently optimal is tick time = 1 ms */
uint32_t ssp_get_ticks();

/** Should send data over communication channel (i.e. UART)*/
void ssp_send_data(uint8_t* data, uint16_t size);

/***********************************************
 * This functions implemented by SSP:
 */

/* Should be called when data byte is ready.
 * It is safe to call it from in interrupt
 */
void ssp_receive_byte(uint8_t byte);

#ifdef __cplusplus
	}
#endif

#endif /* SMART_SENSOR_INC_SSP_DRIVER_H_ */
