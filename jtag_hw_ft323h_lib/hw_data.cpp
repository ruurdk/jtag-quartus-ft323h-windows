#include "hw_data.h"

HW_DATA::HW_DATA()
{
	pOper = NULL;
	pServer = NULL;
	dwUsbClock = 12000000;
	dwJtagClock = 2000000;
	dw_clock_count = 0;
	dw_bitpos_tdi = 0;
	dw_bitpos_tdo = 0;
	dw_buff_size = MAX_DATA_SIZE;
	tms_buffer = (uint8_t*)malloc(MAX_DATA_SIZE);
	tdi_buffer = (uint8_t*)malloc(MAX_DATA_SIZE);
	tdo_buffer = (uint8_t*)malloc(MAX_DATA_SIZE);
	mpsse_ctx = NULL;

	assert(tdo_buffer);
	assert(tdi_buffer);
	assert(tms_buffer);

	memset(tdo_buffer, 0, dw_buff_size);
	memset(tdi_buffer, 0, dw_buff_size);
	memset(tms_buffer, 0, dw_buff_size);
}

HW_DATA::HW_DATA(AJHI_SERVER_OPS const* ops, JTAG_SERVER_EXTERNAL const* server) : HW_DATA()
{
	pOper = ops;
	pServer = server;
}

HW_DATA::~HW_DATA()
{
	free(tms_buffer);
	free(tdi_buffer);
	free(tdo_buffer);
}
