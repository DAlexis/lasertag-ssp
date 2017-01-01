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

typedef uint16_t Sensor_Address;
typedef uint16_t Sensor_Command;
typedef uint8_t Sensor_Argument_Size;
typedef uint8_t S2M_Package_Type;
typedef uint16_t S2M_Package_Size;

#pragma pack(push, 1)

// Package from master to sensor
typedef struct {
	uint8_t start_byte; // should be SSP_START_BYTE_M2S
	Sensor_Address target;
	Sensor_Command command;
	Sensor_Argument_Size size;
} SSP_M2S_Header;

// Header of packager from sensor to master
typedef struct {
	uint8_t start_byte; // should be SSP_START_BYTE_S2M
	S2M_Package_Type package_type;
	S2M_Package_Size package_size;
} SSP_S2M_Header;

// Debug info (no real data, simply text for human)
typedef struct {
	uint8_t start_byte;
	uint16_t size;
} SSP_Debug_Header;

typedef struct {
	uint8_t red_power;
	uint8_t green_power;
	uint8_t blue_power;
	uint8_t	vibro_enabled;
} SSP_Sensor_Animation_State;

typedef struct {
	SSP_Sensor_Animation_State state;
	uint32_t ms_from_last_state;
} SSP_Sensor_Animation_Task;

#pragma pack(pop)

#define SSP_IR_BUFFER_MAX_SIZE          50

#define SSP_START_BYTE_M2S              ((uint8_t) 'M')
#define SSP_START_BYTE_S2M              ((uint8_t) 'S')
#define SSP_START_BYTE_DEBUG            ((uint8_t) 'D')

#define SSP_BROADCAST_ADDRESS           ((Sensor_Address) 0xFFFF)

#define SSP_M2S_GET_IR_BUFFER           ((Sensor_Command) 'g')
#define SSP_M2S_ADD_ANIMATION_TASK      ((Sensor_Command) 't')
#define SSP_M2S_BREAK_ANIMATION         ((Sensor_Command) 'b')
#define SSP_M2S_SEND_IR_BUFFER          ((Sensor_Command) 's')
#define SSP_M2S_NOPE                    ((Sensor_Command) '0')

#define SSP_S2M_PACKAGE_TYPE_NOPE       ((S2M_Package_Type) 'n')
#define SSP_S2M_PACKAGE_TYPE_IR_DATA    ((S2M_Package_Type) 'i')

#ifdef __cplusplus
	} // end of extern "C"
#endif

#endif /* INC_SSP_H_ */
