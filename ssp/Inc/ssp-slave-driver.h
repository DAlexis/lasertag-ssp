/*
 * ssp-slave-driver.h
 *
 *  Created on: 9 янв. 2017 г.
 *      Author: dalexies
 */

#ifndef SMART_SENSOR_LASERTAG_SSP_SSP_INC_SSP_SLAVE_DRIVER_H_
#define SMART_SENSOR_LASERTAG_SSP_SSP_INC_SSP_SLAVE_DRIVER_H_

#include "ssp-driver.h"

#ifdef __cplusplus
	extern "C" {
#endif

/***********************************************
 * This functions must be implemented externally:
 */

/** Should returns true when IR data received and ready to handle it */
uint8_t ssp_is_ir_data_ready(void);

/** Should set pointer to IR data and set its size */
void ssp_get_ir_data(uint8_t** data, uint16_t* size);

/** Returns self address of device */
SSP_Address ssp_self_address();

#ifdef __cplusplus
	} // end of extern "C"
#endif

#endif /* SMART_SENSOR_LASERTAG_SSP_SSP_INC_SSP_SLAVE_DRIVER_H_ */
