#pragma once
#ifndef _AJI_CLIENT_DEF_H
#define _AJI_CLIENT_DEF_H

enum AJI_ERROR {
	AJI_NOERROR					= 0,	// success
    AJI_DEVCIE_NOT_ATTACHED		= 1,	// 
	AJI_TIMEOUT 				= 2, 	// Timeout
	// = 3,			// Invalid HUB index
	// = 5,			// Invalid HUB index
	// = 7,			// unknown error
	// = 12,		// AJI_HUB_CHAIN_BROKEN
	AJI_UNKNOWN_HARDWARE 		= 32,	// Unknown hardware
	AJI_INVALID_CHAIN_ID 		= 33,	// Invalid CHAIN_ID
    AJI_ALREADY_LOCKED			= 34,	// Locked already
    AJI_NOT_LOCKED				= 35,	// Not locked
    AJI_CHAIN_INUSE				= 36,	// Chain in use
    AJI_NODEVICES				= 37,	// No devices
    AJI_CHAIN_NOT_CONFIGURED	= 38,	// Chain not configured
    AJI_BAD_TAP_POSITION		= 39,	// Bad TAP position
    AJI_DEVICE_NOTMATCH			= 40,	// Device doesn't match
    AJI_IR_LENGTH				= 41,	// IR length error
    AJI_DEVICE_NOT_CONFIGURED	= 42,	// Device not configured
    AJI_DEVICE_CONFLICT			= 43,	// Conflict with another device
    AJI_OPEN_ID_INVALID			= 44,	// Invalid OPEN_ID
    AJI_INVALID_PARAMETER		= 45,	// Invalid parameter
    AJI_BAD_TAP_STATE			= 46,	// Bad TAP state
	AJI_TOO_MANY_DEVICES		= 47,	// Too many devices
	AJI_DIFF_TAPS				= 48,	// Different TAPS selected
	AJI_BAD_SEQUENCE			= 49,	// Bad sequence
	AJI_INSTRUCTION_CLAIMED		= 50,	// Instruction claimed
	//
	// AJI_HUB_ERRORS
	// 52 - ir access
	// 60 - lock error	  AJI_HUB_ALREADY_LOCKED
	// 61 - unlock error  AJI_HUB_NOT_LOCKED
	// 70 - AJI_BAD_TAP_STATE

	AJI_FILE_NOT_FOUND			= 80,	// File not found
	AJI_COMM_ERROR				= 81,	// Communications error
	AJI_SERVER_ERROR			= 82,	// Server error
	AJI_OUT_OF_MEMORY			= 83,	// Out of memory
	AJI_BAD_PORT_NAME			= 84,	// Bad port name
	AJI_HW_IN_USE				= 85,	// Hardware in use
	AJI_HW_NOT_ATTACHED			= 86,	// Hardware not attached
	AJI_CHAIN_BROKEN			= 87,	// JTAG chain broken
	AJI_CLAIM_ERROR				= 88,	// 
	AJI_PORT_ACCESS				= 89, 	// Insufficient port permissions
	AJI_HW_DISABLED				= 90, 	// Hardware disabled
	//

	AJI_NOT_IMPLEMENTED			= 126,	// unknown command (not implemented)
	AJI_INTERNAL_ERROR			= 127	// Internal error
	// AJI_HUB_ERROR = 0x100, 0x101, 0x102, 0x103	
};

/*
	Device Features
*/
enum AJI_FEATURE {
	AJI_FEATURE_DUMMY 		= 0x0001, 
	AJI_FEATURE_DYNAMIC 	= 0x0002,
	AJIF_AUTOREMOVE			= 0x0002,
	AJI_FEATURE_SPECIAL 	= 0x0004,
	AJI_FEATURE_LED     	= 0x0100,
	AJI_FEATURE_BUTTON  	= 0x0200,
	AJI_FEATURE_INSOCKET 	= 0x0400,
	AJI_FEATURE_JTAG     	= 0x0800,
	AJI_FEATURE_PASSIVE  	= 0x1000,
	AJI_FEATURE_ACTIVE  	= 0x2000, 
	AJI_FEATURE_SECONDARY	= 0x4000
};

//
// 
//
enum AJI_PACK_STYLE {
	AJI_NOPACK      = 0,
	AJI_PACK_SIMPLE = 1,
	AJI_PACK_LOCK   = 2
};
// JTAG chain pack style
// Purpose unknown
// Possilbe use for allignment ???
// type1 - allign = 2
// type2 - allign = 4
// type3 - allign = 8
enum AJI_CHAIN_TYPE {
	AJI_CHAIN_TYPE0 = 0,
	AJI_CHAIN_TYPE1 = 1,
	AJI_CHAIN_TYPE2 = 2,
	AJI_CHAIN_TYPE3 = 3
};
//
// Claim types 
// 
enum AJI_CLAIM_TYPE {
	AJI_CLAIM_BASIC 	= 0x100,
	AJI_CLAIM_OVERLAY 	= 0x101,
	AJI_CLAIM_USER0 	= 0x400,
	AJI_CLAIM_USER1 	= 0x300,
	AJI_CLAIM_SPEC0 	= 0x800,
	AJI_CLAIM_SPEC1 	= 0x801
};

class	AJI_CHAIN;
class	AJI_OPEN;
class	AJI_HUB;
struct	RXMESSAGE;

typedef	AJI_OPEN*	AJI_HANDLE;

struct	AJI_CLAIM
{
	DWORD	m_type;		// AJI_CLAIM_TYPE
	DWORD	m_instr;	// JTAG instruction	code
};

// public structures
struct AJI_HARDWARE
{
	AJI_CHAIN*  m_chain;
	DWORD		m_index;			// programmer number
	char*		m_szName;			// programmer name
	char*		m_szPort;			// programmer port
	char*		m_szDescription;	// user description string
	DWORD		m_Type;				// programmer type
	char*		m_szServerName;		// server name
	DWORD		m_dwFeatures;		// driver features
};

// size = 72
// Stack Frame
struct SF
{
	SF*				m_next;
	void*			m_ref;
	void*			m_data;
};

struct AJI_DEVICE
{
	DWORD		m_ID_CODE;			// device JTAG IDCODE
	SF*			m_stack_ptr;		// unknown pointer to call stack ??
	DWORD		m_dw_IR_Len;		// IR length
	DWORD		m_dw_VIR_Len;		// VIR length
	char*		m_szDeviceName;		// Device name
};

struct AJI_RECORD
{
	char*	m_szName;	// record name
	DWORD	m_count;	// number of dwords max=16
	DWORD*	m_pData;	// dwords data array
	DWORD	m_Flags;	// flags ?? 0x40000000, 0x1
	char*	m_szValue;	// value
};

#endif
