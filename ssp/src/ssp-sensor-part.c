#include "ssp-sensor-part.h"


#define MAX_INPUT_BUFFER_SIZE	    20
#define MAX_ANIMATION_TASKS_COUNT	10
#define TICKS_TIMEOUT	1

// Public variables
SSP_Sensor_Animation_State sensor_state;

// Private variables

static uint8_t input_buffer[MAX_INPUT_BUFFER_SIZE];
static unsigned int buffer_size = 0;
static uint8_t ready_to_precess_buffer = 0;
static uint32_t ticks_last_receive = 0;

static SSP_Sensor_Animation_Task animation_tasks[MAX_ANIMATION_TASKS_COUNT];
static uint32_t last_animation_task_time = 0;
static uint8_t preloaded_animations_count = 0;
static uint8_t next_animation_to_load = 0;
static uint8_t next_animation_to_play = 0;

static uint8_t next_animation_ring_index(uint8_t index);
static uint8_t prev_animation_ring_index(uint8_t index);

// Private functions prototypes

static void reset_receiver(void);
static void parse_package();
static uint8_t is_current_package_valid();
static uint8_t is_current_package_for_me();
static void S2M_Header_Struct_Init(SSP_S2M_Header* header);
static void Debug_Header_Init(SSP_Debug_Header* header);
static void send_ir_data(void);
static uint8_t ssp_is_receiving_timeouted(void);
static void animate();
static void load_animation_task(SSP_Sensor_Animation_Task* task);

// Public functions

void ssp_receive_byte(uint8_t byte)
{
	ticks_last_receive = ssp_get_time_ms();
	input_buffer[buffer_size++] = byte;
}

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
		} else
			reset_receiver();
	}

	if (ssp_is_receiving_timeouted())
	{
		reset_receiver();
	}

	/* If package is not for smart sensor, we can do nothing
	 * except waiting for timeout, because data stream structure
	 * is unknown
	 */
}

void ssp_sensor_init(void)
{
	reset_receiver();
	ssp_drivers_init();
}

void ssp_send_debug_msg(char *ptr, int len)
{
	SSP_Debug_Header header;
	Debug_Header_Init(&header);
	header.size = len;

	ssp_send_data((uint8_t*) &header, sizeof(header));
	ssp_send_data((uint8_t*) ptr, len);
}

// Private functions

/* SSP will call it to check if receiving is timeouted.
 * If returns true, SSP will be forced to handle received data
 */
uint8_t ssp_is_receiving_timeouted(void)
{
	if (ssp_get_time_ms() - ticks_last_receive > TICKS_TIMEOUT)
		return 1;
	else
		return 0;
}

uint8_t is_current_package_valid()
{
	// Incoming data can be processed
	if (buffer_size < sizeof(SSP_M2S_Header))
		return 0;

	// We can read header
	SSP_M2S_Header *incoming = (SSP_M2S_Header *) input_buffer;

	if (buffer_size < sizeof(SSP_M2S_Header) + incoming->size)
		return 0;

	if (incoming->start_byte != SSP_START_BYTE_M2S)
		return 0; // Package is not for sensor

	return 1;
}

uint8_t is_current_package_for_me()
{
	SSP_M2S_Header *incoming = (SSP_M2S_Header *) input_buffer;

	if (incoming->target == SSP_BROADCAST_ADDRESS || incoming->target == SSP_SELF_ADDRESS)
		return 1; // Package not for me
	else
		return 0;
}

void reset_receiver(void)
{
	buffer_size = 0;
	ready_to_precess_buffer = 0;
}

void parse_package()
{
	SSP_M2S_Header *incoming = (SSP_M2S_Header *) input_buffer;
	switch(incoming->command)
	{
	case SSP_M2S_GET_IR_BUFFER: send_ir_data(); return;
	case SSP_M2S_ADD_ANIMATION_TASK: load_animation_task((SSP_Sensor_Animation_Task*) input_buffer + incoming->size); return;
	}
}


void S2M_Header_Struct_Init(SSP_S2M_Header* header)
{
	header->start_byte = SSP_START_BYTE_S2M;
	header->package_size = 0;
	header->package_type = SSP_S2M_PACKAGE_TYPE_NOPE;
}

void Debug_Header_Init(SSP_Debug_Header* header)
{
	header->start_byte = SSP_START_BYTE_DEBUG;
	header->size = 0;
}

void send_ir_data(void)
{
	SSP_S2M_Header header;
	S2M_Header_Struct_Init(&header);
	header.package_type = SSP_S2M_PACKAGE_TYPE_IR_DATA;

	if (ssp_is_ir_data_ready())
	{
		// We have IR data, so sending it via SSP
		uint8_t* ir_data;
		uint16_t ir_data_size;
		ssp_get_ir_data(&ir_data, &ir_data_size);
		header.package_size = ir_data_size;
		ssp_send_data((uint8_t*) &header, sizeof(header));
		ssp_send_data(ir_data, ir_data_size);
	} else {
		header.package_size = 0;
		ssp_send_data((uint8_t*) &header, sizeof(header));
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
