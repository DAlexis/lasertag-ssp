#include "ssp-sensor-part.h"

#include "ssp-receiver.h"

#define MAX_ANIMATION_TASKS_COUNT	10

// Public variables
SSP_Sensor_Animation_State sensor_state;

// Private variables

static SSP_Sensor_Animation_Task animation_tasks[MAX_ANIMATION_TASKS_COUNT];
static uint32_t last_animation_task_time = 0;
static uint8_t preloaded_animations_count = 0;
static uint8_t next_animation_to_load = 0;
static uint8_t next_animation_to_play = 0;

static uint8_t next_animation_ring_index(uint8_t index);
static uint8_t prev_animation_ring_index(uint8_t index);

// Private functions prototypes

static void parse_package();
static uint8_t is_current_package_valid();
static uint8_t is_current_package_for_me();
static void S2M_Header_Struct_Init(SSP_Header* header);
static void Debug_Header_Init(SSP_Header* header);
static void send_ir_data(void);
static void animate();
static void load_animation_task(SSP_Sensor_Animation_Task* task);

// Public functions

void ssp_sensor_task_tick(void)
{
	// First, lets make animation step
	animate();

	if (is_current_package_valid())
	{
		if (is_current_package_for_me())
		{
			// We have just received a properly-formed package
			parse_package();
		}
		ssp_reset_receiver();
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
	ssp_drivers_init();
}

void ssp_send_debug_msg(char *ptr, int len)
{
	SSP_Header header;
	Debug_Header_Init(&header);
	header.size = len;

	ssp_send_data((uint8_t*) &header, sizeof(header));
	ssp_send_data((uint8_t*) ptr, len);
}

// Private functions

uint8_t is_current_package_valid()
{
	// Incoming data can be processed
	if (ssp_receiver_buffer.size < sizeof(SSP_Header))
	{
		return 0;
	}
	//printf("s=%d\n", ssp_receiver_buffer.size);
	//HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_0|GPIO_PIN_1);
	// We can read header
	SSP_Header *incoming = (SSP_Header *) ssp_receiver_buffer.buffer;

	if (ssp_receiver_buffer.size < sizeof(SSP_Header) + incoming->size)
		return 0;

	return 1;
}

uint8_t is_current_package_for_me()
{
	SSP_Header *incoming = (SSP_Header *) ssp_receiver_buffer.buffer;

	if (incoming->target == SSP_BROADCAST_ADDRESS || incoming->target == SSP_SELF_ADDRESS)
		return 1; // Package not for me
	else
		return 0;
}

void parse_package()
{
	SSP_Header *incoming = (SSP_Header *) ssp_receiver_buffer.buffer;

	switch(incoming->command)
	{
	case SSP_M2S_GET_IR_BUFFER: send_ir_data(); return;
	case SSP_M2S_ADD_ANIMATION_TASK: load_animation_task((SSP_Sensor_Animation_Task*) (ssp_receiver_buffer.buffer + incoming->size) ); return;
	}
}

void S2M_Header_Struct_Init(SSP_Header* header)
{
	header->size = 0;
	header->target = SSP_MASTER_ADDRESS;
	header->command = SSP_S2M_NOPE;
}

void Debug_Header_Init(SSP_Header* header)
{
	S2M_Header_Struct_Init(header);
	header->command = SSP_S2M_DEBUG;
}

void send_ir_data(void)
{
	SSP_Header header;
	S2M_Header_Struct_Init(&header);
	header.command = SSP_S2M_IR_DATA;

	if (ssp_is_ir_data_ready())
	{
		// We have IR data, so sending it via SSP
		uint8_t* ir_data;
		uint16_t ir_data_size;
		ssp_get_ir_data(&ir_data, &ir_data_size);
		header.size = ir_data_size;
		ssp_send_data((uint8_t*) &header, sizeof(SSP_Header));
		ssp_send_data(ir_data, ir_data_size);
	} else {
		header.size = 0;
		ssp_send_data((uint8_t*) &header, sizeof(SSP_Header));
	}
}

// Animation
void animate()
{
	// Stub
	sensor_state.red_power = 255;
	// end stub

	if (preloaded_animations_count == 0)
		return;

	uint8_t current_animation_state = prev_animation_ring_index(next_animation_to_play);

	if (animation_tasks[next_animation_to_play].ms_from_last_state <= ssp_get_time_ms() - last_animation_task_time)
	{
		preloaded_animations_count--;
		// We need to change animation
		sensor_state = animation_tasks[next_animation_to_play].state;
		next_animation_to_play = prev_animation_ring_index(next_animation_to_play);
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
