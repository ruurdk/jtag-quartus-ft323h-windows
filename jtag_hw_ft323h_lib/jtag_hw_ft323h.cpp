/*
	An example of a connected user driver for integration into the Quartus environment.

	This driver is intended to familiarize you with the user's writing method. soon
	module-driver for JTAG interface operation. Since this example is intended for
	It does not use the control of any hardware devices.
	To demonstrate how a driver works with a physical level as a horse many
	device creates a virtual TAP controller that emulates the operation of the EP3C5/EP3C10.

	The emulator supports the minimum functionality, i.e. The following set of JTAG instructions:
	IDCODE, USERCODE, USER0, USER1, BYPASS.
	If necessary, the desired functionality is quite simple to add to the tap_device class or
	derivative from something.

	It is also possible to connect the model to HDL in Modelsim to debug the functionality uie
	JTAG models directly in Modelsim. There is already a special interface for this
	driver for simulation.
 */
#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <setupapi.h>
#include "log.h"
#include "inc\alt_jtag_hw.h"
#include "inc\alt_jtag_enum.h"
#include "hw_data.h"

#include "mpsse.h"
#include "tap_fsm.h"
#include "tap_device.h"


#ifdef _DEBUG
#define DBG_LOG DebugLog
#else
#define DBG_LOG(x) 
#endif

#define MAX_DEVICES	1

#define JTAG_MODE (LSB_FIRST | POS_EDGE_IN | NEG_EDGE_OUT)

const UINT16 vid = 0x0403;
const UINT16 pid = 0x6014;


/*
	The function is called by the server to determine the connections
	active programmers. For devices on the USB bus, wired
	scanning of all USB buses.
	For the USB-Blaster, the port name is set in the USB-N format.


	PARAMETERS:
		Returns the name of the port to which the device is connected.

	WARNING:

*/
BOOL scan_ports(int index, char* device, int size)
{
	char	buffer[256];
	int		result = FALSE;
	
	DBG_LOG("scan_ports+\n");

	// To implement the physical layer:
	// In this function, you need to scan the USB bus if the end device î
	// use it as a physical layer.
	// For devices based on FTDI, call the functions FT_ListDevices(&numDevs, NULL, FT_LIST_NUMBER_ONLY);
	// FT_CreateDeviceInfoList(&numDevs), FT_GetDeviceInfoList(devInfo,&numDevs);
	// for devices on other transceivers calling function
	// SetupDiEnumDeviceInterfaces(), SetupDiGetDeviceInterfaceDetail() .
	// and search for the desired device.
	// The name of the port on which the device was found is returned to the server.
	if (index < 10) {
		if (device && index == 0) {
			// return port name to which device is attached
			int len = sprintf_s(buffer, sizeof(buffer), "JDBG-%d", (ULONG)index);
			if (size >= len) {
				memcpy(device, buffer, len);
				result = TRUE;
			}
		}
	}

	DBG_LOG("scan_ports-\n");
	return result;
}

