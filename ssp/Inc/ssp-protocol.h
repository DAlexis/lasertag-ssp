/*
 * ssp.h
 *
 *  Created on: 27 дек. 2016 г.
 *      Author: dalexies
 */

#ifndef INC_SSP_H_
#define INC_SSP_H_

/** SSP - Smart Sensor Protocol - protocol for communication
 * with lasertag smart sensor.
 * This header contains basic protocol definitions:
 * package headers, constants and operations codes
 * "M2S" means Master To Slave, "S2M" vice versa
 */

#include <stdint.h>

#include "ssp-base.h"
#include "ssp-types.h"
#include "ssp-config.h"

#ifdef __cplusplus
	extern "C" {
#endif

#define SSP_MAX_ARGUMENT_SIZE           (SSP_MAX_INPUT_BUFFER_SIZE - sizeof(SSP_Package))

// network structures are packed
#pragma pack(push, 1)

	// Package from master to sensor
	typedef struct {
		SSP_Checksum crc8;
		SSP_Argument_Size size;
		SSP_Address target;
		SSP_Address sender;
		SSP_Command command;
	} SSP_Header;

	typedef struct {
		uint16_t probability; // real prob is probability/UINT16_MAX
	} SSP_Address_Request;

#pragma pack(pop)

typedef struct {
	uint8_t buffer[SSP_MAX_INPUT_BUFFER_SIZE];
	uint16_t size;
	uint8_t has_new_data;
} SSP_Receiver_Buffer;

typedef struct {
	SSP_Header header;
	uint8_t* argument;
} SSP_Package;

// Default addresses
#define SSP_BROADCAST_ADDRESS           ((SSP_Address) 0xFFFF)
#define SSP_MASTER_ADDRESS              ((SSP_Address) 0xFFF0)

// Master to slave communication
#define SSP_M2S_GET_IR_BUFFER           ((SSP_Command) 'g')
#define SSP_M2S_ADD_ANIMATION_TASK      ((SSP_Command) 't')
#define SSP_M2S_RESET_ANIMATION         ((SSP_Command) 'r')
#define SSP_M2S_BREAK_ANIMATION         ((SSP_Command) 'b')
#define SSP_M2S_SEND_IR_BUFFER          ((SSP_Command) 's')
#define SSP_M2S_NOPE                    ((SSP_Command) '0')

// Address discovering features
#define SSP_M2S_SEND_ADDRESS_PROB       ((SSP_Command) 'p')
#define SSP_M2S_ENABLE_SEND_ADDR_PROB   ((SSP_Command) 'x') // Argument is uint8_t true/false

// Slave to master communication
#define SSP_S2M_NOPE               ((SSP_Command) 'n') // No argument
#define SSP_S2M_ADDRESS_DISCOVERY  ((SSP_Command) 'a') // No argument
#define SSP_S2M_IR_DATA            ((SSP_Command) 'i')
#define SSP_S2M_DEBUG              ((SSP_Command) 'd')

#ifdef __cplusplus
	} // end of extern "C"
#endif

#endif /* INC_SSP_H_ */
