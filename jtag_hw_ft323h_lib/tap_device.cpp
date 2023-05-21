/*
	Dummy TAP device for following JTAG instruction flow.
*/
#include "inc\alt_jtag_enum.h"
#include "tap_device.h"


tap_device::tap_device()
{	
	m_device_id = 0x020000DD;
	m_ir_length = 10;
	reset();	
}

tap_device::tap_device(DWORD device_id)	: m_device_id(device_id)
{
	m_ir_length = 10;
	reset();
}

tap_device::~tap_device()
{

}

void tap_device::reset()
{
	m_ir_shift  = 0x155;
	m_cur_ir    = JI_IDCODE;
	m_dr_length = 32;
	m_dr_shift  = m_device_id;
	
	m_is_configured = false;	
	m_bsr_length	= 603;					// ??? only used for JI_SAMPLE_PRELOAD
	m_confdone		= (201-126)*3 + 1;		// ??? only used for JI_SAMPLE_PRELOAD and JI_STATUS
	m_iocsr_length	= 8208;					// ??? only used for JI_CONFIG_IO
	m_cram_length   = 0; // 280909197 //2944088; //2944296	// only used for JI_PROGRAM, likely bitstream length. What is this for Stratix V GS???
}

/*
	Processing rising edge of clock.
	Test-Logic-Reset.

*/
bool tap_device::clock_rise(bool tms, bool tdi)
{
	switch ( m_tap.state() ) {
		case TAP_DR_SHIFT:
			m_tdo = dr_shift(tdi);
			m_tdo_oe = true;
			break;

		case TAP_IR_SHIFT:
			m_tdo = ir_shift(tdi);
			m_tdo_oe = true;
			break;

		case TAP_DR_SELECT:
			dr_select();
			break;
		
		case TAP_IR_SELECT:
			dr_select();
			break;

		case TAP_IR_CAPTURE:
			ir_capture();
			break;

		case TAP_DR_CAPTURE:
			dr_capture();
			break;

		default:
			break;
	}

	return m_tdo;
}

/*
	Processing falling edge of clock.
	Update output data.
*/
bool tap_device::clock_fall(bool tms, bool tdi)
{
	switch ( m_tap.state() ) {
		case TAP_TLR:
			reset();
			break;

		case TAP_RTI:
			break;

		case TAP_IR_UPDATE:
			ir_update();
			break;

		case TAP_DR_UPDATE:
			dr_update();
			break;

		default:
			break;
	}
	
	m_tdo_pad = m_tdo_oe ? m_tdo : m_tdo_pad;

	return m_tdo_pad;
}

bool tap_device::clock(bool tms, bool tdi)
{
	m_tdo_oe = false;
	// process rising edge old state
	clock_rise(tms, tdi);

	// update new state
	m_tap.clock(tms);

	// process falling edge new state
	clock_fall(tms, tdi);

	return m_tdo_pad;
}

int	tap_device::state(void)
{
	return m_tap.state();
}

bool tap_device::ir_shift(bool tdi)
{
	bool tdo = m_tdo;

	if ( m_tap.state() != TAP_IR_SHIFT) return tdo;

	tdo = shift_reg(tdi, m_ir_shift, m_ir_length);	
	return tdo;
}

bool tap_device::dr_shift(bool tdi)
{
	bool tdo = m_tdo;

	if ( m_tap.state() != TAP_DR_SHIFT) return tdo;

	switch (m_cur_ir) {
		case JI_PROGRAM:
			// update core register
			tdo = shift_reg(tdi, m_dr_shift, m_dr_length);
			//if (!m_cram_length--) { m_is_configured = true; }
			m_cram_length++;
			break;
		// return status
		case JI_STATUS:
			tdo = shift_reg(tdi, m_dr_shift, m_dr_length);
			//if (!m_confdone--) tdo = 1;
			tdo = 1;
			break;

		case JI_SAMPLE_PRELOAD:
			shift_reg(tdi, m_dr_shift, m_dr_length);
			tdo = 1;
			break;

		case JI_USER0:
			// sld_dr_shift();
			// if (m_is_configured) tdo = m_hub.shift_hub_info(tdi);
			tdo = shift_reg(tdi, m_dr_shift, m_dr_length);
			break;

		case JI_USER1:
			// sld_ir_shift();
			// if (m_is_configured) tdo = m_hub.shift_vir(tdi);
			tdo = shift_reg(tdi, m_dr_shift, m_dr_length);
			break;
		
		// select bypass
		default:
			tdo = shift_reg(tdi, m_dr_shift, m_dr_length);
			break;
	}
	
	return tdo;
}

