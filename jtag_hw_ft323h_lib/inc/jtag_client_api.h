#paragma once
#ifndef _JTAG_CLIENT_API_H
#define _JTAG_CLIENT_API_H

//#define	 AJI_IMPL

#ifdef AJI_IMPL
	#include "aji_client_impl.h"
#else
	#include "aji_client_types.h"
#endif

/* JTAG Logic API */
AJI_ERROR aji_access_dr(AJI_OPEN *, DWORD, DWORD, DWORD, DWORD, BYTE const *, DWORD, DWORD, BYTE *);
AJI_ERROR aji_access_dr(AJI_OPEN *, DWORD, DWORD, DWORD, DWORD, BYTE const *, DWORD, DWORD, BYTE *, DWORD);
AJI_ERROR aji_access_dr(AJI_OPEN *, DWORD, DWORD, DWORD, DWORD, BYTE const *, DWORD, DWORD, BYTE *, DWORD, DWORD *);
AJI_ERROR aji_access_dr_multiple(DWORD, DWORD, AJI_OPEN * const *, DWORD const *, DWORD const *, DWORD const *, BYTE const * const *, DWORD const *, DWORD const *, BYTE * const *);
AJI_ERROR aji_access_dr_repeat(AJI_OPEN *, DWORD, DWORD, DWORD, DWORD, BYTE const *, DWORD, DWORD, BYTE *, BYTE const *, BYTE const *, BYTE const *, BYTE const *, DWORD);
AJI_ERROR aji_access_dr_repeat_multiple(DWORD, DWORD, AJI_OPEN * const *, DWORD, DWORD, DWORD, BYTE const * const *, DWORD, DWORD, BYTE * const *, BYTE const *, BYTE const *, BYTE const *, BYTE const *, DWORD);

AJI_ERROR aji_access_ir(AJI_OPEN *, DWORD, DWORD *,DWORD);
AJI_ERROR aji_access_ir(AJI_OPEN *, DWORD, BYTE const *, BYTE *, DWORD);
AJI_ERROR aji_access_ir_multiple(DWORD, AJI_OPEN * const *, DWORD const *, DWORD *);

AJI_ERROR aji_access_overlay(AJI_OPEN *, DWORD, DWORD *);
AJI_ERROR aji_active_serial_command(AJI_OPEN *, DWORD, BYTE const *, DWORD, BYTE *);
AJI_ERROR aji_active_serial_command_repeat(AJI_OPEN *, DWORD, BYTE const *, DWORD, BYTE *, BYTE const *, BYTE const *, BYTE const *, BYTE const *, DWORD);
AJI_ERROR aji_passive_serial_download(AJI_OPEN *, BYTE const *, DWORD, DWORD *);
AJI_ERROR aji_direct_pin_control(AJI_OPEN *, DWORD, DWORD, DWORD *);
AJI_ERROR aji_test_logic_reset(AJI_OPEN*);
AJI_ERROR aji_run_test_idle(AJI_OPEN *, DWORD);

AJI_ERROR aji_flush(AJI_OPEN *);
AJI_ERROR aji_push(AJI_OPEN *, DWORD); // flush with param

AJI_ERROR aji_set_checkpoint(AJI_OPEN *, DWORD);
AJI_ERROR aji_get_checkpoint(AJI_OPEN *, DWORD *);
AJI_ERROR aji_any_sequence(AJI_OPEN *, DWORD, BYTE const *, BYTE const *, BYTE *);
AJI_ERROR aji_delay(AJI_OPEN *, DWORD);

/* SLD Trigger API*/
AJI_ERROR aji_watch_status(AJI_OPEN *, DWORD, DWORD, DWORD);
AJI_ERROR aji_watch_dr(AJI_OPEN *, DWORD, DWORD, DWORD, DWORD, BYTE const *, DWORD, DWORD, BYTE const *, BYTE const *, DWORD);
AJI_ERROR aji_get_watch_triggered(AJI_OPEN *, bool *);
AJI_ERROR aji_get_node_info(AJI_OPEN *, DWORD *, DWORD *);

typedef void (*AJI_PROGRESS_CALLBACK)(void *, DWORD);
void	  aji_register_progress_callback(AJI_OPEN *, void (*)(void *, DWORD), void *);

/* JTAG chain interface */
AJI_ERROR aji_lock_chain(AJI_CHAIN *, DWORD dwTimeout);
AJI_ERROR aji_unlock_chain(AJI_CHAIN *);
AJI_ERROR aji_lock(AJI_OPEN *, DWORD dwTimeout, AJI_PACK_STYLE);
AJI_ERROR aji_unlock(AJI_OPEN *);
AJI_ERROR aji_unlock_lock(AJI_OPEN *, AJI_OPEN *);

AJI_ERROR aji_unlock_chain_lock(AJI_CHAIN *, AJI_OPEN *, AJI_PACK_STYLE);
AJI_ERROR aji_unlock_lock_chain(AJI_OPEN *, AJI_CHAIN *);

