#ifndef __SYSTEM_STATES_H__
#define __SYSTEM_STATES_H__ 

enum states_tag
{
	DCE_STATE_RESET = 0,
	DCE_STATE_NORMAL,
	DCE_STATE_DEBUG,
	DCE_STATE_PUMP 
};
typedef enum states_tag states;
typedef enum parser_ret_code_tag
{
	parser_error = -1,
	parser_unknown,
	parser_ok
	
} parser_ret_code;

enum mbx_typ_tag
{
	NOTYPE = 0,						/* 0 */
	SLAC_interrupt, 
    SLAC_all_trunks_busy, 
    SLAC_error,
	SLAC_on_hook,
    SLAC_off_hook,
    SLAC_dialing_params,
    SLAC_init,
    SLAC_unconditional_branch,
	SLAC_ring,						/* 10 */
	SLAC_digit,
	SLAC_busy,
	SLAC_ring_back,
	SLAC_connect,
	SLAC_network_busy,
	SLAC_busy_timeout,
	SLAC_network_busy_timeout,
	SLAC_no_sim_timeout,
	SLAC_digit_timeout,
	SLAC_first_digit_timeout,		/* 20 */
	SLAC_disconnect_timeout,
	SLAC_ring_back_on_timeout,
	SLAC_ring_back_off_timeout,
	SLAC_ring_timeout,
	SLAC_short_ring_timeout,
	SLAC_roh_timeout,
	SLAC_slic_off_timeout,
	SLAC_SS_confirm,
	SLAC_SS_failure,
	SLAC_ssconfirm_timeout,			/* 30 */
	SLAC_ssfailure_timeout,
	SLAC_crssfailure,
	SLAC_disconnect,
	SLAC_disconnect_held,
	SLAC_dce_normal,
	SLAC_dce_pump,
	SLAC_no_service,
	SLAC_no_sim,
    SLAC_service_available,
    SLAC_flash_timer,				/* 40 */
    SLAC_invalid_flash_timer,
    SLAC_pulse_break_timer,
    SLAC_inter_pulse_timer,			
    SLAC_call_waiting,
    SLAC_call_waiting_sas_on,
    SLAC_call_waiting_sas_off,
    SLAC_cas_on,
    SLAC_cas_off,
    SLAC_ten_sec_cw_sas_on,
    SLAC_caller_id,					/* 50 */
    SLAC_call_waiting_ack,
    SLAC_cidcw_transmit,
    SLAC_call_waiting_released,
    SLAC_call_wait_unmute,
    SLAC_call_held,
    SLAC_call_active,
    SLAC_ss_cancel,
    SLAC_ss_prompt_timeout,
    SLAC_ss_prompted,
    SLAC_ss_cancel_timeout,			/* 60 */
    SLAC_crss_failed_timeout,
    SLAC_ss_disconnecttone_timeout,
    SLAC_neg_status,		
    SLAC_neg_status_timeout,
    SLAC_disable_interrupt_timeout,
    SLAC_cid_messages,
    SLAC_send_cid_message,
    SLAC_call_flashed,
    SLAC_send_dtmf_cid,
    SLAC_delay_dtmf_cid,			/* 70 */
    SLAC_done_dtmf_cid,
    SLAC_delay_dtmf_gen,
    SLAC_supv_conn,
    SLAC_supv_disconn,
    SLAC_supv_aoc,
    SLAC_supv_continue,
    AT_answer,
	AT_busy,
	AT_connect,
	AT_dialing_params,				/* 80 */
	AT_disconnect,
	AT_disconnect_held,
	AT_disable_incoming_call,
	AT_enable_incoming_call,
	AT_error,
    AT_ringing,
    AT_SS_confirm,
    AT_SS_failure,
    AT_SS_neg_status,
    AT_response,					/* 90 */
    AT_init_normal,
    AT_init_data,
    AT_restart,						
    AT_data_mode,
    AT_command_mode,
    AT_call_accept_waiting,
    AT_call_hold,
    AT_call_mpty,
    AT_call_ect,
    AT_call_active,					/* 100 */
    AT_call_held,
    AT_inactive_disconnect,
    AT_SS_flag,
    AT_start_callmetering,
    AT_stop_callmetering,
    AT_show_callmetering,
    DCE_change_state,
    DCE_uart,
    DCE_at_command,
    DCE_pump,						/* 110 */
    FM_incoming_data,
    FM_incoming_fax,
    FM_outgoing_data,
    FM_outgoing_fax,
    FM_modem_type,
    FM_start,
    FM_stop,
 	FM_AT_FTH_3,
	FM_AT_FRH_3,					/* 120 */
	FM_AT_FTS_8,
	FM_AT_FTM_96,
	FM_AT_FRM_96,
	FR_init,
	FR_start,
	FR_outgoing,
	FR_incoming,
    FR_IN_modem_char,
    FR_OUT_modem_char,
    FR_OK_IN,						/* 130 */
    FR_OK_OUT,
    FR_ERROR_IN,
    FR_ERROR_OUT,
    FR_CONNECT_IN,
    FR_CONNECT_OUT,
    FR_NO_CARRIER_IN,
    FR_NO_CARRIER_OUT,
    FR_DLE_ETX_IN,
    FR_DLE_ETX_OUT,
    FR_stop,						/* 140 */
    FR_COUNTER_OK,
    FR_RINGBACK,
    FR_BUSY,
    FR_COUNTER_OVER,
    FR_FAX_CALL,
    FR_NSF_IN,
    FR_NSF_OUT,
    FR_CSI_IN,
    FR_CSI_OUT,						/* 150 */
    FR_DIS_IN,
    FR_DIS_OUT,
    FR_TSI_IN,
    FR_DCS_IN,
    FR_DCN_IN,
    FR_DEFAULT_IN,
    FR_UNKNOWN_IN,
    FR_TRANING_END,
    FR_CFR_OUT,
    FR_MCF_OUT,						/* 160 */
    FR_RTN_OUT,
    FR_RTP_OUT,
    FR_FTT_OUT,
    FR_DEFAULT_OUT,
    FR_PUMP_OUT,
    FR_PUMP_IN,
    FR_MPS_IN,
    FR_EOM_IN,
    FR_EOP_IN,						/* 170 */
    FR_TRANING_END_IN,
    FR_TR_IN_OK,
	FR_TRANING_IN_FAULT,
	FR_FRS_OUT_START,
	FR_NOT_LASTPAGE,
	FR_BCS_START_OUT,
	FR_TIMEOUT_IN,
    FR_TIMEOUT_OUT,
    FR_finished,
    SYSTEM							/* 180 */
};
typedef enum mbx_typ_tag mbx_typ;

#endif //__SYSTEM_STATES_H__


