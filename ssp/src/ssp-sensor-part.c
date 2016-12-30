#include "ssp-sensor-part.h"


#define MAX_INPUT_BUFFER_SIZE	20

// Private variables

static uint8_t input_buffer[MAX_INPUT_BUFFER_SIZE];
static int buffer_size = 0;
static uint8_t ready_to_precess_buffer = 0;

// Private functions

static void reset_receiver(void);
static void parse_package(SSP_M2S_Package *incoming);
static void SSP_S2M_Header_Struct_Init(SSP_S2M_Header* header);
static void SSP_Debug_Header_Init(SSP_Debug_Header* header);
static void send_ir_data(void);


// Public functions

void ssp_receive_byte(uint8_t byte)
{
	input_buffer[buffer_size++] = byte;
}

void ssp_task_tick(void)
{
	if (ssp_is_receiving_timeouted())
	{
		// Incoming data can be processed
		if (buffer_size != sizeof(SSP_M2S_Package))
		{
			reset_receiver();
			return;
		}

		SSP_M2S_Package *incoming = (SSP_M2S_Package *) input_buffer;

		if (incoming->start_byte != SSP_START_BYTE_M2S)
			return; // Package is not for sensor

		if (incoming->target != SSP_BROADCAST_ADDRESS && incoming->target != SSP_SELF_ADDRESS)
			return; // Package not for me

		parse_package(incoming);
		reset_receiver();
	}
}

void ssp_init(void)
{
	reset_receiver();
	ssp_drivers_init();
}

void ssp_send_debug_msg(char *ptr, int len)
{
	SSP_Debug_Header header;
	header.size = len;
	SSP_Debug_Header_Init(&header);

	ssp_send_data((uint8_t*) &header, sizeof(header));
	ssp_send_data((uint8_t*) ptr, len);
}

// Private functions

void reset_receiver(void)
{
	buffer_size = 0;
	ready_to_precess_buffer = 0;
}

void parse_package(SSP_M2S_Package *incoming)
{
	switch(incoming->command)
	{
	case SSP_M2S_GET_IR_BUFFER: send_ir_data(); return;
	}
}


void SSP_S2M_Header_Struct_Init(SSP_S2M_Header* header)
{
	header->start_byte = SSP_START_BYTE_S2M;
	header->package_size = 0;
	header->package_type = SSP_S2M_PACKAGE_TYPE_NOPE;
}

void SSP_Debug_Header_Init(SSP_Debug_Header* header)
{
	header->start_byte = SSP_START_BYTE_DEBUG;
}

void send_ir_data(void)
{
	SSP_S2M_Header header;
	SSP_S2M_Header_Struct_Init(&header);
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

/*
// private functions prototypes
void parse_package(SSP_M2S_Package *incoming);
//uint8_t is_bus_idle(void);
void send_ir_data(void);

// private variables
//static uint32_t last_msg_time = 0;

void ssp_init(void)
{
	ir_receiver_init();
	transiver_init();
}

void ssp_receive(uint8_t* data, uint16_t size)
{
	printf("ssp_receive\n");
	if (size != sizeof(SSP_M2S_Package))
		return; // Package size is invalid

	SSP_M2S_Package *incoming = (SSP_M2S_Package *) data;

	if (incoming->start_byte != SSP_START_BYTE_M2S)
		return; // Package is not for sensor

	if (incoming->target != SSP_BROADCAST_ADDRESS && incoming->target != SSP_SELF_ADDRESS)
		return; // Package not for me

	// Now we have valid package! We can parse it
	parse_package(incoming);
}

void ssp_send_debug_msg(char *ptr, int len)
{
	SSP_Debug_Header header;
	SSP_Debug_Header_Init(&header);

	transiver_send_data((uint8_t*) &header, sizeof(header));
	transiver_send_data((uint8_t*) ptr, len);
}

void ssp_task_tick(void)
{
	if (transiver_is_input_data_ready())
	{
		Transiver_Buffer *transiver_data = transiver_get_input_data();
		ssp_receive(transiver_data->buffer, transiver_data->size);
	}
}

*/
