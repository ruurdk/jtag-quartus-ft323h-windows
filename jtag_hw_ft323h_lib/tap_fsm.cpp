#include <stdio.h>
#include "tap_fsm.h"

const char*	tap_fsm::m_sz_state_names[] = {
	"RESET",
	"RUN/IDLE",

	"DRSELECT",
	"DRCAPTURE",
	"DRSHIFT",
	"DREXIT1",
	"DRPAUSE",
	"DREXIT2",
	"DRUPDATE",

	"IRSELECT",
	"IRCAPTURE",
	"IRSHIFT",
	"IREXIT1",
	"IRPAUSE",
	"IREXIT2",
	"IRUPDATE",

	"UNKNOWN"
};

// constructor
tap_fsm::tap_fsm() : m_state(TAP_UNKNOWN) , m_prev(TAP_UNKNOWN)  
{
	m_count = 0;
}

tap_fsm::~tap_fsm()
{

}

int	tap_fsm::clock( int tms )
{
	// save current state	
	m_prev = m_state;

	switch (m_state) {
		case TAP_TLR:
			m_state = tms ? TAP_TLR : TAP_RTI;
			break;

		case TAP_RTI:
			m_state = tms ? TAP_DR_SELECT : TAP_RTI;				
			break;

		case TAP_DR_SELECT:
			m_state = tms ? TAP_IR_SELECT : TAP_DR_CAPTURE;
			break;

		case TAP_IR_SELECT:
			m_state = tms ? TAP_TLR : TAP_IR_CAPTURE;
			break;

		case TAP_DR_CAPTURE:
			m_state = tms ? TAP_DR_EXIT1 : TAP_DR_SHIFT;
			break;

		case TAP_DR_SHIFT:
			m_state = tms ? TAP_DR_EXIT1 : TAP_DR_SHIFT;
			break;

		case TAP_DR_EXIT1:
			m_state = tms ? TAP_DR_UPDATE :  TAP_DR_PAUSE;				
			break;

		case TAP_DR_PAUSE:
			m_state = tms ? TAP_DR_EXIT2 : TAP_DR_PAUSE;
			break;

		case TAP_DR_EXIT2:
			m_state = tms ? TAP_DR_UPDATE : TAP_DR_SHIFT;
			break;

		case TAP_DR_UPDATE:
			m_state = tms ? TAP_DR_SELECT : TAP_RTI;
			break;

		case TAP_IR_CAPTURE:
			m_state = tms ? TAP_IR_EXIT1 : TAP_IR_SHIFT;
			break;

		case TAP_IR_SHIFT:
			m_state = tms ? TAP_IR_EXIT1 : TAP_IR_SHIFT;
			break;

		case TAP_IR_EXIT1:
			m_state = tms ? TAP_IR_UPDATE : TAP_IR_PAUSE;
			break;

		case TAP_IR_PAUSE:
			m_state = tms ? TAP_IR_EXIT2 : TAP_IR_PAUSE;
			break;

		case TAP_IR_EXIT2:			
			m_state = tms ? TAP_IR_UPDATE : TAP_IR_SHIFT;
			break;

		case TAP_IR_UPDATE:			
			m_state = tms ? TAP_DR_SELECT : TAP_RTI;
			break;

		case TAP_INIT:
			m_count = 0;
			m_state = TAP_UNKNOWN;
			// no break, fall through
		case TAP_UNKNOWN:
		default:
			if (tms) {
				if (++m_count>=5)
					m_state = TAP_TLR;
				else
					m_state = TAP_UNKNOWN;						
			} else {
				m_count = 0;
				m_state = TAP_UNKNOWN;
			}
			break;
	}

	return m_state;
}

const char*	 tap_fsm::state_name() const
{
	if (m_state < TAP_UNKNOWN) {
		return m_sz_state_names[m_state];
	} else {
		return m_sz_state_names[TAP_UNKNOWN];
	}	
}

void tap_fsm::debug_msg( )
{
	printf("DBG: Tap state = %s\n", state_name() );
}