/*
	The function is called by the server when the device is opened and initialized.

	PARAMETERS:
		ppClientData - pointer to where the user's context address is stored.

		dev_name - string with the name of the device port, in the name string can
		This is the number by which the driver can determine the device index.

		pOper - pointer to the AJHI_SERVER_OPS structure of the server.

		pServer is a pointer to the JTAG_SERVER_EXTERNAL class.

	WARNING:
		The returned error code.
*/
#pragma warning(push)
#pragma warning(disable:4996) // warning C4996: 'sscanf' may be unsafe.
AJI_ERROR open_hardware(AJI_CONTEXT* pContext, char const* dev_name, AJHI_SERVER_OPS const* pOper, JTAG_SERVER_EXTERNAL const* pServer)
{
	AJI_ERROR	result = AJI_NOERROR;
	HW_DATA*	pDriver;
	int			devnum = 0;
	
	DBG_LOG("open_hardware+\n");


	// To implement the physical layer:
	// In this function, you need to check whether devices are connected to the specified port 
	// and if the device is detected, then the device opens with help
	// call FT_OpenEx("USB-Blaster", FT_OPEN_BY_DESCRIPTION, &ftHandle);
	// FT_ResetDevice(ftHandle); CreateFile(), CreatePipe().
	// You can also use the network port number to use and create a network connection Unity.
	// the open device descriptor is stored in the user struct re.
	// After the initial initialization and configuration of the device, etc.

	if (pContext && dev_name) {
		DBG_LOG("open_hardware: using portname %s\n", dev_name);
		// getting the port number
		if (sscanf(dev_name, "JDBG%d", &devnum)) {
			devnum = abs(devnum);
		}
		// checking the name and number of the specified port
		if (strstr(dev_name, "JDBG") && devnum < MAX_DEVICES) {
			pDriver = new HW_DATA(pOper, pServer);

			// open device, run mpsse lib to set to JTAG mode			
			pDriver->mpsse_ctx = mpsse_open(&vid, &pid, NULL, NULL, NULL, 0);

			// create a virtual device and initialize
			pDriver->p_device = new tap_device();
			pDriver->tap.init();

			*pContext = static_cast<AJI_CONTEXT>(pDriver);
			result = AJI_NOERROR;
		}
		else {
			DBG_LOG("init_hardware: Invalid portname: %s\n", dev_name);
			result = AJI_BAD_PORT_NAME;
		}
	}
	else {
		DBG_LOG("open_hardware: Invalid parameter\n");
		result = AJI_INVALID_PARAMETER;
	}

	DBG_LOG("open_hardware-\n");
	return result;
}
#pragma warning(pop)

/*
	The function is called by the server to close the device.

	PARAMETERS:
		Context - a pointer to the user's context.

	WARNING:
		The returned error code.
*/
AJI_ERROR close_hardware(AJI_CONTEXT Context)
{	
	DBG_LOG("close_hardware+\n");

	if (Context) {
		HW_DATA* pDriver = (HW_DATA*)Context;

		mpsse_close(pDriver->mpsse_ctx);
		pDriver->mpsse_ctx = NULL;

		if (pDriver->p_device) {
			delete pDriver->p_device;
			pDriver->p_device = NULL;
		}

		delete pDriver;
	}

	DBG_LOG("close_hardware-\n");
	return AJI_NOERROR;
}

/*
	The function is called by the server when the device is initialized.
	Invoked for devices added manually.

	PARAMETERS:
		pContext - pointer to where the user's context address is stored.

		dev_name - string with the name of the device port, in the name string can
		This is the number by which the driver can determine the device index.

		pOper - pointer to the AJHI_SERVER_OPS structure of server operations.

		pServer is a pointer to the JTAG_SERVER_EXTERNAL class.

	WARNING:
		The returned error code.
*/
#pragma warning(push)
#pragma warning(disable:4996) // warning C4996: 'sscanf' may be unsafe.
AJI_ERROR init_hardware(AJI_CONTEXT* pContext, char const* dev_name, AJHI_SERVER_OPS const* pOper, JTAG_SERVER_EXTERNAL const* pServer)
{
	AJI_ERROR	result = AJI_NOERROR;
	HW_DATA* pDriver;
	int			devnum = 0;
	
	DBG_LOG("init_hardware+\n");

	// This function performs the same functions as open_hardware, but this function calls
	// for drivers that are added and removed manually, unlike drivers
	// with dynamic loadout.
	if (pContext && dev_name) {
		DBG_LOG("init_hardware: using portname %s\n", dev_name);
		// getting the port number
		if (sscanf(dev_name, "JDBG%d", &devnum)) {
			devnum = abs(devnum);
		}
		// checking the name and number of the specified port
		if (strstr(dev_name, "JDBG") && devnum < MAX_DEVICES) {
			// create a virtual device
			pDriver = new HW_DATA(pOper, pServer);

			// open device, run mpsse lib to set to JTAG mode			
			pDriver->mpsse_ctx = mpsse_open(&vid, &pid, NULL, NULL, NULL, 0);

			// create a virtual device and initialize
			pDriver->p_device = new tap_device();
			pDriver->tap.init();

			pDriver->dw_clock_count = 0;
			pDriver->dw_bitpos_tdo = 0;

			*pContext = (AJI_CONTEXT)pDriver;
			result = AJI_NOERROR;
		}
		else {
			DBG_LOG("init_hardware: Invalid portname: %s\n", dev_name);
			result = AJI_BAD_PORT_NAME;
		}
	}
	else {
		DBG_LOG("init_hardware: Invalid parameter\n");
		result = AJI_INVALID_PARAMETER;
	}

	DBG_LOG("init_hardware-\n");
	return result;
}
#pragma warning(pop)

