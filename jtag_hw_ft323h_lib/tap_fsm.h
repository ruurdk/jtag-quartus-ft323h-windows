#pragma once
#ifndef _TAP_STATE_H
#define _TAP_STATE_H

typedef enum e_tap_state {
	TAP_TLR,			// Test_Logic_Reset
	TAP_RTI,			// Run_Test_Idle

	TAP_DR_SELECT,		// Select_DR_Scan
	TAP_DR_CAPTURE,		// Capture_DR
	TAP_DR_SHIFT,		// Shift_DR
	TAP_DR_EXIT1,		// Exit1_DR
	TAP_DR_PAUSE,		// Pause_DR
	TAP_DR_EXIT2,		// Exit2_DR
	TAP_DR_UPDATE,		// Update_DR

	TAP_IR_SELECT,		// Select_IR_Scan
	TAP_IR_CAPTURE,		// Capture_IR
	TAP_IR_SHIFT,		// Shift_IR
	TAP_IR_EXIT1,		// Exit1_IR
	TAP_IR_PAUSE,		// Pause_IR
	TAP_IR_EXIT2,		// Exit2_IR
	TAP_IR_UPDATE,		// Update_IR
	TAP_UNKNOWN,		// TAPSTAT_RESET
	TAP_INIT			//
} tap_state_t;

class tap_fsm {
	int m_count;		// counter for tap reset logic
	int m_prev;			// prev tap_state
	int m_state;		// current tap_state
	// tap state names
	static const char*	m_sz_state_names[];
public:
	tap_fsm();
	virtual ~tap_fsm();
	// methods
	int			init()			{ return m_state = TAP_INIT; }
	// return current tap state
	int			state()			{ return m_state; }
	// return previous tap state
	int			prev_state()	{ return m_prev; }
	// reset tap SM
	int			reset()			{ return m_state = TAP_TLR; }
	// clock SM
	int			clock(int tms);

	const char*	state_name() const;
	void		debug_msg();
};

#endif // _TAP_STATE_H