#include "ssp-protocol.h"
#include "ssp-config.h"
#include "ssp-slave-driver.h"
#include "ssp-slave.h"
#include "ssp-utils.h"

#include <stddef.h>
#include <string.h>
//#include "stm32f0xx_hal.h"

#define MAX_ANIMATION_TASKS_COUNT	10

// Public variables
SSP_Sensor_Animation_State sensor_state;

// Private variables

static SSP_Sensor_Animation_Task animation_tasks[MAX_ANIMATION_TASKS_COUNT];
static uint32_t last_animation_task_time = 0;
static uint8_t preloaded_animations_count = 0;
static uint8_t next_animation_to_load = 0;
static uint8_t next_animation_to_play = 0;
static uint8_t is_address_sending_enabled = 1;

static uint8_t next_animation_ring_index(uint8_t index);
static uint8_t prev_animation_ring_index(uint8_t index);

// Private functions prototypes

static void parse_package(SSP_Package* package);
static uint8_t is_package_for_me(SSP_Package* package);
static void package_init(SSP_Package* package);
static void send_ir_data(void);
static void send_address_probably(uint16_t prob);
static void address_sending_enable(uint8_t enable);
static void animate(void);
static void load_animation_task(SSP_Sensor_Animation_Task* task);

// Public functions

void ssp_sensor_task_tick(void)
{
	// First, lets make animation step
	animate();

	// Fetching all packages one by one
	SSP_Package* package;
	while (NULL != (package = get_package_if_ready()))
	{
		if (is_package_for_me(package))
			parse_package(package);
	}

	if (ssp_is_receiving_timeouted())
	{
		ssp_reset_receiver();
	}

	/* If package is not for smart sensor, we can do nothing
	 * except waiting for timeout, because data stream structure
	 * is unknown
	 */
}

void ssp_sensor_init(void)
{
	ssp_reset_receiver();
	sensor_state.red = 0;
	sensor_state.green = 0;
	sensor_state.blue = 0;
	sensor_state.vibro = 0;
	ssp_drivers_init();
}

void ssp_get_leds_vibro_state(uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* vibro)
{
	*r = sensor_state.red;
	*g = sensor_state.green;
	*b = sensor_state.blue;
	*vibro = sensor_state.vibro;
}

void ssp_send_debug_msg(char *ptr, int len)
{
	SSP_Package package;
	package_init(&package);
	package.header.command = SSP_S2M_DEBUG;
	package.header.size = len;
	package.argument = (uint8_t*) ptr;
	send_package(&package);
}

// Private functions

uint8_t is_package_for_me(SSP_Package* package)
{
	if (package->header.target == SSP_BROADCAST_ADDRESS || package->header.target  == SSP_SELF_ADDRESS)
		return 1; // Package not for me
	else
		return 0;
}

void parse_package(SSP_Package* package)
{
	switch(package->header.command)
	{
	case SSP_M2S_GET_IR_BUFFER: send_ir_data(); return;
	case SSP_M2S_ADD_ANIMATION_TASK: load_animation_task((SSP_Sensor_Animation_Task*) package->argument ); return;
	case SSP_M2S_ENABLE_SEND_ADDR_PROB: address_sending_enable(*package->argument); return;
	case SSP_M2S_SEND_ADDRESS_PROB: send_address_probably( *((uint16_t*) package->argument) ); return;
	}
}

void package_init(SSP_Package* package)
{
	package->header.size = 0;
	package->header.target = SSP_MASTER_ADDRESS;
	package->header.sender = SSP_SELF_ADDRESS;
	package->header.command = SSP_S2M_NOPE;
	package->argument = NULL;
}

void send_ir_data(void)
{
	SSP_Package package;
	package_init(&package);
	package.header.command = SSP_S2M_IR_DATA;

	if (ssp_is_ir_data_ready())
	{
		// We have IR data, so sending it via SSP
		uint8_t* ir_data;
		uint16_t ir_data_size;
		ssp_get_ir_data(&ir_data, &ir_data_size);

		SSP_IR_Buffer buf;
		memset(buf.data, 0, sizeof(buf.data));
		memcpy(buf.data, ir_data, ssp_bits_to_bytes(ir_data_size));
		buf.bits_count = ir_data_size;

		package.header.size = sizeof(SSP_IR_Buffer);
		package.argument = &buf;
		send_package(&package);
	} else {
		package.header.size = 0;
		send_package(&package);
	}
}

void send_address_probably(uint16_t prob)
{
	if (!is_address_sending_enabled)
		return;

	// Throwing dice
	if (ssp_random() > prob)
		return;

	//HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_0);
	SSP_Package package;
	package_init(&package);
	package.header.command = SSP_S2M_ADDRESS_DISCOVERY;
	send_package(&package);
}

void address_sending_enable(uint8_t enable)
{

	is_address_sending_enabled = enable;
}

// Animation
void animate()
{
	if (preloaded_animations_count == 0)
		return;

	SSP_Sensor_Animation_Task* next = &(animation_tasks[next_animation_to_play]);
	SSP_Sensor_Animation_Task* current = &(animation_tasks[prev_animation_ring_index(next_animation_to_play)]);

	uint32_t elapsed = ssp_get_time_ms() - last_animation_task_time;
	uint32_t period = next->ms_from_last_state;

	if (elapsed > period)
		elapsed = period;

	sensor_state.red   = current->state.red   * (period - elapsed) / period + next->state.red   * elapsed / period;
	sensor_state.green = current->state.green * (period - elapsed) / period + next->state.green * elapsed / period;
	sensor_state.blue  = current->state.blue  * (period - elapsed) / period + next->state.blue  * elapsed / period;

	//uint8_t current_animation_state = prev_animation_ring_index(next_animation_to_play);

	if (animation_tasks[next_animation_to_play].ms_from_last_state <= ssp_get_time_ms() - last_animation_task_time)
	{
		// We need to change animation
		preloaded_animations_count--;

		sensor_state = animation_tasks[next_animation_to_play].state;

		next_animation_to_play = next_animation_ring_index(next_animation_to_play);
		last_animation_task_time = ssp_get_time_ms();
	}
}

void load_animation_task(SSP_Sensor_Animation_Task* task)
{
	if (preloaded_animations_count == MAX_ANIMATION_TASKS_COUNT-1)
		return;

	animation_tasks[next_animation_to_load] = *task;

	next_animation_to_load = next_animation_ring_index(next_animation_to_load);

	preloaded_animations_count++;
}

uint8_t next_animation_ring_index(uint8_t index)
{
	return (index+1) % MAX_ANIMATION_TASKS_COUNT;
}

uint8_t prev_animation_ring_index(uint8_t index)
{
	return index != 0 ? index-1 : MAX_ANIMATION_TASKS_COUNT-1;
}