/*
	Select-IR-Scan.

*/
void tap_device::ir_select()
{

}

/*
	Capture-IR.
	Load patter to shift register.
	lsb[1:0] = 0b01;
*/
void tap_device::ir_capture()
{
	// load pattern
	m_ir_shift = 0x155; // 0b0101010101
}

void tap_device::ir_update()
{
	m_cur_ir = m_ir_shift & ( (1 << m_ir_length) - 1 );
	// select data path
}

/*
	Capture-DR.
	Rising edge.
	Load parallel data to selected capture test data registers.
*/
void tap_device::dr_capture()
{
	// m_bypass  = m_dr_shift & 1;	
}

/*
	Update-DR.
	Falling edge.
	Copy data from shift register to selected data register.
*/
void tap_device::dr_update()
{
	switch (m_cur_ir) {
		case JI_PROGRAM:
			// update core register
			break;

		case JI_USER0:
			// if (m_is_configured) m_hub.sld_hub_update();
			break;

		case JI_USER1:
			// if (m_is_configured) // m_hub.sld_vir_update();
			break;
		
		// select bypass
		default:
			m_bypass = m_dr_shift & 1;
			break;
	}
}

/*
	Select-DR-Scan.
	Rising edge.
	Select Data chain
*/
void tap_device::dr_select()
{
	switch (m_cur_ir) {
		// reset configuration
		case JI_PULSE_NCONFIG:
			m_dr_length = 1;
			m_dr_shift = m_bypass & 1;
			break;
		
		// start configuration
		case JI_PROGRAM:
			m_dr_length = m_cram_length; // bitstream length
			m_dr_shift = m_bypass & 1;
			break;

		// start user mode
		case JI_STARTUP:
			m_dr_length = 1; // bypass reg
			m_dr_shift = m_bypass & 1;
			break;
		
		// return status
		case JI_STATUS:
		case JI_SAMPLE_PRELOAD:
			m_dr_length = m_bsr_length;
			m_dr_shift  = 0;
			m_confdone  = (201-126)*3 + 1;
			break;

		case JI_IDCODE:
			m_dr_shift = m_device_id;
			m_dr_length = 32;
			break;

		case JI_USERCODE:
			if (m_is_configured) {
				m_dr_length = 32;
				m_dr_shift = 0x12345678;
			} else {
				m_dr_length = 1;
				m_dr_shift = m_bypass & 1;		
			}
			// if not configured m_dr_length = 1;
			
			break;

		case JI_CONFIG_IO:
			m_dr_length = m_iocsr_length;
			m_dr_shift = m_bypass & 1;
			break;
		
		case JI_USER0:
			// if (m_is_configured) m_hub.sld_vir_select();
			m_dr_length = 1;
			m_dr_shift = m_bypass & 1;
			break;

		case JI_USER1:
			// if (m_is_configured) m_hub.sld_vdr_select();
			m_dr_length = 1;
			m_dr_shift = m_bypass & 1;
			break;
		
		// select bypass
		default:
			m_dr_length = 1;
			m_dr_shift = m_bypass & 1;
			break;
	}
}