/*
	The function is called by the server to control the TAP machine.

	PARAMETERS:
		TMS - TMS signal status
		TDI - TDI signal status
		count - number of cycles
		position - current position in bars from the beginning of the buffer
	WARNING:
		The returned error code.
*/
AJI_ERROR	clock_raw(AJI_CONTEXT Context, bool TMS, bool TDI, int count, int position)
{
	BYTE*	 tdi_buffer, * tms_buffer;
	HW_DATA* pClientData = (HW_DATA*)Context;

#ifdef verbose
	DBG_LOG("clock_raw+\n");
	DBG_LOG("clock_raw: TMS: %d TDI: %d len: %0d, position: %0d\n", TMS ? 1ul : 0ul, TDI ? 1ul : 0ul, (ULONG)count, (ULONG)position);
#endif

	tdi_buffer = pClientData->tdi_buffer;
	tms_buffer = pClientData->tms_buffer;

	const char* old_state = pClientData->tap.state_name();

	for (int i = 0; i < count; i++)
	{
		uint32_t byte_sel = pClientData->dw_bitpos_tdi / 8;
		uint32_t bit_sel = pClientData->dw_bitpos_tdi % 8;

		tdi_buffer[byte_sel] |= (((int)TDI) << bit_sel);
		tms_buffer[byte_sel] |= (((int)TMS) << bit_sel);

#ifdef verbose
		DBG_LOG("clock_raw: tms [%d:%d] = %d, tdi [%d:%d] = %d", byte_sel, bit_sel, TMS, byte_sel, bit_sel, TDI);
#endif

		pClientData->dw_bitpos_tdi += 1;

		// transfer the TAP machine to a new state
		pClientData->tap.clock(TMS);
		pClientData->dw_clock_count++;
		// transfer the fake TAP device to a new state (ignore output)
		(*pClientData->p_device).clock(TMS, TDI);

#ifdef verbose		
		if (TMS) {
			DBG_LOG("clock_raw: TAP_STATE = %s -> %s\n", old_state, pClientData->tap.state_name());
		}
#endif				
		if (pClientData->tap.state() == TAP_IR_UPDATE)
		{
			DBG_LOG("clock_raw: detected instruction: %03X (%s).\n", (*pClientData->p_device).instruction(), (*pClientData->p_device).instruction_str());
		}
		if (pClientData->tap.state() == TAP_TLR) 
		{
			DBG_LOG("clock_raw: detected reset\n");
		}
	}

#ifdef verbose	
	if (!TMS) {
		DBG_LOG("clock_raw: TAP_STATE = %s -> %s\n", old_state, pClientData->tap.state_name());
	}
	DBG_LOG("clock_raw-\n");
#endif

	return AJI_NOERROR;
}

