/*
 * config.h
 *
 *  Created on: 27 дек. 2016 г.
 *      Author: dalexies
 */

#ifndef INC_SSP_CONFIG_H_
#define INC_SSP_CONFIG_H_

/**
 * This is SSP configuration file. You do not need to change variables
 * here directly, you may use compiler keys like -DSSP_UART_SPEED=9600
 * You may edit it if default settings
 * is not appropriable for you
 */

///////////////////////////////////////////
// Generic configuration

#ifndef SSP_MAX_INPUT_BUFFER_SIZE
	#define SSP_MAX_INPUT_BUFFER_SIZE    70
#endif

#ifndef RECEIVER_TIMEOUT
	#define RECEIVER_TIMEOUT	         2
#endif

/** Transmitter timeout should be
 * at least >= RECEIVER_TIMEOUT + 2
 */
#ifndef TRAMSMITTER_TIMEOUT
	#define TRAMSMITTER_TIMEOUT          RECEIVER_TIMEOUT + 3
#endif

#ifndef SSP_IR_BUFFER_MAX_SIZE
	#define SSP_IR_BUFFER_MAX_SIZE       50
#endif

/**
 * You may use it if your bus is UART
 */
#ifndef SSP_UART_SPEED
	#define SSP_UART_SPEED               115200
#endif

///////////////////////////////////////////
// Sensor configuration

#ifndef MAX_ANIMATION_TASKS_COUNT
	#define MAX_ANIMATION_TASKS_COUNT	 15
#endif

///////////////////////////////////////////
// Master configuration

#ifndef ADDRESS_DISCOVERING_PROB_STEP
	#define ADDRESS_DISCOVERING_PROB_STEP    UINT16_MAX / 32
#endif

#ifndef ADDRESS_DISCOVERING_START_PROB
	#define ADDRESS_DISCOVERING_START_PROB   ADDRESS_DISCOVERING_PROB_STEP
#endif

#ifndef ADDRESS_DISCOVERING_TIMEOUT
	#define ADDRESS_DISCOVERING_TIMEOUT      30
#endif

#endif /* INC_SSP_CONFIG_H_ */
