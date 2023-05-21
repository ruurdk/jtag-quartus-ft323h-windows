/*
 * alt_jtag_hw.h
 *
 * Altera JTAG Hardware Interface API header file.
 *
 *
 * Copyright (C) 2013 Altera Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once
#ifndef __JTAG_HW_H
#define	__JTAG_HW_H

#include <stdint.h>
#ifdef AJI_IMPL
	#include "aji_client_impl.h"
#else
	#include "aji_client_types.h"
#endif

// forward declaration
struct AJHI_SERVER_OPS;
class JTAG_SERVER_EXTERNAL;
struct HW_DATA;

/*
   struct AJHI_OPERATIONS
*/
#if defined _M_IX86
typedef struct {
/* 0x00 */	ULONG		cbSize;			// 0x7C,
/* 0x04 */	UCHAR		szName[32];		// Name
/* 0x24 */	DWORD		dwFeatures;		// Device Features
/* 0x28 */	union {
			DWORD		dwFlags;		// Save TDO
			void*		pUnused;		// for proper alignment SBZ
			} UNNAMED;
// __declspec(align(4))	// for 32bit x86
/* 0x2C */	void		*pUnused;				// Unknown/Reserved	SBZ
/* 0x30 */	BOOL	 	(*op_scan_ports)		(int , char *device_list, int);
/* 0x34 */	void		(*op_pfnUnused0);		// Unknown/Reserved	SBZ
/* 0x38 */	AJI_ERROR	(*op_open_hardware)		(struct HW_DATA** ppClientData, char const* dev_name, AJHI_SERVER_OPS const*, JTAG_SERVER_EXTERNAL const* );
/* 0x3C */	AJI_ERROR	(*op_close_hardware)	(struct HW_DATA* pClientData);
/* 0x40 */	AJI_ERROR	(*op_set_param)			(struct HW_DATA* pClientData, char const * param_name, unsigned long val);
/* 0x44 */	AJI_ERROR	(*op_get_param)			(struct HW_DATA* pClientData, char const * param_name, unsigned long * retval);
/* 0x48 */	BOOL		(*op_direct_control)	(struct HW_DATA* pClientData, unsigned long, unsigned long *);
/* 0x4C */	AJI_ERROR	(*op_clock_raw)			(struct HW_DATA* pClientData, bool, bool , int, int);
/* 0x50 */	AJI_ERROR	(*op_clock_multiple)	(struct HW_DATA* pClientData, bool, void*, int, int);
/* 0x54 */	AJI_ERROR	(*op_delay)				(struct HW_DATA* pClientData, unsigned long);
/* 0x58 */	BOOL		(*op_flush)				(struct HW_DATA* pClientData, BOOL, int);
/* 0x5C */	AJI_ERROR	(*op_enable_recirculate)(struct HW_DATA* pClientData, unsigned long);
/* 0x60 */	AJI_ERROR	(*op_disable_recirculate)(struct HW_DATA* pClientData);
/* 0x64 */	AJI_ERROR	(*op_recirculate)		(struct HW_DATA* pClientData, unsigned long, bool, int);
/* 0x68 */	AJI_ERROR	(*op_set_param_block)	(struct HW_DATA* pClientData, char const* name, unsigned char const *buffer, unsigned long size);
/* 0x6C */	AJI_ERROR	(*op_get_param_block)	(struct HW_DATA* pClientData, char const* name, unsigned char *buffer, unsigned long *size);
/* 0x70 */	AJI_ERROR	(*op_init_driver)		(struct HW_DATA** ppClientData, char const* dev_name, AJHI_SERVER_OPS const*, JTAG_SERVER_EXTERNAL const* );
/* 0x74 */	AJI_ERROR	(*op_enable_active_serial)(bool);
/* 0x78 */	AJI_ERROR	(*op_remove_driver)		(char const* );
} AJHI_OPERATIONS;
#else

