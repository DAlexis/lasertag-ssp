/*
 * ssp-master-driver.h
 *
 *  Created on: 9 янв. 2017 г.
 *      Author: dalexies
 */

#ifndef SMART_SENSOR_LASERTAG_SSP_SSP_INC_SSP_MASTER_DRIVER_H_
#define SMART_SENSOR_LASERTAG_SSP_SSP_INC_SSP_MASTER_DRIVER_H_


#include "ssp-driver.h"

#ifdef __cplusplus
	extern "C" {
#endif

/***********************************************
 * This functions must be implemented externally:
 */

/* Write data to debug channel */
void ssp_write_debug(uint8_t* data, uint16_t size);

#ifdef __cplusplus
	} // end of extern "C"
#endif


#endif /* SMART_SENSOR_LASERTAG_SSP_SSP_INC_SSP_MASTER_DRIVER_H_ */
