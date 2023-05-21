/*

 */
#include "stdafx.h"
#include "inc\jtag_client_api.h"
#include "inc\alt_jtag_enum.h"

#pragma comment( lib, ".\\lib32\\jtag_client.lib" )

/*
	Sample.
	Test device hardware.
*/
void using_hardware()
{
	AJI_HARDWARE*	hw;
	DWORD			hw_count=0;
	AJI_ERROR		status;
	char			text[144];

	// get hardware count
	// set hardware pointer to zero to get number of hardwares attached
	status = aji_get_hardware( &hw_count, NULL, 250);
	if (AJI_TOO_MANY_DEVICES == status ) {
		hw = new AJI_HARDWARE[hw_count];
		status = aji_get_hardware( &hw_count, hw, 0);
		if (AJI_NOERROR != status ) {
			printf("ERROR: aji_find_hardware failed, status = %2x\n", status);
		} else {
			printf("INFO: found %d device%s\n", hw_count, hw_count>1 ? "(s)" : "");
			for (unsigned i=0; i<hw_count; i++) {
				printf("Hardware (%d): %s [%s] (%4x)\n", i, hw[i].m_szName, hw[i].m_szPort, hw[i].m_dwFeatures);	
			}
				
			status = aji_lock_chain(hw[0].m_chain, 250);
			
			// get short hardware name
			status = aji_print_hardware_name(hw[0].m_chain, text, sizeof(text));
			if (AJI_NOERROR == status ) 
				printf("Device name: %s \n", text);

			// get full hardware name with server			
			status = aji_print_hardware_name(hw[0].m_chain, text, sizeof(text), TRUE, NULL);
			if (AJI_NOERROR == status )
				printf("Device name: %s \n", text);

			status = aji_unlock_chain(hw[0].m_chain);
		}

		delete hw;		
	}
}

/*
	Sample.
	Server functions.
*/
void using_server_func()
{
	AJI_ERROR		status;
	status = aji_enable_local_jtagserver(TRUE);

	const char* path = aji_get_server_path();
	const char *version = aji_get_server_version_info();
	printf("Server path: %s\n", path);
	printf("Server version: %s\n", version);

	// get server list
	const char*		server_list[16];
	DWORD			count = sizeof(server_list)/sizeof(server_list[0]);	

	// status = aji_get_servers(&count, server_list, TRUE);
	status = aji_get_servers(&count, server_list, FALSE);
	if (count) {
		printf("Server list:\n");
		for (unsigned i=0; i<count; i++) {
			if ( server_list[i] ) printf("Server[%d] = %s\n", i, server_list[i] );
		}
	} else {
		printf("No active servers\n");
	}
	
	/*
	status = aji_replace_local_jtagserver(0);
	if (status) {
		const char *ss = aji_get_error_info();
		sprintf(text, "%s", ss);
		printf("Status = %2x, %s\n", status, text);

	}
	*/
}


