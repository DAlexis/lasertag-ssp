/*
 * ssp-driver-stub.cpp
 *
 *  Created on: 7 янв. 2017 г.
 *      Author: dalexies
 */

#include <stdint.h>

extern "C"
{
void ssp_drivers_init(void)
{

}

uint32_t ssp_get_ticks()
{
}


void ssp_send_data(uint8_t* data, uint16_t size)
{
}

uint8_t ssp_is_ir_data_ready(void) { return 0; }

void ssp_get_ir_data(uint8_t** data, uint16_t* size) { }

void ssp_write_debug(uint8_t* data, uint16_t size)
{

}

}
