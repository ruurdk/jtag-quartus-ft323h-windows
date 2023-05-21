#include "stdafx.h"
#include <stdio.h>
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call) {
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
			// DBG_LOG("Debug: Load Library\n");
			break;

		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			// DBG_LOG("Debug: Unload Library\n");
			break;
	}
	return TRUE;
}

