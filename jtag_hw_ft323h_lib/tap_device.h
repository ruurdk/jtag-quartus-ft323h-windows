/*
	A simple class to emulate the minimal functionality of the JTAG subsystem.

	1. Shift registers are limited to 32 bits.
	2. IDCODE, USERCODE, BYPASS registers are emulated.
	3. For full functionality, you need to increase the length of the shift registers to the desired size and
	add long register shift processing functions.
	4. To emulate the Virtual JTAG subsystem, add functions for working with the USER0/USER1 registers of the SLD_HUB subsystem.
	
 */
#pragma once
#ifndef __TAP_DEVICE_H
#define __TAP_DEVICE_H

#include "stdafx.h"
#include "tap_fsm.h"

class tap_device {
	DWORD		m_device_id;

	DWORD		m_ir_shift;
	DWORD		m_dr_shift; // replace with bit vector <bitset>
	
	DWORD		m_bypass;
	DWORD		m_confdone;
	DWORD		m_bsr_length;
	DWORD		m_iocsr_length;
	DWORD		m_cram_length;
	DWORD		m_dr_length;

	WORD		m_ir_length;	
	WORD		m_cur_ir;

	bool		m_tdo;
	bool		m_tdo_oe;
	bool		m_tdo_pad;
	bool		m_is_configured;
	tap_fsm		m_tap;
	// sld_hub		m_hub;

	// be replaced by a shift function of long bit vectors.
	bool shift_reg(bool tdi, DWORD& reg, size_t length) {
		bool tdo = (reg & 1)? 1 : 0;
		reg >>= 1;	reg |= tdi ? 1 << (length-1) : 0;
		return tdo;
	};

protected:
	virtual void	reset();
	
	virtual void	ir_select();
	virtual void	ir_capture();
	virtual	bool	ir_shift(bool tdi);
	virtual void	ir_update();
	
	virtual void	dr_select();
	virtual void	dr_capture();
	virtual	bool	dr_shift(bool tdi);
	virtual void	dr_update();	

	virtual bool	clock_rise(bool tms, bool tdi);
	virtual bool	clock_fall(bool tms, bool tdi);
public:
	tap_device();
	tap_device(DWORD device_id);
	virtual ~tap_device();	
	virtual	bool	clock(bool tms, bool tdi);
	virtual bool	last_tdo(void) { return m_tdo_pad; };
	virtual int		state(void);
	const char*		state_str(void) { return m_tap.state_name(); };
	DWORD			instruction(void) { return m_cur_ir; };
	const char*		instruction_str(void);
};

#endif