int main(int argc, _TCHAR* argv[])
{
	AJI_ERROR		status;
	AJI_HARDWARE	hw[4] = {0};
	DWORD			buffer[256] = {0};
	DWORD			count=0;

	// enable local server
	status = aji_enable_local_jtagserver(TRUE);

	// sample usage of server group functions
	using_server_func();
	// sample usage of hardware group functions
	using_hardware();

	// get device count
	status = aji_get_hardware( &count, NULL, 250);
	if (AJI_TOO_MANY_DEVICES == status ) {
		count  = min( count, sizeof(hw)/sizeof(hw[0]) );
		status = aji_get_hardware( &count, hw, 0);	
	}

	if (count) {
		printf("Using hardware (%d): %s [%s] (%4x)\n", 0, hw[0].m_szName, hw[0].m_szPort, hw[0].m_dwFeatures);	

		AJI_CHAIN*		chain = hw[0].m_chain;
		AJI_OPEN*		handle = 0;
		// создаем список разрешенных инструкций
		AJI_CLAIM		claims[] = {
							{ AJI_CLAIM_BASIC, JI_PULSE_NCONFIG },
							{ AJI_CLAIM_BASIC, JI_PROGRAM },
							{ AJI_CLAIM_BASIC, JI_STARTUP },
							{ AJI_CLAIM_BASIC, JI_STATUS },
							{ AJI_CLAIM_BASIC, JI_IDCODE },
							{ AJI_CLAIM_BASIC, JI_USERCODE },
							{ AJI_CLAIM_BASIC, JI_BYPASS } };

		status = aji_lock_chain(hw[0].m_chain, 250);		
		status = aji_scan_device_chain(hw[0].m_chain);
		if (AJI_NOERROR != status ) {
			printf("Error: aji_scan_device_chain failed, Status = %2x, %s\n", status, aji_get_error_info());
		}
		
		AJI_DEVICE	dev1[16];
		DWORD		size = 16;
		
		status = aji_read_device_chain( chain, &size, dev1, 0);
		if (AJI_NOERROR != status ) {
			printf("Error: aji_read_device_chain failed, Status = %2x, %s\n", status, aji_get_error_info());
		}
		
		status = aji_open_device(chain, 0, &handle, (const AJI_CLAIM*)&claims, sizeof(claims)/sizeof(AJI_CLAIM), "AJI_TEST");		
		if (AJI_NOERROR != status ) {
			printf("Error: aji_open_device failed, Status = %2x, %s\n", status, aji_get_error_info());
		}
		
		status = aji_unlock_chain(chain);		
		if (AJI_NOERROR != status ) {
			printf("Error: aji_unlock_chain failed, status = %2x, %s\n", status, aji_get_error_info());
		}
		
		if ( handle ) {
			status = aji_lock(handle, 500, AJI_PACK_SIMPLE );
		   	status = aji_test_logic_reset(handle);
			status = aji_run_test_idle(handle, 100);
			status = aji_flush(handle);

			DWORD	dw = -1;
			// read device id
			status = aji_access_ir(handle, JI_IDCODE, 0, 0);
			status = aji_access_dr(handle, 32, 0, 0, 32, (const BYTE*)&dw, 0, 32, ( BYTE*)&dw);
			if (AJI_NOERROR == status ) {
				printf("IDCODE   = %08x\n", dw);
			} else {
				printf("Error: aji_access_dr failed, status = %2x, %s\n", status, aji_get_error_info());
			}
			
			// read usercode
			status = aji_access_ir(handle, JI_USERCODE, 0, 0);
			status = aji_access_dr(handle, 32, 0, 0, 32, (const BYTE*)&dw, 0, 32, ( BYTE*)&dw);
			if (AJI_NOERROR == status ) {
				printf("USERCODE = %08x\n", dw);
			} else {
				printf("Error: aji_access_dr failed, status = %2x, %s\n", status, aji_get_error_info());
			}
			// pulse nconfig
			status = aji_access_ir(handle, JI_PULSE_NCONFIG, 0, 0);
			// delay
			status = aji_run_test_idle(handle, 1000);

			// start programmig
			// send bitstream
			BYTE	buffer[32];
			memset(buffer, 0xFF, sizeof(buffer));
			status = aji_access_ir(handle, JI_PROGRAM, 0, 0);
			// посылаем двоичный массив, данные с линии TDO игнорируем
			status = aji_access_dr(handle, sizeof(buffer)*8, 0, 0, sizeof(buffer)*8, buffer, 0, 0, 0); 

			// get status
			status = aji_access_ir(handle, JI_STATUS, 0, 0);
			// читаем регистр состояния 
			status = aji_access_dr(handle, 32, 0, 0, 0, 0, 0, 32, ( BYTE*)&dw);
			
			// delay
			status = aji_run_test_idle(handle, 1000);

			// startup
			status = aji_access_ir(handle, JI_STARTUP, 0, 0);
			
			// delay
			status = aji_run_test_idle(handle, 1000);
			
			// bypass
			status = aji_access_ir(handle, JI_BYPASS, 0, 0);
			
			status = aji_unlock(handle);

		   	if (!status) {				
				status = aji_close_device(handle);	
			}
		}
	}

	return 0;
}