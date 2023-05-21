#pragma once

#include "stdafx.h"
#include "inc\alt_jtag_hw.h"
#include "mpsse.h"
#include "tap_fsm.h"
#include "tap_device.h"

// A structure that is passed to the driver when a new device is created.
// The format of the structure is arbitrary, the top level of the server is not used
// which fields from this structure directly.
// 
// In the structure, you need to store a pointer to AJHI_SERVER_OPS
// (list of server operations) and a pointer to the server instance
// JTAG_SERVER_EXTERNAL. These items will be used to
// exchange data between the lower level and the server.
struct HW_DATA {
	AJHI_SERVER_OPS const*			pOper;			// list of server operations
	JTAG_SERVER_EXTERNAL const*		pServer;		// server location
	tap_fsm							tap;			// fake tap state machine for following JTAG state machine
	tap_device*						p_device;		// fake tap device for following JTAG instruction flow
	DWORD							dwUsbClock;		// USB clock speed
	DWORD							dwJtagClock;	// JTAG clock speed
	DWORD							dw_clock_count;	// total clocks received from server
	DWORD							dw_bitpos_tdi;	// current input (from server) buffer position (in bits)
	DWORD							dw_bitpos_tdo;	// current output (from JTAG device) buffer position (in bits)
	DWORD							dw_buff_size;	// buffer size (in bytes)
	BYTE*							tms_buffer;		// TMS buffer
	BYTE*							tdi_buffer;		// TDI buffer
	BYTE*							tdo_buffer;		// TDO buffer
	mpsse_ctx*						mpsse_ctx;		// context to MPSSE physical transport functions


	HW_DATA();
	HW_DATA(AJHI_SERVER_OPS const* ops, JTAG_SERVER_EXTERNAL const* server);
	~HW_DATA();
};

typedef struct HW_DATA* AJI_CONTEXT;

#define MAX_DATA_SIZE (32 * 1024 * 1024)