#if defined	_M_AMD64
typedef struct HW_DRIVER_x64 {
/* 0x00 */	ULONG		cbSize;			// 0xD0
/* 0x04 */	UCHAR		szName[32];		// Name
/* 0x24 */	DWORD		dwFeatures;		// Device Features
/* 0x28 */	UINT64		dwFlags;		// Save TDO
// __declspec(align(8)) 
/* 0x30 */	void		*pUnused;		// Unknown/Reserved	SBZ
/* 0x38 */	BOOL	 	(*op_scan_ports)(int , char *device_list, int);
/* 0x40 */	AJI_ERROR	(*op_pfnUnused0);		// Unknown/Reserved	SBZ
/* 0x48 */	AJI_ERROR	(*op_open_hardware)(struct HW_DATA** ppClientData, char const* dev_name, AJHI_SERVER_OPS const*, JTAG_SERVER_EXTERNAL const* );
/* 0x50 */	AJI_ERROR	(*op_close_hardware)(struct HW_DATA* pClientData);
/* 0x58 */	AJI_ERROR	(*op_set_param)(struct HW_DATA* pClientData, char const * param_name, unsigned long val);
/* 0x60 */	AJI_ERROR	(*op_get_param)(struct HW_DATA* pClientData, char const * param_name, unsigned long * retval);
/* 0x68 */	BOOL		(*op_direct_control)(struct HW_DATA* pClientData, unsigned long , unsigned long *);
/* 0x70 */	AJI_ERROR	(*op_clock_raw)(struct HW_DATA* pClientData, bool, bool, int, int);
/* 0x78 */	AJI_ERROR	(*op_clock_multiple)(struct HW_DATA* pClientData, bool, void*, int, int);
/* 0x80 */	AJI_ERROR	(*op_delay)(struct HW_DATA* pClientData, unsigned long);
/* 0x88 */	BOOL		(*op_flush)(struct HW_DATA* pClientData, BOOL, int);
/* 0x90 */	AJI_ERROR	(*op_enable_recirculate)(struct HW_DATA* pClientData, unsigned long);
/* 0x98 */	AJI_ERROR	(*op_disable_recirculate)(struct HW_DATA* pClientData);
/* 0xA0 */	AJI_ERROR	(*op_recirculate)(struct HW_DATA* pClientData, unsigned long, bool, int);
/* 0xA8 */	AJI_ERROR	(*op_set_param_block)(struct HW_DATA* pClientData, char const* name, unsigned char const *buffer, unsigned long size);
/* 0xB0 */	AJI_ERROR	(*op_get_param_block)(struct HW_DATA* pClientData, char const* name, unsigned char *buffer, unsigned long *size);
/* 0xB8 */	AJI_ERROR	(*op_init_driver)(struct HW_DATA** ppClientData, char const* dev_name, AJHI_SERVER_OPS const*, JTAG_SERVER_EXTERNAL const* );
/* 0xC0 */	AJI_ERROR	(*op_enable_active_serial)(bool);
/* 0xC8 */	AJI_ERROR	(*op_remove_driver)(char const* );
} AJHI_OPERATIONS;

#else
#error "Architecture not supported. Select X86 or AMD64"
#endif

#endif
/*

*/
struct AJHI_SERVER_OPS {
/* 0x00*/	ULONG	ulSize;		// 0x18(WIN32), 0x30(WIN64)
// allign(4) on WIN32, allign(8) on WIN64
#if defined _M_IX86
__declspec(align(4))
#else
#if defined	_M_AMD64
__declspec(align(8))
#endif
#endif
/* 0x04/0x08 */	void	(*op_store_tdo)(void *, ULONG const *, ULONG);
/* 0x08/0x10 */	void	(*op_store_unknown_tdo)(void *, ULONG);
/* 0x0C/0x18 */	void	(*op_store_needed_tdo)(void *, ULONG const *, ULONG);
/* 0x10/0x20 */	int		(*op_tdo_needed)(void*, int);
/* 0x14/0x28 */	void	(*op_indicate_flush)(void *);
};

#endif