/*
	The function is called by the server to control the TAP machine.

	PARAMETERS:
		TMS - TMS signal status
		TDI - TDI signal value buffer
		count - number of cycles
		position - current position in bars from the beginning of the buffer

	WARNING:
		The returned error code.
*/
AJI_ERROR	clock_multiple(AJI_CONTEXT Context, bool TMS, void* TDI, int count, int position)
{
	BYTE* tdi_buffer, * tms_buffer;
	HW_DATA* pClientData = (HW_DATA*)Context;
#ifdef verbose
	DBG_LOG("clock_multiple: TMS:%d bits: %p len:%d position: %d \n", TMS, TDI, (ULONG)count, (ULONG)position);
#endif
	tdi_buffer = pClientData->tdi_buffer;
	tms_buffer = pClientData->tms_buffer;

	uint8_t* in_ptr = (uint8_t*)(TDI);
	for (int i = 0; i < count; i++) {
		uint32_t in_byte_sel = i / 8;
		uint32_t in_bit_sel = i % 8;
		uint32_t byte_sel = pClientData->dw_bitpos_tdi / 8;
		uint32_t bit_sel = pClientData->dw_bitpos_tdi % 8;

		uint8_t in_bit = (in_ptr[in_byte_sel] >> in_bit_sel) & 0x1;

		tdi_buffer[byte_sel] |= (((int)in_bit) << bit_sel);
		tms_buffer[byte_sel] |= (TMS << bit_sel);

		pClientData->dw_bitpos_tdi += 1;
		pClientData->dw_clock_count += 1;

		// transfer the TAP machine to a new state
		pClientData->tap.clock(TMS);
		// transfer the fake TAP device to a new state (ignore output)
		(*pClientData->p_device).clock(TMS, in_bit);

		if (pClientData->tap.state() == TAP_IR_UPDATE)
		{
			DBG_LOG("clock_multiple: detected instruction: %03X (%s).\n", (*pClientData->p_device).instruction(), (*pClientData->p_device).instruction_str());
		}
		if (pClientData->tap.state() == TAP_TLR)
		{
			DBG_LOG("clock_multiple: detected reset\n");
		}
	}

	return AJI_NOERROR;
}

/*
	The function is called by the server to control the TAP machine.

	PARAMETERS:
		bStore - flag indicating the need to save/unload buffers
		position - current position in bars from the beginning of the buffer

	WARNING:

*/
BOOL	flush(AJI_CONTEXT Context, BOOL bStore, int position)
{
	BYTE*		tdo_buffer;
	BYTE*		tdi_buffer;
	BYTE*		tms_buffer;
	BOOL		status = TRUE;
	HW_DATA*	pClientData = (HW_DATA*)Context;

	tdo_buffer = (uint8_t*)pClientData->tdo_buffer;
	tdi_buffer = (uint8_t*)pClientData->tdi_buffer;
	tms_buffer = (uint8_t*)pClientData->tms_buffer;

#ifdef verbose
	DBG_LOG("flush+\n");

	DWORD		need_tdo;
	BOOL		flag;

	need_tdo = pClientData->pOper->op_tdo_needed((void*)pClientData->pServer, position);
	flag = (need_tdo & 0x80000000) ? 1 : 0;
	need_tdo &= 0x7FFFFFFF;

	DBG_LOG("flush: FLAG = %d, position: %d\n", (ULONG)bStore, (ULONG)position);
	DBG_LOG("flush: TAP_STATE = %s\n", pClientData->tap.state_name());
	DBG_LOG("flush: need %s tdo = %d\n", flag ? "unknown" : "known", (ULONG)need_tdo);
	DBG_LOG("flush: total_clocks = %d, tdi bits = %d\n", (ULONG)pClientData->dw_clock_count, (ULONG)pClientData->dw_bitpos_tdi);
#endif	

	// write and read everything we have to device
	for (unsigned int i = 0; i < pClientData->dw_bitpos_tdi; i++) {
		unsigned int byte_sel = i / 8;
		unsigned int bit_sel = i % 8;

		int tdi = (tdi_buffer[byte_sel] >> bit_sel) & 1;

		mpsse_clock_tms_cs(pClientData->mpsse_ctx, tms_buffer, i, tdo_buffer, i, 1, tdi, JTAG_MODE);
		pClientData->dw_bitpos_tdo++;
	}
#ifdef verbose
	DBG_LOG(" hexdump of tms followed by tdi, %d bits", pClientData->dw_bitpos_tdi);
	mini_hexdump(tms_buffer, (pClientData->dw_bitpos_tdi + 7) / 8);
	mini_hexdump(tdi_buffer, (pClientData->dw_bitpos_tdi + 7) / 8);
#endif	
	int rc = mpsse_flush(pClientData->mpsse_ctx);
#ifdef verbose	
	DBG_LOG(" [MPSSE] mpsse_flush = %d", rc);
	DBG_LOG(" [MPSSE] tdo_data_count = %d ", pClientData->dw_bitpos);
	mini_hexdump(tdo_buffer, (pClientData->dw_bitpos_tdo + 7) / 8);
#endif
	pClientData->pOper->op_store_tdo((void*)pClientData->pServer, (ULONG*)tdo_buffer, pClientData->dw_bitpos_tdo);

//#ifdef verbose
	//log instruction and result if possible
	DBG_LOG("flush: %s =>  in: %d bits, out: %d bits", pClientData->p_device->instruction_str(), pClientData->dw_bitpos_tdi, pClientData->dw_bitpos_tdo);
//#endif

	// log JTAG identifier if we find it.
	if ((pClientData->p_device->instruction() == JI_IDCODE) && (pClientData->dw_bitpos_tdo >= 64))
	{
		uint64_t jtag_ident = 0;
		memcpy(&jtag_ident, tdo_buffer, sizeof(jtag_ident));
		jtag_ident >>= 6;
		jtag_ident >>= 4;
		jtag_ident &= 0xFFFFFFFFUL;
		if ((jtag_ident != 0xFFFFFFFFUL) && ((jtag_ident & 1) == 1)) // lowest bit should always be 1 according to spec.
		{
			int manufacturer = (jtag_ident >> 1) & 0x7FF;
			int partno = (jtag_ident >> 12) & 0xFFFF;
			int version = (jtag_ident >> 28) & 0xF;
			DBG_LOG("flush: possible JTAG IDENTIFIER %lx. Manufacturer %03x. Part number %04x. Version %01x", jtag_ident, manufacturer, partno, version);
		}
	}
	// log usercode
	if ((pClientData->p_device->instruction() == JI_USERCODE) && (pClientData->dw_bitpos_tdo >= 32))
	{
		uint32_t usercode = 0;
		memcpy(&usercode, tdo_buffer, sizeof(usercode));
		DBG_LOG("flush: possible USERCODE %04x", usercode);
	}


	// empty all buffers and indicate flush
	pClientData->dw_bitpos_tdi = 0;
	pClientData->dw_bitpos_tdo = 0;
	memset(tdo_buffer, 0, pClientData->dw_buff_size);
	memset(tdi_buffer, 0, pClientData->dw_buff_size);
	memset(tms_buffer, 0, pClientData->dw_buff_size);

	pClientData->pOper->op_indicate_flush((void*)pClientData->pServer);

#ifdef verbose
	DBG_LOG("flush-\n");
#endif
	return status;
}


