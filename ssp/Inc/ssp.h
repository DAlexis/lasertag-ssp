/*
 * ssp.h
 *
 *  Created on: 27 дек. 2016 г.
 *      Author: dalexies
 */

#ifndef INC_SSP_H_
#define INC_SSP_H_

/** SSP - Smart Sensor Protocol - protocol for communication
 * with lasertag smart sensor, this device.
 * "M2S" means Master To Sensor, "S2M" vice versa
 */

#include <stdint.h>

#include "ssp-driver.h"

#ifdef __cplusplus
	extern "C" {
#endif

typedef uint16_t SSP_Address;
typedef uint16_t SSP_Command;
typedef uint8_t SSP_Argument_Size;
typedef uint8_t SSP_Checksum;

#pragma pack(push, 1)

// Package from master to sensor
typedef struct {
	SSP_Checksum crc8;
	SSP_Argument_Size size;
	SSP_Address target;
	SSP_Command command;
} SSP_Header;

typedef struct {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t	vibro;
} SSP_Sensor_Animation_State;

typedef struct {
	SSP_Sensor_Animation_State state;
	uint32_t ms_from_last_state;
} SSP_Sensor_Animation_Task;

#pragma pack(pop)

#define SSP_IR_BUFFER_MAX_SIZE          50
#define SSP_MAX_INPUT_BUFFER_SIZE       70

#define RECEIVER_TIMEOUT	10

#define SSP_BROADCAST_ADDRESS           ((SSP_Address) 0xFFFF)
#define SSP_MASTER_ADDRESS              ((SSP_Address) 0xFFF0)

#define SSP_M2S_GET_IR_BUFFER           ((SSP_Command) 'g')
#define SSP_M2S_ADD_ANIMATION_TASK      ((SSP_Command) 't')
#define SSP_M2S_BREAK_ANIMATION         ((SSP_Command) 'b')
#define SSP_M2S_SEND_IR_BUFFER          ((SSP_Command) 's')
#define SSP_M2S_PING                    ((SSP_Command) 'p')
#define SSP_M2S_NOPE                    ((SSP_Command) '0')

#define SSP_S2M_NOPE       ((SSP_Command) 'n')
#define SSP_S2M_IR_DATA    ((SSP_Command) 'i')
#define SSP_S2M_DEBUG      ((SSP_Command) 'd')


typedef struct {
	uint8_t buffer[SSP_MAX_INPUT_BUFFER_SIZE];
	uint16_t size;
} SSP_Receiver_Buffer;

uint8_t ssp_is_receiving_timeouted(void);
void ssp_reset_receiver(void);
uint8_t ssp_crc8(uint8_t *pcBlock, uint16_t len);
uint8_t ssp_crc8_seed(uint8_t seed, uint8_t *pcBlock, uint16_t len);
uint8_t ssp_is_package_valid(uint8_t* package, uint16_t size);
void ssp_set_crc8(SSP_Header* header, uint8_t *argument);

extern SSP_Receiver_Buffer ssp_receiver_buffer;

#ifdef __cplusplus
	} // end of extern "C"
#endif

#endif /* INC_SSP_H_ */
