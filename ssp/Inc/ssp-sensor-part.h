/*
 * ssp-sensor-part.h
 *
 *  Created on: 27 дек. 2016 г.
 *      Author: dalexies
 */

#ifndef INC_SSP_SENSOR_PART_H_
#define INC_SSP_SENSOR_PART_H_

#include "ssp.h"
#include "ssp-config.h"

void ssp_sensor_init(void);

void ssp_send_debug_msg(char *ptr, int len);

/* Should be called as frequently as possible from main loop or OS thread */
void ssp_sensor_task_tick(void);

extern SSP_Sensor_Animation_State sensor_state;

#endif /* INC_SSP_SENSOR_PART_H_ */