/*
	The function is called by the server to set the parameter.

	PARAMETERS:
		param_name - the name of the parameter
		val - parameter value

	WARNING:

*/
AJI_ERROR	set_param(AJI_CONTEXT Context, char const* param_name, unsigned long val)
{
	AJI_ERROR	status = AJI_NOERROR;
	HW_DATA* pClientData = (HW_DATA*)Context;

	DBG_LOG("set_param+\n");
	DBG_LOG("set_param: param_name = %s, val = %08X\n", param_name, val);

	if (!_strcmpi(param_name, "UsbClock")) {
		pClientData->dwUsbClock = val;
	}
	else if (!_strcmpi(param_name, "JtagClock")) {
		pClientData->dwJtagClock = val;
	}
	else if (!_strcmpi(param_name, "DebugLevel")) {
		// pClientData->dwDebugLevel = val;
	}
	else {
		status = AJI_INVALID_PARAMETER;
	}

	DBG_LOG("set_param-\n");
	return status;
}

/*
	The function is called by the server to get the parameter.

	PARAMETERS:

	WARNING:

*/
AJI_ERROR	get_param(AJI_CONTEXT Context, char const* param_name, unsigned long* retval)
{
	AJI_ERROR	status = AJI_NOERROR;
	HW_DATA* pClientData = (HW_DATA*)Context;

	DBG_LOG("get_param+\n");
	DBG_LOG("get_param: param_name = %s\n", param_name);

	if (!_strcmpi(param_name, "UsbClock")) {
		if (retval) *retval = pClientData->dwUsbClock;
		DBG_LOG("get_param: return_value = %08X\n", pClientData->dwUsbClock);
	}
	else if (!_strcmpi(param_name, "JtagClock")) {
		if (retval) {
			*retval = pClientData->dwJtagClock;
			DBG_LOG("get_param: return_value = %08X\n", *retval);
		}
	}
	else {
		if (retval) *retval = 0;
	}

	DBG_LOG("get_param-\n");
	return status;
}

