#include "stdafx.h"
#include <stdio.h>
#include <stdint.h>

void DebugLog(const char* format, ...)
{
#define BSIZE 1000
	char buffer[BSIZE];

	va_list args;
	va_start(args, format);
	vsnprintf(buffer, BSIZE, format, args);
	va_end(args);

	OutputDebugStringA(buffer);
}

void mini_hexdump(void* ptr, unsigned int len) {    
    DebugLog("mini_hexdump(ptr = %p)\n", ptr);
    uint8_t *p = (uint8_t *)ptr;

    if (len >= 128)
    {
        uint64_t checksum = 0;
        for (unsigned int i = 0; i < len; i++)
        {
            checksum += p[i];
        }
        DebugLog("too large to print: %d, checksum: %08x", len, checksum);
        return;
    }

    char buffer[140] = "";

    for (unsigned int i = 0; i < len; i++) {
      if ((i % 8 == 0) && (i != 0)) {          
          sprintf_s(buffer + strlen(buffer), 140 - strlen(buffer), "\n");
      }
      if (i % 8 == 0) {
        sprintf_s(buffer + strlen(buffer), 140 - strlen(buffer), "%04x: ", i);
      }
      sprintf_s(buffer + strlen(buffer), 140 - strlen(buffer), "%02x ", p[i]);
    }

    if ((len % 8) != 0) {
        sprintf_s(buffer + strlen(buffer) , 140 - strlen(buffer), "\n");
    }

    DebugLog(buffer);    
}