AJI_ERROR aji_open_device(AJI_CHAIN *, DWORD, AJI_OPEN * *, AJI_CLAIM const *, DWORD, char const *);
AJI_ERROR aji_open_entire_device_chain(AJI_CHAIN * ,AJI_OPEN * *, AJI_CHAIN_TYPE, char const *);
AJI_ERROR aji_close_device(AJI_OPEN *);

AJI_ERROR aji_scan_device_chain(class AJI_CHAIN *);
AJI_ERROR aji_define_device(AJI_CHAIN *, DWORD, AJI_DEVICE const *);
AJI_ERROR aji_read_device_chain(AJI_CHAIN *, DWORD *, AJI_DEVICE *, bool);
AJI_ERROR aji_cancel_operation(AJI_CHAIN *);

typedef DWORD (*AJI_WATCH_CALLBACK)(void *, BYTE const *, DWORD);
AJI_ERROR aji_watch_data(AJI_CHAIN *, DWORD (*)(void *, BYTE const *, DWORD), void *, bool);

/* JTAG SLD HUB interface */
AJI_ERROR aji_get_nodes(AJI_CHAIN *, DWORD, DWORD *, DWORD *);
AJI_ERROR aji_get_nodes(AJI_CHAIN *, DWORD, DWORD *, DWORD *, DWORD *);
AJI_ERROR aji_open_node(AJI_CHAIN *, DWORD, DWORD, AJI_OPEN * *, AJI_CLAIM const *, DWORD, char const *);
AJI_ERROR aji_find_node(AJI_CHAIN *, int, int, DWORD, AJI_CLAIM const *, DWORD, char const *, AJI_OPEN * *, DWORD *);

/* AJI Hardware interface */
AJI_ERROR aji_add_hardware(struct AJI_HARDWARE *);
AJI_ERROR aji_get_hardware(DWORD *, AJI_HARDWARE *, DWORD dwTimeout);
AJI_ERROR aji_find_hardware(DWORD, AJI_HARDWARE *, DWORD dwTimeout);
AJI_ERROR aji_find_hardware(char const *, AJI_HARDWARE *, DWORD dwTimeout);
AJI_ERROR aji_get_potential_hardware(DWORD *, AJI_HARDWARE *);
AJI_ERROR aji_print_hardware_name(AJI_CHAIN *, char *, DWORD);
AJI_ERROR aji_print_hardware_name(AJI_CHAIN *, char *, DWORD, bool, DWORD *);
AJI_ERROR aji_change_hardware_settings(AJI_CHAIN *, AJI_HARDWARE const *);
AJI_ERROR aji_remove_hardware(AJI_CHAIN *);

/* AJI Hardware parameters */
AJI_ERROR aji_set_parameter(AJI_CHAIN *, char const *, DWORD);
AJI_ERROR aji_set_parameter(AJI_CHAIN *, char const *, BYTE const *, DWORD);
AJI_ERROR aji_get_parameter(AJI_CHAIN *, char const *, DWORD *);
AJI_ERROR aji_get_parameter(AJI_CHAIN *, char const *, BYTE *,DWORD *);
AJI_ERROR aji_get_parameter(AJI_CHAIN *, char const *, BYTE *,DWORD *,DWORD);

/* Server interface API */
const char* aji_get_server_version_info(void);
const char* aji_get_server_path(void);
AJI_ERROR aji_add_remote_server(char const *, char const *);
AJI_ERROR aji_add_remote_server(char const *, char const *,bool);
AJI_ERROR aji_get_servers(DWORD *,char const * *,bool);
AJI_ERROR aji_enable_local_jtagserver(bool);
AJI_ERROR aji_replace_local_jtagserver(char const *);
AJI_ERROR aji_configuration_in_memory(bool);
AJI_ERROR aji_load_quartus_devices(char const *);
AJI_ERROR aji_check_password(DWORD, BYTE const *, BYTE const *);
AJI_ERROR aji_enable_remote_clients(bool, char const *);
AJI_ERROR aji_get_remote_clients_enabled(bool *);
AJI_ERROR aji_define_device(AJI_DEVICE const *);
AJI_ERROR aji_undefine_device(AJI_DEVICE const *);
AJI_ERROR aji_get_defined_devices(DWORD *, AJI_DEVICE *);
AJI_ERROR aji_get_local_quartus_devices(DWORD *, AJI_DEVICE *);

/* Server records */
AJI_ERROR aji_add_record(AJI_RECORD const *);
AJI_ERROR aji_remove_record(AJI_RECORD const *);
AJI_ERROR aji_get_records(DWORD *, AJI_RECORD *);
AJI_ERROR aji_get_records(char const *, DWORD *, AJI_RECORD *);
AJI_ERROR aji_get_records(char const *, DWORD *, AJI_RECORD *, DWORD, char const * *);

typedef		void (*AJI_OUTPUT_CALLBACK)(void *, DWORD, char const *);
void		aji_register_output_callback( void (*)(void *, DWORD, char const *), void *);
const char* aji_get_error_info(void);

#endif