/*
	The function is called by the server for direct control
	device ports.

	PARAMETERS:
		Context - user context
		data - data byte for writing to the port
		pReturnData - pointer to return value buffer

	WARNING:
		FALSE - if data is not received
		TRUE - If data was returned in pReturnData.

*/
BOOL	direct_control(AJI_CONTEXT Context, unsigned long data, unsigned long* pReturnData)
{
	HW_DATA*	pClientData = (HW_DATA*)Context;
	BOOL		fResult = FALSE;

	DBG_LOG("direct_control+\n");
	DBG_LOG("direct_control: data = %08X\n", data);

	
	if (pReturnData) {
		*pReturnData = 0x60;
	}
	
	// if (has data) fResult = TRUE;
	DBG_LOG("direct_control-\n");
	return fResult;
}

/*
	The function is called by the server for permission (denial)
		programming mode (Active Serial).

	PARAMETERS:
		fEnable - flag for enabling/disabling the AS mode.

	WARNING:
		Error code.
*/
AJI_ERROR	enable_as(bool fEnable)
{
	DBG_LOG("enable_active_serial+\n");
	DBG_LOG("enable_active_serial: flag = %d\n", fEnable);
	//depending on the given flag
	// configure hardware to work in AS/JTAG mode

	DBG_LOG("enable_active_serial-\n");
	return AJI_NOERROR;
}

/*
	Driver removal function.

	PARAMETERS:
		name - name of the port to which the device is connected.

	WARNING:
		Error code.
*/
AJI_ERROR	remove_driver(char const* name)
{
	DBG_LOG("remove_driver+\n");

	DBG_LOG("remove_driver: name = %s\n", name);
	DBG_LOG("remove_driver: Removing driver\n");

	DBG_LOG("remove_driver-\n");
	return AJI_NOERROR;
}

#pragma warning(push)
#pragma warning(disable:4996) // warning C4996: 'strcpy' may be unsafe.
BOOL find_devs(int dev_index, char* out_desc, int api_ver) {
	DBG_LOG("find_devs+ (dev_index = %d, api_ver = %d)\n", dev_index, api_ver);

	if (api_ver < 4) {
#ifdef verbose
		DBG_LOG("  return 0\n");
		DBG_LOG("find_devs-\n");
#endif
		return 0;
	}

	if (dev_index >= 1) {
#ifdef verbose
		DBG_LOG("  return 0\n");
		DBG_LOG("find_devs-\n");
#endif
		return 0;
	}

	strcpy(out_desc, "JDBG-0");

#ifdef verbose
	DBG_LOG("  return 1\n");
	DBG_LOG("find_devs-\n");
#endif
	return 1;
}
#pragma warning(pop)

/*


*/
AJHI_OPERATIONS	hw_dev_list[MAX_DEVICES] = {
		sizeof(AJHI_OPERATIONS),
		"FTDI-JTAG-Blaster",
		AJI_FEATURE_JTAG,
		0,  // nullptr
		NULL, // unused
		find_devs,
		NULL, // unused - find descriptions?
		open_hardware,
		close_hardware,
		set_param,
		get_param,
		direct_control,
		clock_raw,
		clock_multiple,
		NULL, // delay
		flush,
		NULL, // enable_recirculate
		NULL, // disable_recirculate
		NULL, // recirculate
		NULL, // set_param_block
		NULL, // get_param_block
		init_hardware,
		enable_as,
		remove_driver
};

#ifdef __cplusplus
extern "C" {
#endif

	__declspec(dllexport)
		void* get_supported_hardware(int index)
	{			
		if (index < MAX_DEVICES) {
			DBG_LOG("Debug: get_supported_hardware, index = %d\n", (ULONG)index);
			return &hw_dev_list[index];
		}
		else
			return NULL;
	}

#ifdef __cplusplus
}
#endif