const char* tap_device::instruction_str()
{
	switch (m_cur_ir)
	{
	case		JI_EXTEST: return "EXTEST";
	case		JI_PULSE_NCONFIG: return "PULSE_NCONFIG";
	case 		JI_PROGRAM: return "PROGRAM";
	case		JI_STARTUP: return "STARTUP";
	case		JI_STATUS: return "STATUS";
	case 		JI_SAMPLE_PRELOAD: return "SAMPLE_PRELOAD";
	case 		JI_IDCODE: return "IDCODE";
	case 		JI_USERCODE: return "USERCODE";
	case 		JI_CLAMP: return "CLAMP";
	case 		JI_HIGHZ: return "HIGHZ";
	case 		JI_USER0: return "USER0";
	case 		JI_CONFIG_IO: return "CONFIG_IO";
	case 		JI_USER1: return "USER1";
	case 		JI_EXTEST2: return "EXTEST2";

		// test group
	case 		JI_EDERROR_INJECT: return "EDERROR_INJECT";
	case 		JI_SHIFT_EDERROR_REG: return "SHIFT_EDERROR_REG";
	case 		JI_EXTEST_TRAIN: return "EXTEST_TRAIN";
	case 		JI_EXTEST_PULSE: return "EXTEST_PULSE";

		// ??
	case 		JI_FPGA_ISC_ENABLE: return "FPGA_ISC_ENABLE";
	case 		JI_FPGA_ISC_DISABLE: return "FPGA_ISC_DISABLE";
	case 		JI_FPGA_ISC_ADDRESS_SHIFT: return "FPGA_ISC_ADDRESS_SHIFT";
	case 		JI_FPGA_ISC_PROGRAM: return "FPGA_ISC_PROGRAM";

		// AS/AP control
	case 		JI_EN_ACTIVE_CLK: return "EN_ACTIVE_CLK";
	case 		JI_DIS_ACTIVE_CLK: return "DIS_ACTIVE_CLK";
	case 		JI_ACTIVE_DISENGAGE: return "ACTIVE_DISENGAGE";
	case 		JI_ACTIVE_ENGAGE: return "ACTIVE_ENGAGE";
	case 		JI_APFC_BOOT_ADDR: return "APFC_BOOT_ADDR";
	case 		JI_FACTORY: return "FACTORY";

		// security key
	case 		JI_FUSE_WRITE: return "FUSE_WRITE";
	case 		JI_KEY_VERIFY: return "KEY_VERIFY";
	case 		JI_KEY_CLR_VREG: return "KEY_CLR_VREG";
	case 		JI_TEST_DISABLE: return "TEST_DISABLE";
	case 		JI_VOL_KEY_ZERO: return "VOL_KEY_ZERO";
	case 		JI_KEY_SECURE_OTP: return "KEY_SECURE_OTP";
	case 		JI_VOL_KEY_SECURE: return "VOL_KEY_SECURE";
	case 		JI_VOL_KEY_LOCK: return "VOL_KEY_LOCK";
	case 		JI_KEY_PROG_VOL: return "KEY_PROG_VOL";
	case 		JI_LOCK: return "LOCK";
	case 		JI_RESET_AES: return "RESET_AES";
	case 		JI_VOL_KEY_LOCK_EN: return "VOL_KEY_LOCK_EN";
	case 		JI_KEY_PROG_OTP: return "KEY_PROG_OTP";
	case 		JI_KEY_PROG_OTP_EN: return "KEY_PROG_OTP_EN";
	case 		JI_UNLOCK: return "UNLOCK";
	case 		JI_IP_RECONFIG: return "IP_RECONFIG";

		// ASC 	
	case 		JI_EPC_IDCODE: return "EPC_IDCODE";
	case 		JI_EPC_USERCODE: return "EPC_USERCODE";
	case 		JI_EPC_ISC_ENABLE: return "EPC_ISC_ENABLE";
	case 		JI_EPC_ISC_DISABLE: return "EPC_ISC_DISABLE";
	case 		JI_EPC_ISC_PROGRAM: return "EPC_ISC_PROGRAM";
	case 		JI_EPC_ISC_ERASE: return "EPC_ISC_ERASE";
	case 		JI_EPC_ISC_ADDRESS_SHIFT: return "EPC_ISC_ADDRESS_SHIFT";
	case 		JI_EPC_ISC_READ_INFO: return "EPC_ISC_READ_INFO";
	case 		JI_EPC_ISC_READ: return "EPC_ISC_READ";
	case 		JI_EPC_ISC_NOOP: return "EPC_ISC_NOOP";
	case 		JI_EPC_ISC_STAT: return "EPC_ISC_STAT";

		// MAX3000	
	case 		JI_M3K_SAMPLE: return "M3K_SAMPLE";
	case 		JI_M3K_ISC_ENABLE: return "M3K_ISC_ENABLE";
	case 		JI_M3K_ISC_PROGRAM: return "M3K_ISC_PROGRAM";
	case 		JI_M3K_ISC_READ: return "M3K_ISC_READ";
	case 		JI_M3K_ISC_READ_H: return "M3K_ISC_READ_H";
	case 		JI_M3K_ISC_READ_L: return "M3K_ISC_READ_L";

		// MAX7000	
	case 		JI_M7K_CLAMP: return "M7K_CLAMP";
	case 		JI_M7K_ISC_ENABLE0: return "M7K_ISC_ENABLE0";
	case 		JI_M7K_ISC_NOOP1: return "M7K_ISC_NOOP1";

		// MAX II	
	case 		JI_MII_ISC_ENABLE: return "MII_ISC_ENABLE";
	case 		JI_MII_ISC_DISABLE: return "II_ISC_DISABLE";
	case 		JI_MII_ISC_PROGRAM: return "MII_ISC_PROGRAM";
	case 		JI_MII_ISC_ERASE: return "MII_ISC_ERASE";
	case 		JI_MII_ISC_ADDRESS_SHIFT: return "MII_ISC_ADDRESS_SHIFT";
	case 		JI_MII_ISC_READ: return "MII_ISC_READ";
	case 		JI_MII_ISC_NOOP: return "MII_ISC_NOOP";

		// MAX V	

		// MAX 10	
	case 		JI_MAX10_ISC_ENABLE_CLAMP: return "MAX10_ISC_ENABLE_CLAMP";
	case 		JI_MAX10_BGP_ENABLE: return "MAX10_BGP_ENABLE";
	case 		JI_MAX10_BGP_DISABLE: return "MAX10_BGP_DISABLE";

		// S10/A10
	case 		JI_ISC_DSM_VERIFY: return "ISC_DSM_VERIFY";
	case 		JI_ISC_DSM_ERASE: return "ISC_DSM_ERASE";
	case 		JI_ISC_DSM_PROGRAM: return "ISC_DSM_PROGRAM";

		// private instructions
		// CUDA
	case 		JI_CUDA_PRIVATE2: return "CUDA_PRIVATE2";
	case 		JI_CUDA_PRIVATE3: return "CUDA_PRIVATE3";
		// HARDCOPY
	case 		JI_HC_PRIVATE0: return "HC_PRIVATE0";
	case 		JI_HC_PRIVATE1: return "HC_PRIVATE1";
	case 		JI_HC_PRIVATE2: return "HC_PRIVATE2";
	case 		JI_HC_PRIVATE3: return "HC_PRIVATE3";
		// MAX10
	case 		JI_MAX10_PRIVATE1: return "MAX10_PRIVATE1";
	case 		JI_MAX10_PRIVATE2: return "MAX10_PRIVATE2";

		// Stratix V
	case		JI_S5_PRIVATE1: return "S5_PRIVATE1";
	case		JI_S5_PRIVATE2: return "S5_PRIVATE2";
	case		JI_S5_PRIVATE3: return "S5_PRIVATE3";
	case		JI_S5_PRIVATE4: return "S5_PRIVATE4";
	
	case 		JI_BYPASS: return "BYPASS";
	default:
		return "UNKNOWN";
	}
}