#include "su.h"   
#include "fr_frames.h"


// In-state machine functions
state_service_func start_frs_out(void *p);
state_service_func exp_fclass_in(void *p);
state_service_func send_frs_out_connect(void *p);
state_service_func set_speed_from_DCS(void *p);
state_service_func get_page_into_buf(void *p);
state_service_func exp_ath_in(void *p);

// Out-state machine functions
state_service_func exp_fclass_out(void *p);
state_service_func exp_atd_out(void *p);
state_service_func exp_FTS_8_out(void *p);
state_service_func send_frs_in_connect(void *p);
state_service_func send_to_cc_disconnect(void *p);
state_service_func exp_ath_out(void *p);


int pump_page = 0;
int pump_page_zeros = 0;
int zeros_pump_period = 0;
int leading_zeros = 0;

int page_recv_flag = 0;
int page_in_ptr;
int page_out_ptr; 
long page_char_count_in = 0;
long page_char_count_out = 0;
long simbols_from_vxx = 0;  // Nobody uses, can be removed!
int page_start_trigger;
int fax_speed;

void sub_ret(MODEM *mod);
void frame_pump_tsk_out(int event);

#define PAGE_BUFFER_LEN 2000
#define PAGE_BUFFER_B_LEN (PAGE_BUFFER_LEN*2)
u16 page_buffer[PAGE_BUFFER_LEN];

int frame_buf[128];
int delayed_frame_buf[128];
int frame_buf_in_index = 0;
int delayed_frame_buf_in_index = 0;
int frame_buf_out_index = 0;
int frames_in_buf = 0;
int dis_received = 0;
int frame_byte_counter = 0;
int last_frame = 0;
int dcs_frame = 0;
int skip_frame_end = 0;
int dis_frame = 0;
int nsf_frame = 0;	// Not used!
int frame_start = 0;
int frm_char[2];

int in_frame_connected = 0;
int out_frame_connected = 0;

char fr_phone_num[MAX_DIGIT_COUNT];

extern MODEM In;
extern MODEM Out;
int fr_stop_flag;

extern int fr_state_in;
extern int fr_state_out;
extern int fr_task_on;
extern int clcc_check_enabled;
extern int pas_check_enabled;

char xx[2];
char xo[2];

int timer_out_ID;
int timer_in_ID;

extern int fr_direction;

void fr_init_structs(void)
{
	page_char_count_in = 0;
	page_char_count_out = 0;
	page_in_ptr = 0;
	page_out_ptr = 0;
	frame_buf_in_index = 0;
	delayed_frame_buf_in_index = 0;
	frame_buf_out_index = 0;
	frames_in_buf = 0;
	frame_byte_counter = 0;
	last_frame = 0;
	dcs_frame = 0;
	skip_frame_end = 0;
	dis_frame = 0;
	nsf_frame = 0;
	frame_start = 0;
	dis_received = 0;
	
	in_frame_connected = 0;
	out_frame_connected = 0;
	
	pump_page = 0;
	pump_page_zeros = 0;
	zeros_pump_period = 0;
	leading_zeros = 0;
	page_recv_flag = 0;
	
	fr_stop_flag = 0;

	frame_buf_in_index = 0;
	delayed_frame_buf_in_index = 0;
	frame_buf_out_index = 0;
	
	timer_out_ID = -1;
	timer_in_ID = -1;
}


//========================

void export_1(MODEM *mod, char *command, VFI driver)
{
	mod->i_stack_driver++;
	mod->in_flag[mod->i_stack_driver] = 0;
	mod->driver[mod->i_stack_driver] = driver;
	mod->string_out(command);
}

void get_frame_out_tsk(int event)
{
	int i;
	
	switch (event)
	{
		case FR_CONNECT_OUT:
			TimeDebugSPrintf("getfr CON out");
			frame_start = 0;
			frame_byte_counter = 0;
			frm_char[0] = 0;
			frm_char[1] = 0;
			nsf_frame = 0;
			dis_frame = 0;
			skip_frame_end = 0;
			Out.in_flag[Out.i_stack_driver] = 1;
			break;
	
	
		case FR_OUT_modem_char:
//			TimeDebugSPrintf("fr_out_char");
			if((!frame_start) && (Out.current_char != ADDR))
			{
					nsf_frame = 0; 
					break;
			}
			frame_start = 1;
			if(Out.current_char == ADDR)
			{
				TimeDebugSPrintf("getfr FF out");
				if(dis_received)
					send0(FR_IN,FR_BCS_START_OUT);
			}

			if(skip_frame_end)
				break;
				
			if(frame_byte_counter == 1)
			{
				if(Out.current_char == CTLLST) 
					last_frame = 1;
				frm_char[0] = Out.current_char;
			}
			else if((frame_byte_counter == 2))
			{
				if((Out.current_char & 0xfe) == DIS)
				{
					dis_frame = 1;
//					TimeDebugSPrintf("DIS");
				}
				else if((Out.current_char & 0xfe) == NSF)
				{
					Out.frame_in[frame_byte_counter++] = Out.current_char;
					
					Out.frame_in[frame_byte_counter++] = 0xB5; //USA
					Out.frame_in[frame_byte_counter++] = 0x66; //?
	
					Out.frame_in[frame_byte_counter++] = DLE;
					Out.frame_in[frame_byte_counter++] = ETX;

					if(!dis_received)
					{					
						for(i = 0; i < frame_byte_counter; i++)
							delayed_frame_buf[delayed_frame_buf_in_index++] = Out.frame_in[i];

					}
										
					skip_frame_end = 1;
					
					break;					

				}
				else if((Out.current_char & 0xfe) == MCF ||
						(Out.current_char & 0xfe) == RTP ||
						(Out.current_char & 0xfe) == RTN ||
						(Out.current_char & 0xfe) == NSF ||
						(Out.current_char & 0xfe) == CFR ||
						(Out.current_char & 0xfe) == FTT ||
						(Out.current_char & 0xfe) == CRP)
				{
					Out.frame_in[frame_byte_counter++] = Out.current_char;
					Out.frame_in[frame_byte_counter++] = DLE;
					Out.frame_in[frame_byte_counter++] = ETX;
					
					for(i = 0; i < frame_byte_counter; i++)
						frame_buf[frame_buf_in_index++] = Out.frame_in[i];
					
					skip_frame_end = 1;
					
					In.driver[In.i_stack_driver](FR_PUMP_IN);
					
					break;					
				}
				else if((Out.current_char & 0xfe) == CSI)
				{
					Out.frame_in[frame_byte_counter++] = Out.current_char;
					
					for(i = 0; i < 20; i++)
					{
						Out.frame_in[frame_byte_counter++] = 0x20;  // Space is sent instead of CSI
					}
	
					Out.frame_in[frame_byte_counter++] = DLE;
					Out.frame_in[frame_byte_counter++] = ETX;
					
					if(!dis_received)
					{					
						for(i = 0; i < frame_byte_counter; i++)
							delayed_frame_buf[delayed_frame_buf_in_index++] = Out.frame_in[i];

					}
					
					skip_frame_end = 1;
					
					break;					
				}
	
				frm_char[1] = Out.current_char;
			}
			
			if((dis_frame) && (frame_byte_counter == 4))
			{
			
				Out.current_char = Out.current_char & 0x4E;
			}
			if((dis_frame) && (frame_byte_counter == 5))
			{
				Out.current_char = Out.current_char & 0x0F;
				Out.frame_in[frame_byte_counter++] = Out.current_char;
				Out.frame_in[frame_byte_counter++] = DLE;
				Out.frame_in[frame_byte_counter++] = ETX;
				
				if(!dis_received)
				{				
					for(i = 0; i < frame_byte_counter; i++)
						delayed_frame_buf[delayed_frame_buf_in_index++] = Out.frame_in[i];
				
					for(i = 0; i < delayed_frame_buf_in_index; i++)
						frame_buf[frame_buf_in_index++] = delayed_frame_buf[i];
				}
				
				skip_frame_end = 1;
				dis_received = 1;
				
				send0(FR_IN, FR_DIS_IN);
				
				break;
			}
	
			if ((frame_byte_counter > 22) 
				&& (Out.current_char != DLE) && (Out.current_char != ETX))
				break;
			
			if(frame_buf_in_index > sizeof(frame_buf) - 3)
				break;
			if(delayed_frame_buf_in_index > sizeof(delayed_frame_buf) - 3)
				break;
			
			Out.frame_in[frame_byte_counter++] = Out.current_char;

			break;
	
		case FR_DLE_ETX_OUT:
			TimeDebugSPrintf("getfr DLE_ETX out");

			Out.in_flag[Out.i_stack_driver] = 0;
			if(frame_byte_counter > 0 && !skip_frame_end)
			{
				for(i = 0; i < frame_byte_counter; i++)
					frame_buf[frame_buf_in_index++] = Out.frame_in[i];
					
				frame_buf[frame_buf_in_index++] = ETX;
				frames_in_buf++;
				
				In.driver[In.i_stack_driver](FR_PUMP_IN);

			}

			frame_start = 0;
			frame_byte_counter = 0;

			break;
		case FR_OK_OUT:
			TimeDebugSPrintf("getfr OK out");
			if (last_frame)
			{
//				TimeDebugSPrintf("getfr LAST out");
				sub_ret(&Out);
				Out.driver[Out.i_stack_driver](FR_OK_OUT);
			}
			else
			{
				Out.string_out("AT+FRH=3\r");
				TimeDebugSPrintf("getfr FRH out");
	        }
			break;
		case FR_ERROR_OUT:                        
			TimeDebugSPrintf("ERROR");
			sub_ret(&Out);
			Out.driver[Out.i_stack_driver](FR_ERROR_OUT);
			break;
		case FR_NO_CARRIER_OUT:
			TimeDebugSPrintf("NOCARRIER");
			sub_ret(&Out);
			Out.driver[Out.i_stack_driver](FR_NO_CARRIER_OUT);
			break;
	}
}


void send_frame_in_tsk(int event)
{
	switch (event)
	{
		case FR_CONNECT_IN:
			TimeDebugSPrintf("Frm CON send in");
			in_frame_connected = 1;
			break;
		case FR_PUMP_IN:
			if(!in_frame_connected)
				break;
				
			while(in_frame_connected && (frame_buf_in_index) && (frame_buf_in_index > frame_buf_out_index))
			{	/* there is frame in buffer */
				if(!In.char_out_non_block(frame_buf[frame_buf_out_index]))
				{
					break;
				}
					
				if((frame_buf[frame_buf_out_index] == ETX) 
					&& (frame_buf[frame_buf_out_index -1] == DLE))
				{
						in_frame_connected = 0;
						TimeDebugSPrintf("Frm DLE_ETX snd in");
				}
				In.frame_out[frame_buf_out_index] = frame_buf[frame_buf_out_index];
				frame_buf_out_index++;
			}
			

			break;
		case FR_DLE_ETX_IN:
			TimeDebugSPrintf("Frm DLE_ETX snd in");
			in_frame_connected = 0;
			break;
		case FR_OK_IN:
			sub_ret(&In);
			TimeDebugSPrintf("Frm OK send in");
			in_frame_connected = 0;
			In.driver[In.i_stack_driver](FR_OK_IN);
			break;
		case FR_ERROR_IN:                        
			sub_ret(&In);
			in_frame_connected = 0;
			In.driver[In.i_stack_driver](FR_ERROR_IN);
			break;
		case FR_NO_CARRIER_IN:
			sub_ret(&In);
			in_frame_connected = 0;
			In.driver[In.i_stack_driver](FR_NO_CARRIER_IN);
			break;
	}
}

state_service_func send_frame_in(void *p)
{
	int	i;
	
	In.i_stack_driver++;
	In.in_flag[In.i_stack_driver] = 0;
	In.driver[In.i_stack_driver] = (VFI)&send_frame_in_tsk;
	
	in_frame_connected = 1;
	
	if(dis_received)
	{
		frame_buf_in_index = 0;
		frame_buf_out_index = 0;
		for(i = 0; i < delayed_frame_buf_in_index; i++)
			frame_buf[frame_buf_in_index++] = delayed_frame_buf[i];
	}
	
	return 0;
}

state_service_func check_DIS_received(void *p)
{
	if(dis_received)
		send0(FR_IN, FR_DIS_IN);
	return 0;
}


state_service_func get_speed_from_DCS(void *p)  // What if other speeds are reported
{
	int speed;
	
	speed = In.current_char & 0x3C;
	switch (speed)
	{
	case 0x00:		/* 2400 */
		xx[0] = '2';
		xx[1] = '4';
		page_start_trigger = 32;
		TimeDebugSPrintf("Speed 2400");
		fax_speed = 24;
		break;
	case 0x04:		/* 9600 */
		xx[0] = '9';
		xx[1] = '6';
		page_start_trigger = 128;
		TimeDebugSPrintf("Speed 9600");
		fax_speed = 96;
		break;
	case 0x08:		/* 4800 */
		xx[0] = '4';
		xx[1] = '8';
		page_start_trigger = 64;
		TimeDebugSPrintf("Speed 4800");
		fax_speed = 48;
		break;
	case 0x0C:		/* 7200 */
		xx[0] = '7';
		xx[1] = '2';
		page_start_trigger = 96;
		TimeDebugSPrintf("Speed 7200");
		fax_speed = 72;
		break;
	}
	return 0;
}

void get_frame_in_tsk(int event)
{
static int frame_start = 0;
	int	i;

	switch (event)
	{
		case FR_CONNECT_IN:
			TimeDebugSPrintf("Frm CON get in");
			frame_start = 0;
			frame_byte_counter = 0;
			skip_frame_end = 0;
			In.in_flag[In.i_stack_driver] = 1;
			
			break;
		case FR_IN_modem_char:
			if(In.current_char == ADDR)
			{
				TimeDebugSPrintf("frame-In start");
			}
			if((!frame_start) && (In.current_char != ADDR)) 
				break;
			
			if(skip_frame_end)
				break;
				
			frame_start = 1;
			
			if((frame_byte_counter == 1) && (In.current_char == CTLLST)) 
					last_frame = 1;
					
			if((frame_byte_counter == 2) && ((In.current_char & 0xfe) == DCS))
					dcs_frame = 1;
			
			if(frame_byte_counter == 2)
			{
				if((In.current_char & 0xfe) == TSI)
				{
					In.frame_in[frame_byte_counter++] = In.current_char;
					
					/* send empty TSI */
					for(i = 0; i < 20; i++)
					{
						In.frame_in[frame_byte_counter++] = 0x20;
					}
					
					In.frame_in[frame_byte_counter++] = DLE;
					In.frame_in[frame_byte_counter++] = ETX;
					
					for(i = 0; i < frame_byte_counter; i++)
						frame_buf[frame_buf_in_index++] = In.frame_in[i];
	
					skip_frame_end = 1;
					
					Out.driver[Out.i_stack_driver](FR_PUMP_OUT);
					
					break;
				}
				else if((In.current_char & 0xfe) == MPS ||
						(In.current_char & 0xfe) == CRP ||
						(In.current_char & 0xfe) == DCN ||
						(In.current_char & 0xfe) == EOP)
				{
					In.frame_in[frame_byte_counter++] = In.current_char;
					In.frame_in[frame_byte_counter++] = DLE;
					In.frame_in[frame_byte_counter++] = ETX;

					for(i = 0; i < frame_byte_counter; i++)
						frame_buf[frame_buf_in_index++] = In.frame_in[i];
	
					skip_frame_end = 1;
					
					Out.driver[Out.i_stack_driver](FR_PUMP_OUT);
					
					break;
				}

			}
			
					
			if((dcs_frame) && (frame_byte_counter == 4))
			{
				//In.current_char = In.current_char & 0xCE;
				//In.current_char = In.current_char & 0x4E;
				get_speed_from_DCS(0);
	// ***** 2400 -> 9600 out modem
	//			In.current_char = (In.current_char & 0xC2) | 0x04;
			}
			
			
			if((dcs_frame) && (frame_byte_counter == 5))
			{
				//In.current_char = In.current_char & 0x7F;
				In.frame_in[frame_byte_counter++] = In.current_char;
				In.frame_in[frame_byte_counter++] = DLE;
				In.frame_in[frame_byte_counter++] = ETX;
				
				for(i = 0; i < frame_byte_counter; i++)
					frame_buf[frame_buf_in_index++] = In.frame_in[i];

				skip_frame_end = 1;
				
				Out.driver[Out.i_stack_driver](FR_PUMP_OUT);
				
				break;
			}
	

			if ((frame_byte_counter > 22) 
				&& (In.current_char != DLE) && (In.current_char != ETX))
				break;
				
			In.frame_in[frame_byte_counter++] = In.current_char;

			break;
	
		case FR_DLE_ETX_IN:
			TimeDebugSPrintf("frame DLE_ETX - in");

			In.in_flag[In.i_stack_driver] = 0;
			if(!skip_frame_end && frame_byte_counter > 0)
			{		
				for(i = 0; i < frame_byte_counter; i++)
					frame_buf[frame_buf_in_index++] = In.frame_in[i];

				frame_buf[frame_buf_in_index++] = ETX;

				Out.driver[Out.i_stack_driver](FR_PUMP_OUT);
			}

			frames_in_buf++;
			frame_start = 0;
			frame_byte_counter = 0;
			break;
		case FR_OK_IN:
			TimeDebugSPrintf("frame OK - in");
			if (last_frame)
			{
				sub_ret(&In);
				{
					In.driver[In.i_stack_driver](FR_OK_IN);
					//send0(FR_IN,FR_OK_IN);
				}
			}
			else
			{
				In.string_out("AT+FRH=3\r");
				TimeDebugSPrintf("frame  FRH=3 in");	
	        }
			break;
		case FR_ERROR_IN:                        
			sub_ret(&In);
			In.driver[In.i_stack_driver](FR_ERROR_IN);
			TimeDebugSPrintf("frame  ERROR in");
			break;
		case FR_NO_CARRIER_IN:
			sub_ret(&In);
			In.driver[In.i_stack_driver](FR_NO_CARRIER_IN);
			TimeDebugSPrintf("frame  NOCARRIER in");
			break;
	}
}

//In statemashine functions

state_service_func start_frs_out(void *p)
{
	send0(FR_OUT,FR_FRS_OUT_START);

	return 0;
}

void sub_ret(MODEM *mod)
{
	if(mod->i_stack_driver == 0)
		for(;;);
	mod->i_stack_driver--;
}

void export_1_tsk_in(int event)
{
	switch (event)
	{
		case FR_OK_IN:
			TimeDebugSPrintf("FR_OK in");
			sub_ret(&In);
			send0(FR_IN,FR_OK_IN);
			break;
		case FR_CONNECT_IN:
			TimeDebugSPrintf("FR_CONNECT in");
			sub_ret(&In);
			send0(FR_IN,FR_CONNECT_IN);
			break;
		case FR_ERROR_IN:
			TimeDebugSPrintf("FR_ERROR in");
			sub_ret(&In);
			send0(FR_IN,FR_ERROR_IN);
			break;
		case FR_NO_CARRIER_IN:
			TimeDebugSPrintf("FR_NO_CARRIER in");
			sub_ret(&In);
			send0(FR_IN,FR_NO_CARRIER_IN);
			break;

	}
}

void ata_tsk_in(int event)
{
	switch (event)
	{
		case FR_OK_IN:
			//TimeDebugSPrintf("FR_OK in");
			//sub_ret(&In);
			//send0(FR_IN,FR_OK_IN);
			break;
		case FR_CONNECT_IN:
			TimeDebugSPrintf("FR_CONNECT in");
			sub_ret(&In);
			send0(FR_IN,FR_CONNECT_IN);
			break;
		case FR_ERROR_IN:
			TimeDebugSPrintf("FR_ERROR in");
			sub_ret(&In);
			send0(FR_IN,FR_ERROR_IN);
			break;
		case FR_NO_CARRIER_IN:
			TimeDebugSPrintf("FR_NO_CARRIER in");
			sub_ret(&In);
			send0(FR_IN,FR_NO_CARRIER_IN);
			break;

	}
}


state_service_func exp_fclass_in(void *p)
{
	export_1(&In, "AT+FCLASS=1\r", (VFI)&export_1_tsk_in);
//	TimeDebugSPrintf("AT+FCLASS=1 in");
	return 0;
}


state_service_func exp_AT_in(void *p)
{
	export_1(&In, "AT\r", (VFI)&export_1_tsk_in);
	return 0;
}

state_service_func exp_ATA_in(void *p)
{
	export_1(&In, "ATA\r", (VFI)&ata_tsk_in);
	TimeDebugSPrintf("ATA in");
	return 0;
}

state_service_func exp_FTH_3_in(void *p)
{
	export_1(&In, "AT+FTH=3\r", (VFI)&export_1_tsk_in);
//	TimeDebugSPrintf("AT+FTH=3 in");
	return 0;
}

state_service_func exp_FRH_3_in(void *p)
{
	export_1(&In, "AT+FRH=3\r", (VFI)&export_1_tsk_in);
//	TimeDebugSPrintf("AT+FRH=3 in");
	return 0;
}

state_service_func exp_FRS_8_in(void *p)
{
	export_1(&In, "AT+FRS=8\r", (VFI)&export_1_tsk_in);
//	TimeDebugSPrintf("AT+FRS=8 in");
	return 0;
}

state_service_func exp_FTS_8_in(void *p)
{
	export_1(&In, "AT+FTS=8\r", (VFI)&export_1_tsk_in);
	return 0;
}


state_service_func get_frame_in(void *p)
{
	frame_buf_in_index = 0;
	if(!dis_received)
		delayed_frame_buf_in_index = 0;
	frame_buf_out_index = 0;	
	frames_in_buf = 0;
	frame_byte_counter = 0;

	/* turn off DIS flag because it already has been sent and understood */	
	dis_received = 0;
	
	last_frame = 0;
	dcs_frame = 0;
	skip_frame_end = 0;

	In.i_stack_driver++;
	In.in_flag[In.i_stack_driver] = 1;
	In.driver[In.i_stack_driver] = (VFI)&get_frame_in_tsk;
	

	
//	TimeDebugSPrintf("get TSI.. in - st");

	return 0;
}

state_service_func send_frs_out_connect(void *p)
{
	send0(FR_OUT,FR_CONNECT_IN);
	return 0;
}


state_service_func set_speed_from_DCS(void *p)
{

//	TimeDebugSPrintf("TSI_DCS-rcvd");
	xo[0] = xx[0];
	xo[1] = xx[1];

	return 0;
}


state_service_func exp_FRM_xx_in(void *p)
{
	char command[11] = {'A','T','+','F','R','M','=','0','0','\r',0};
	
	command[7] = xx[0];
	command[8] = xx[1];
	
 
	export_1(&In, command, (VFI)&export_1_tsk_in);

	TimeDebugSPrintf("AT+FRMxx in");
	
	return 0;
}


state_service_func check_connect_in(void *p)
{
	switch(fr_state_in)
	{
		case ii14:
		case ii18:
		case ii27:
		case ii30:
			send0(FR_OUT,FR_CONNECT_IN);
			break;
		default:
			break;
	}
	return 0;
}

void put_into_buf(int simbol)
{
	// This routine can be called direcly from modem code
	page_buffer[page_in_ptr] = simbol & 0xFF;
	page_in_ptr++;
	page_char_count_in++;

	if (page_in_ptr == PAGE_BUFFER_LEN)
		page_in_ptr = 0;
	return;
}

int get_from_buf(int pointer)
{	
	return ((int)(page_buffer[pointer] & 0xff));
}

void page_into_buf(int event)
{
	switch (event)
	{
		case FR_IN_modem_char:
			if(page_char_count_in == 0)
			{
				if(In.current_char == 0xA || In.current_char == 0xD)
					break;
				else
					TimeDebugSPrintf("Page first char in");
			}
			put_into_buf(In.current_char);
			//timer_enable(timer_in_ID, 20);	// restart 200 ms timer at every char
			break;
		case FR_DLE_ETX_IN:
			put_into_buf(ETX);
			page_recv_flag = 0;
			In.in_flag[In.i_stack_driver] = 0;
			//timer_disable(timer_in_ID);
			TimeDebugSPrintf("Page last char in");
			break;
		case FR_NO_CARRIER_IN:
			if(page_recv_flag)
			{
				put_into_buf(0);
				put_into_buf(DLE);
				put_into_buf(ETX);
				page_recv_flag = 0;
				TimeDebugSPrintf("Short page in");
			}
			sub_ret(&In);
			send0(FR_IN,FR_OK_IN);
			//timer_disable(timer_in_ID);
			break;
		case FR_OK_IN:
			sub_ret(&In);
			send0(FR_IN,FR_OK_IN);
			//timer_disable(timer_in_ID);
			break;
		case FR_ERROR_IN:
			sub_ret(&In);
			In.driver[In.i_stack_driver](FR_ERROR_IN);
			//timer_disable(timer_in_ID);
			break;
		case FR_TIMEOUT_IN:
			put_into_buf(0);
			put_into_buf(DLE);
			put_into_buf(ETX);
			sub_ret(&In);
			send0(FR_IN,FR_OK_IN);
			break;
	}
}

state_service_func fifo_reset(void *p)
{
	page_in_ptr = 0;
	page_out_ptr = 0; 
	page_char_count_in = 0;
	page_char_count_out = 0;
	
	simbols_from_vxx =0;

	return 0;
}

state_service_func get_page_into_buf(void *p)
{
	In.i_stack_driver++;
	In.in_flag[In.i_stack_driver] = 1;
	In.driver[In.i_stack_driver] = (VFI)&page_into_buf;
//	TimeDebugSPrintf("P Pump - in start");

	page_recv_flag = 1;	
	return 0;
}

state_service_func check_DCS_DCN_in(void *p)
{
	switch (In.frame_in[2] & 0xfe)
	{
		case DCS:
			send0(FR_IN, FR_DCS_IN);
//			TimeDebugSPrintf("DCS received in");
			break;
		case DCN:
			send0(FR_IN, FR_DCN_IN);
//			TimeDebugSPrintf("DCN received in");
			break;
		default:
			send0(FR_IN, FR_DEFAULT_IN);
//			TimeDebugSPrintf("DCS check neg in");
			break;				
	}
	
	return 0;
}

state_service_func check_MPS_EOP_in(void *p)
{
	switch (In.frame_in[2] & 0xfe)
	{
		case MPS:
			send0(FR_IN, FR_MPS_IN);
			break;
		case EOP:
			send0(FR_IN, FR_EOP_IN);
			break;
		default:
			send0(FR_IN, FR_DEFAULT_IN);
			break;				
	}
	
	return 0;
}

state_service_func check_CFR_FTT_in(void *p)
{
	switch (Out.frame_in[2] & 0xfe)
	{
		case CFR:
			send0(FR_IN, FR_CFR_OUT);
			break;
		case FTT:
			send0(FR_IN, FR_FTT_OUT);
			break;
		default:
			send0(FR_IN, FR_DEFAULT_OUT);
			break;				
	}
	
	return 0;
}

state_service_func check_MCF_RTN_RTP_in(void *p)
{
	switch (Out.frame_in[2] & 0xfe)
	{
		case MCF:
			send0(FR_IN, FR_MCF_OUT);
			break;
		case RTN:
			send0(FR_IN, FR_RTN_OUT);
			break;
		case RTP:
			send0(FR_IN, FR_RTP_OUT);
			break;
		default:
			send0(FR_IN, FR_DEFAULT_OUT);
			break;				
	}
	
	return 0;
}

/* Not used
state_service_func frm_decode_in(void *p)
{
	switch (In.frame_in[2] & 0xfe)
	{
	case DCS:
		send0(FR_IN, FR_DCS_IN);
		break;
	case TSI:
		send0(FR_IN, FR_TSI_IN);
		break;
	case MPS:
		send0(FR_IN, FR_MPS_IN);
		break;
    case EOM:
		send0(FR_IN, FR_EOM_IN);
		break;
    case EOP:
		send0(FR_IN, FR_EOP_IN);
		break;
	default:
		send0(FR_IN, FR_UNKNOWN_IN);
		break;
	}

	return 0;
}
*/
state_service_func exp_ath_in(void *p)
{
//	export_1(&In,"ATH\r", (VFI)&export_1_tsk_in);
	send0(FR_IN,FR_OK_IN);
	return 0;
}

//===================
void export_1_tsk_out(int event)
{
	switch (event)
	{
	case FR_OK_OUT:
		TimeDebugSPrintf("FR_OK out");
		sub_ret(&Out);
		send0(FR_OUT,FR_OK_OUT);
		break;
	case FR_CONNECT_OUT:
		TimeDebugSPrintf("FR_CONNECT out");
		sub_ret(&Out);
		send0(FR_OUT,FR_CONNECT_OUT);
		break;
	case FR_NO_CARRIER_OUT:
		TimeDebugSPrintf("FR NO CARRIER out");
		sub_ret(&Out);
		send0(FR_OUT, FR_NO_CARRIER_OUT);
		break;
	case FR_ERROR_OUT:
		TimeDebugSPrintf("FR ERROR out");
		sub_ret(&Out);
		send0(FR_OUT, FR_ERROR_OUT);
		break;
	}
}

void atd_tsk_out(int event)
{
	switch (event)
	{
	case FR_OK_OUT:
		//TimeDebugSPrintf("FR_OK out");
		//sub_ret(&Out); // ?
		//send0(FR_OUT,FR_OK_OUT); // ?
		break;
	case FR_CONNECT_OUT:
		TimeDebugSPrintf("FR_CONNECT out");
		sub_ret(&Out);
		send0(FR_OUT,FR_CONNECT_OUT);
		send0(CC,AT_connect);
		// send0(SLAC,SLAC_connect);  Not necessary, CC sends SLAC_connect -- hakan  
		break;
	case FR_NO_CARRIER_OUT:
		TimeDebugSPrintf("FR NO CARRIER out");
		sub_ret(&Out);
		send0(FR_OUT, FR_NO_CARRIER_OUT);
		break;
	case FR_ERROR_OUT:
		TimeDebugSPrintf("FR ERROR out");
		sub_ret(&Out);
		send0(FR_OUT, FR_ERROR_OUT);
		break;
	}
}

state_service_func exp_fclass_out(void *p)
{
	clcc_check_enabled = 0;
	pas_check_enabled = 0;
	StopPeriodicCREGCheck(0);  // These two lines can be removed --hakan 
	delay(500);

	export_1(&Out, "AT+FCLASS=1\r", (VFI)&export_1_tsk_out);
//	TimeDebugSPrintf("AT+FCLASS=1 out");
	return 0;
}


state_service_func exp_atd_out(void *p)
{
	int i;

	frame_buf_in_index = 0;
	if(!dis_received)
		delayed_frame_buf_in_index = 0;
	frame_buf_out_index = 0;	
	frames_in_buf = 0;
	frame_byte_counter = 0;
	
	last_frame = 0;
	dis_frame = 0;
	
	memcpy(fr_phone_num,"ATD",3);
    for(i=0; i < DialingParams.ndx; i++)
    	fr_phone_num[i+3] = DialingParams.DialingParams[i];
    fr_phone_num[i+3]='\r';
    fr_phone_num[i+4]='\0';
    
	export_1(&Out,fr_phone_num, (VFI)&atd_tsk_out);
	TimeDebugSPrintf("ATD out");

	return 0;
}

state_service_func send_frs_in_connect(void *p)
{
	send0(FR_IN,FR_CONNECT_OUT);
	TimeDebugSPrintf("snd CON out to in");
	return 0;
}

state_service_func get_frame_out(void *p)
{
	frame_start = 0;
	frame_byte_counter = 0;
	frm_char[0] = 0;
	frm_char[1] = 0;
	nsf_frame = 0;
	dis_frame = 0;
	last_frame = 0;
	skip_frame_end = 0;
	
	if(!dis_received)
	{
		delayed_frame_buf_in_index = 0;
		frame_buf_in_index = 0;
		frame_buf_out_index = 0;
	}
	
	Out.i_stack_driver++;
	Out.in_flag[Out.i_stack_driver] = 1;
	Out.driver[Out.i_stack_driver] = (VFI)&get_frame_out_tsk;
	TimeDebugSPrintf("get frm from out");
	return 0;
}



void send_frame_out_tsk(int event)
{
	switch (event)
	{
		case FR_CONNECT_OUT:
			TimeDebugSPrintf("Frm CON send out");
			out_frame_connected = 1;
			break;
		case FR_PUMP_OUT:
			if(!out_frame_connected)
				break;
				
			while(out_frame_connected && (frame_buf_in_index) && (frame_buf_in_index > frame_buf_out_index))
			{	/* there is frame in buffer */
				if(!Out.char_out_non_block(frame_buf[frame_buf_out_index]))
				{
					break;
				}
				
				if((frame_buf[frame_buf_out_index] == ETX) 
					&& (frame_buf[frame_buf_out_index -1] == DLE))
				{
						out_frame_connected = 0;
						TimeDebugSPrintf("Frm DLE_ETX snd out");
				}
									
				Out.frame_out[frame_buf_out_index] = frame_buf[frame_buf_out_index];
				frame_buf_out_index++;
			}
	
			break;
		case FR_DLE_ETX_OUT:
			TimeDebugSPrintf("Frm DLE ETX snd out");
			out_frame_connected = 0;
			break;
		case FR_OK_OUT:
			TimeDebugSPrintf("Frm OK send out");
			out_frame_connected = 0;			
			sub_ret(&Out);
			Out.driver[Out.i_stack_driver](FR_OK_OUT);
			break;
		case FR_ERROR_OUT:                        
			sub_ret(&Out);
			out_frame_connected = 0;
			Out.driver[Out.i_stack_driver](FR_ERROR_OUT);
			break;
		case FR_NO_CARRIER_OUT:
			sub_ret(&Out);
			out_frame_connected = 0;
			Out.driver[Out.i_stack_driver](FR_NO_CARRIER_OUT);
			break;
	}
}

state_service_func send_frame_out(void *p)
{
	Out.i_stack_driver++;
	Out.in_flag[Out.i_stack_driver] = 0;
	Out.driver[Out.i_stack_driver] = (VFI)&send_frame_out_tsk;
	
	out_frame_connected = 1;
	
	return 0;
}

state_service_func exp_FTM_xx_out(void *p)
{
	char command[11] = {'A','T','+','F','T','M','=','0','0','\r',0};
	
	command[7] = xo[0];
	command[8] = xo[1];
	
	export_1(&Out,command, (VFI)&export_1_tsk_out);
	TimeDebugSPrintf("AT+FTMxx out");
	return 0;
}

state_service_func check_connect_out(void *p)   
{
	switch(fr_state_out)
	{
		case oo6:
		case oo23:
		case oo37:
			send0(FR_IN, FR_CONNECT_OUT);
			break;
		default:
			break;
	}
	
	return 0;
}

state_service_func check_DCS_DCN_out(void *p)
{
	switch (Out.frame_out[2] & 0xfe)
	{
		case DCS:
		case TSI:
			send0(FR_OUT, FR_DCS_IN);
			break;
		case DCN:
			send0(FR_OUT, FR_DCN_IN);
			break;
		default:
			send0(FR_OUT, FR_DEFAULT_IN);
			break;				
	}
	
	return 0;
}

state_service_func check_MPS_EOP_out(void *p)
{
	switch (Out.frame_out[2] & 0xfe)
	{
		case MPS:
			send0(FR_OUT, FR_MPS_IN);
			break;
		case EOP:
			send0(FR_OUT, FR_EOP_IN);
			break;
		default:
			send0(FR_OUT, FR_DEFAULT_IN);
			break;				
	}
	
	return 0;
}

state_service_func check_CFR_FTT_out(void *p)
{
	switch (Out.frame_in[2] & 0xfe)
	{
		case CFR:
			send0(FR_OUT, FR_CFR_OUT);
			break;
		case FTT:
			send0(FR_OUT, FR_FTT_OUT);
			break;
		default:
			send0(FR_OUT, FR_DEFAULT_OUT);
			break;				
	}
	
	return 0;
}

state_service_func check_MCF_RTN_RTP_out(void *p)
{
	switch (Out.frame_in[2] & 0xfe)
	{
		case MCF:
			send0(FR_OUT, FR_MCF_OUT);
			break;
		case RTN:
			send0(FR_OUT, FR_RTN_OUT);
			break;
		case RTP:
			send0(FR_OUT, FR_RTP_OUT);
			break;
		default:
			send0(FR_OUT, FR_DEFAULT_OUT);
			break;				
	}
	
	return 0;
}
/*  Not Used
state_service_func frm_decode_out(void *p)
{
	switch (Out.frame_in[2])
	{
	case DIS:
		send0(FR_OUT, FR_DIS_OUT);
		break;
	case CSI:
		send0(FR_OUT, FR_CSI_OUT);
		break;
	case NSF:
		send0(FR_OUT, FR_NSF_OUT);
		break;
	case CFR:
		send0(FR_OUT, FR_CFR_OUT);
		break;
	case MCF:
		send0(FR_OUT, FR_MCF_OUT);
		break;
	case RTN:
		send0(FR_OUT, FR_RTN_OUT);
		break;
	case RTP:
		send0(FR_OUT, FR_RTP_OUT);
		break;
	case FTT:
		send0(FR_OUT, FR_FTT_OUT);	
	default:
	    break;
	}
	return 0;
}
*/
state_service_func clear_frames_in(void *p)
{
	int i;
	
	for(i = 0; i < sizeof(In.frame_in); i++)
		In.frame_in[i] = 0;
	
	return 0;
}

timer_service_func time_in_funct(void *p)
{
	TimeDebugSPrintf("timeout_in");
	send0(FR_IN,FR_TIMEOUT_IN);
	return 0;
}




state_service_func timeout_in_create(void *p)
{
static int param;
	timer_add(&(timer_in_ID), 1,//one_shot
	   (timer_service_func)time_in_funct, 
	   (void *)param, 
	   100);

	return 0;
}

state_service_func timeout_in_delete(void *p)
{
	if(timer_in_ID != -1)
	{
		timer_delete(timer_in_ID);
		timer_in_ID = -1;
	}
	return 0;
}

state_service_func timeout_in_disable(void *p)
{
	timer_disable(timer_in_ID);
	TimeDebugSPrintf("tout in disabld");
	return 0;
}

state_service_func set_timeout_in(void *p)
{
	TimeDebugSPrintf("set_timeout_in");
	timer_enable( timer_in_ID, 250);
	return 0;
}

timer_service_func time_out_funct(void *p)
{
	TimeDebugSPrintf("timeout_out");
	send0(FR_OUT,FR_TIMEOUT_OUT);
	return 0;
}


state_service_func timeout_out_create(void *p)
{
static int param;
	timer_add(&(timer_out_ID), 1,//one_shot
	   (timer_service_func)time_out_funct, 
	   (void *)param, 
	   100);
	return 0;
}

state_service_func timeout_out_delete(void *p)
{
	if(timer_out_ID != -1)
	{
		timer_delete(timer_out_ID);
		timer_out_ID = -1;
	}
	return 0;
}

state_service_func timeout_out_disable(void *p)
{
	TimeDebugSPrintf("timeout_disabl");
	timer_disable(timer_out_ID);
	return 0;
}

state_service_func set_timeout_out(void *p)
{
	TimeDebugSPrintf("set_timeout_out");
	timer_enable( timer_out_ID, 250);
	return 0;
}


int last_out;

long count_b_fputc = 0;
long count_b_fputc_succ = 0;
long count_b_fputc_usucc = 0;

int max_page_acc;
long cts_low_counter;  // Not used!


void page_pump_out(int event)
{
	int simb;
	switch (event)
	{
	case FR_PUMP_OUT:
		if((!pump_page) && (!pump_page_zeros))
		{
			break;
		}
		else if (pump_page_zeros)
		{
			if(Out.char_out_non_block(0x00))
			{
				leading_zeros++;
			}
			return;
		}
		
		
		if(page_char_count_in - page_char_count_out > max_page_acc)
			max_page_acc = page_char_count_in - page_char_count_out;
		if(page_char_count_in < page_start_trigger)
		{
			if(Out.char_out_non_block(0x00));
			return;
		}
			
		while((page_char_count_in > page_char_count_out) ||(!page_recv_flag))
		//if(page_char_count_in > page_char_count_out)
		{
			simb = get_from_buf(page_out_ptr);
			count_b_fputc++;
			if(Out.char_out_non_block(simb))
			{
				count_b_fputc_succ++;
				page_out_ptr++;
				page_char_count_out++;

				if(page_out_ptr == PAGE_BUFFER_LEN)
					page_out_ptr = 0;
					
				if((last_out == DLE) && (simb == DLE)) 
					simb = 0x0;
				if((last_out == DLE) && (simb == ETX))
				{
					pump_page = 0;
					return;
				}
				last_out = simb;
			}
			else
			{
				count_b_fputc_usucc++;
				if(!(UART_A_MSR_REG & CTS))
					cts_low_counter++;
				break;
			}
		}
		
		break;
	case FR_ERROR_OUT:
		pump_page = 0;
		TimeDebugSPrintf("page ERROR");

		sub_ret(&Out);
		Out.driver[Out.i_stack_driver](FR_ERROR_OUT);
		
		break;
	case FR_NO_CARRIER_OUT:
		pump_page = 0;
		TimeDebugSPrintf("page SENT-NOCAR");
		sub_ret(&Out);
		Out.driver[Out.i_stack_driver](FR_OK_OUT);
		
		break;
	case FR_OK_OUT:
		pump_page = 0;
		TimeDebugSPrintf("page SENT-OK");
		sub_ret(&Out);
		Out.driver[Out.i_stack_driver](FR_OK_OUT);
		
		break;
	default:
//		pump_page = 0;
//		sub_ret(&Out);
//		Out.driver[Out.i_stack_driver](FR_OK_OUT);
		
		break;
	}
}

state_service_func pump_leading_zeros(void *p)
{
	int	i;
	
	Out.i_stack_driver++;
	Out.in_flag[Out.i_stack_driver] = 0;
	Out.driver[Out.i_stack_driver] = (VFI)&page_pump_out;
	TimeDebugSPrintf("Strt_zeros_out");

	// Debug counters
	count_b_fputc = 0;
	count_b_fputc_succ = 0;
	count_b_fputc_usucc = 0;
	
	max_page_acc = 0;

	// signal pump
	pump_page_zeros = 1;
	for(i = 0; i < 10; i++)
		page_pump_out(FR_PUMP_OUT);
	zeros_pump_period = 15 * 96 / fax_speed;
	leading_zeros = 0;
	pump_page = 0;
	in_frame_connected = 0;
	out_frame_connected = 0;

	return 0;
}

state_service_func page_pump(void *p)
{
	if(!pump_page_zeros)
	{
		Out.i_stack_driver++;
		Out.in_flag[Out.i_stack_driver] = 0;
		Out.driver[Out.i_stack_driver] = (VFI)&page_pump_out;
	}
	// Debug counters
	count_b_fputc = 0;
	count_b_fputc_succ = 0;
	count_b_fputc_usucc = 0;
	
	max_page_acc = 0;

	// signal pump
	pump_page = 1;
	pump_page_zeros = 0;
	zeros_pump_period = 0;
	in_frame_connected = 0;
	out_frame_connected = 0;

	return 0;
}


state_service_func exp_FTS_8_out(void *p)
{
//	TimeDebugSPrintf("AT+FTS=8\r");
	export_1(&Out,"AT+FTS=8\r", (VFI)&export_1_tsk_out);
	return 0;
}

state_service_func exp_FRS_8_out(void *p)
{
//	TimeDebugSPrintf("AT+FRS=8\r");
	export_1(&Out,"AT+FRS=8\r", (VFI)&export_1_tsk_out);
	return 0;
}

state_service_func exp_FTH_3_out(void *p)
{
//	TimeDebugSPrintf("AT+FTH=3\r");
	export_1(&Out,"AT+FTH=3\r", (VFI)&export_1_tsk_out);
	return 0;
}

state_service_func exp_FRH_3_out(void *p)
{
	export_1(&Out,"AT+FRH=3\r", (VFI)&export_1_tsk_out);
//	TimeDebugSPrintf("AT+FRH=3\r");
	return 0;
}


state_service_func exp_at_out(void *p)
{
	export_1(&Out, "AT\r", (VFI)&export_1_tsk_out);
	return 0;
}

state_service_func clear_frames_out(void *p)
{
	int i;
	
	for(i = 0; i < sizeof(Out.frame_in); i++)
		Out.frame_in[i] = 0;
	
	return 0;
}


state_service_func send_to_cc_disconnect(void *p)
{
    send0_delayed(500,CC,AT_disconnect);
    send0(FR,FR_stop);
    TimeDebugSPrintf("StopDisconnect");
	return 0;
}

state_service_func exp_ath_out(void *p)
{
	if(fr_stop_flag)
	{
   	  	send_at_command("+++ATH\r",100);
		send_at_command("ATE0V0;+WIND=255\r",200);
		TimeDebugSPrintf("ath_out");
		send0_delayed(400,FR,FR_finished);
    	fr_task_on = 0;    
    	fr_state_in = FR_STATE_SLEEP;
	}
	send0(FR_OUT, FR_OK_OUT);
//	TimeDebugSPrintf("AT+FCLASS=1 out");
	return 0;
}

/* Not Used
state_service_func exp_empty_TSI_out(void *p)
{
	int i;
	
	Out.i_stack_driver++;
	Out.in_flag[Out.i_stack_driver] = 0;
	Out.driver[Out.i_stack_driver] = (VFI)&export_1_tsk_out;
	
	Out.char_out(0xFF);
	Out.char_out(0x03);
	Out.char_out(TSI | 0x1);
	for(i = 0; i < 20; i++)
		Out.char_out(0x20);
	Out.char_out(DLE);
	Out.char_out(ETX);
	
	TimeDebugSPrintf("Empty TSI sent");
	
	return 0;
}
*/

state_service_func send_disconnect(void *p)
{
	send0_delayed(100,CC, AT_error); // --hakan 
   	// send0(FR,FR_stop);  // -- hakan CC will handle the call release.
    
    TimeDebugSPrintf("snd disconn");
	return 0;
}

state_service_func start_ringback(void *p)
{
	if(fr_direction == FR_outgoing)
		send0(SLAC, SLAC_ring_back);
	return 0;
}

state_service_func stop_ringback(void *p)
{
	if(fr_direction == FR_outgoing)
	{
	//	send0(CC, AT_connect);  // already sent! --hakan
		send0(SLAC, SLAC_connect);
	}
	return 0;
}

state_service_func exp_DLE_ETX_out(void *p)
{
	Out.char_out(0xFF);
	Out.char_out(0x13);
	Out.char_out(CRP|0x1);
	Out.char_out(DLE);
	Out.char_out(ETX);

	Out.i_stack_driver++;
	Out.in_flag[Out.i_stack_driver] = 0;
	Out.driver[Out.i_stack_driver] = (VFI)&export_1_tsk_out;
	return 0;
}


void fax_pump(void)
{		
	// This routine is called from the main loop once per iteration
	int i;
	
//	TimeDebugSPrintf("fax_pump");
	if(pump_page_zeros)
	{
		if(!zeros_pump_period)
		{
			//TimeDebugSPrintf("pump_zero");
			for(i = 0; i < 9; i++)
				Out.driver[Out.i_stack_driver](FR_PUMP_OUT);
			zeros_pump_period = 15 * 96 / fax_speed;
		}
	}
	if(in_frame_connected)
		In.driver[In.i_stack_driver](FR_PUMP_IN);
	else if(out_frame_connected)
		Out.driver[Out.i_stack_driver](FR_PUMP_OUT);
	else if(pump_page)
		Out.driver[Out.i_stack_driver](FR_PUMP_OUT);
}


const STATE_CONTEXT fr_state_table_in[]=
{
	{
		FR_STATE_SLEEP, 	/* 0 */
		{0,0,0,0,0},
		{0,0,0,0,0},
		{FR_FAX_CALL,0,0,0,0,0,0,0},
		{ii2,0,0,0,0,0,0,0}
	},
	{
		ii1, 	/* 1 */
		{send_disconnect,timeout_in_delete,timeout_out_delete,0,0},
		{0,0,0,0,0},
		{FR_OK_IN,FR_ERROR_IN,FR_NO_CARRIER_IN,0,0,0,0,0},
		{FR_STATE_SLEEP,ii1,ii1,0,0,0,0,0}
	},
	{
		ii2, 	/* 2 */
		{start_frs_out,exp_fclass_in,timeout_in_create,0,0},
		{0,0,0,0,0},
		{FR_OK_IN,FR_ERROR_IN,0,0,0,0,0,0},
		{ii3,ii1,0,0,0,0,0,0}
	},
	{
		ii3, 	/* 3 */
		{check_connect_out,0,0,0,0},
		{0,0,0,0,0},
		{FR_CONNECT_OUT,FR_ERROR_IN,0,0,0,0,0,0},
		{ii5,ii1,0,0,0,0,0,0}
	},
	{
		ii4, 	/* 4 */
		{0,0,0,0,0},
		{0,0,0,0,0},
		{FR_CONNECT_OUT,FR_ERROR_IN,0,0,0,0,0,0},
		{ii7,ii1,0,0,0,0,0,0}
	},
	{
		ii5, 	/* 5 */
		{check_DIS_received,0,0,0,0},
		{0,0,0,0,0},
		{FR_CONNECT_OUT,FR_DIS_IN,FR_ERROR_IN,0,0,0,0,0},
		{ii5,ii6,ii1,0,0,0,0}
	},
	{
		ii6, 	/* answer the call */
		{exp_ATA_in,0,0,0,0},
		{0,0,0,0,0},
		{FR_CONNECT_IN,FR_ERROR_IN,FR_BCS_START_OUT,0,0,0,0,0},
		{ii8,ii1,ii9,0,0,0,0,0}
	},
	{
		ii7, 	/* start sending NSF CSI DIS  */
		{exp_FTH_3_in,0,0,0,0},
		{0,0,0,0,0},
		{FR_CONNECT_IN,FR_ERROR_IN,FR_BCS_START_OUT,0,0,0,0,0},
		{ii8,ii1,ii9,0,0,0,0,0}
	},
	{
		ii8, 	/* wait for connect out */
		{0,0,0,0,0},
		{0,0,0,0,0},
		{FR_BCS_START_OUT,FR_ERROR_IN,0,0,0,0,0,0},
		{ii10,ii1,0,0,0,0,0,0}
	},
	{
		ii9, 	/* wait for connect in */
		{0,0,0,0,0},
		{0,0,0,0,0},
		{FR_CONNECT_IN,FR_ERROR_IN,0,0,0,0,0,0},
		{ii10,ii1,0,0,0,0,0,0}
	},
	{
		ii10, 	/* send NSF CSI DIS */
		{send_frame_in,0,0,0,0},
		{0,0,0,0,0},
		{FR_OK_IN,FR_ERROR_IN,0,0,0,0,0,0},
		{ii11,ii1,0,0,0,0,0,0}
	},
	{
		ii11, 	/* prepare to receive frames */
		{exp_FRS_8_in,0,0,0,0},
		{0,0,0,0,0},
		{FR_OK_IN,FR_ERROR_IN,0,0,0,0,0,0},
		{ii12,ii1,0,0,0,0,0,0}
	},
	{
		ii12, 	/* start receiving frames */
		{exp_FRH_3_in,set_timeout_in,clear_frames_in,0,0},
		{0,0,0,0,0},
		{FR_CONNECT_IN,FR_ERROR_IN,FR_NO_CARRIER_IN,FR_TIMEOUT_IN,0,0,0,0},
		{ii14,ii1,ii12,ii13,0,0,0,0}
	},
	{
		ii13, 	/* get modem attention */
		{exp_AT_in,0,0,0,0},
		{0,0,0,0,0},
		{FR_OK_IN,FR_ERROR_IN,FR_NO_CARRIER_IN,0,0,0,0,0},
		{ii4,ii1,ii1,0,0,0,0,0}
	},

	{
		ii14, 	/* TSI DCS connected */
		{send_frs_out_connect,timeout_in_disable,get_frame_in,0,0},
		{0,0,0,0,0},
		{FR_OK_IN,FR_ERROR_IN,FR_NO_CARRIER_IN,0,0,0,0,0},
		{ii15,ii1,ii1,0,0,0,0,0}
	},
	{
		ii15, 	/* check if DCS has been received */
		{check_DCS_DCN_in,0,0,0,0},
		{0,0,0,0,0},
		{FR_DCS_IN,FR_DEFAULT_IN,FR_DCN_IN,FR_ERROR_IN,0,0,0,0},
		{ii17,ii4,ii38,ii1,0,0,0,0}
	},
	{
		ii16, 
		{0,0,0,0,0},
		{0,0,0,0,0},
		{0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0}
	},
	{
		ii17, 	/* start getting training */
		{set_speed_from_DCS,exp_FRM_xx_in,0,0,0},
		{0,0,0,0,0},
		{FR_CONNECT_IN,FR_ERROR_IN,FR_NO_CARRIER_IN,0,0,0,0,0},
		{ii18,ii1,ii17,0,0,0,0,0}
	},
	{
		ii18, 	/* receive training sequence */
		{send_frs_out_connect,get_page_into_buf,0,0,0},
		{0,0,0,0,0},
		{FR_NO_CARRIER_IN,FR_OK_IN,FR_ERROR_IN,0,0,0,0,0},
		{ii19,ii19,ii1,0,0,0,0,0}
	},
	{
		ii19, 	/* prepare to send the frame */
		{exp_FTS_8_in,0,0,0,0},
		{0,0,0,0,0},
		{FR_OK_IN,FR_ERROR_IN,0,0,0,0,0,0},
		{ii20,ii1,0,0,0,0,0,0}
	},
	{
		ii20, 	/* start sending frame */
		{exp_FTH_3_in,check_connect_out,0,0,0},
		{0,0,0,0,0},
		{FR_CONNECT_IN,FR_ERROR_IN,FR_CONNECT_OUT,0,0,0,0,0},
		{ii21,ii1,ii22,0,0,0,0,0}
	},
	{
		ii21, 	/* wait for connect out */
		{0,0,0,0,0},
		{0,0,0,0,0},
		{FR_CONNECT_OUT,FR_ERROR_IN,0,0,0,0,0,0},
		{ii23,ii1,0,0,0,0,0,0}
	},
	{
		ii22, 	/* wait for connect in */
		{0,0,0,0,0},
		{0,0,0,0,0},
		{FR_CONNECT_IN,FR_ERROR_IN,0,0,0,0,0,0},
		{ii23,ii1,0,0,0,0,0,0}
	},
	{
		ii23, 	/* send frame in */
		{send_frame_in,0,0,0,0},
		{0,0,0,0,0},
		{FR_OK_IN,FR_ERROR_IN,0,0,0,0,0,0},
		{ii24,ii1,0,0,0,0,0,0}
	},
	{
		ii24, 	/* check CFR */
		{check_CFR_FTT_in,fifo_reset,0,0,0},
		{0,0,0,0,0},
		{FR_CFR_OUT,FR_DEFAULT_OUT,FR_FTT_OUT,FR_ERROR_IN,0,0,0,0},
		{ii26,ii11,ii11,ii1,0,0,0,0}
	},
	{
		ii25, 	
		{0,0,0,0,0},
		{0,0,0,0,0},
		{0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0}
	},
	{
		ii26, 	/* start receiving page */
		{exp_FRM_xx_in,0,0,0,0},
		{0,0,0,0,0},
		{FR_CONNECT_IN,FR_ERROR_IN,FR_NO_CARRIER_IN,0,0,0,0,0},
		{ii27,ii1,ii26,0,0,0,0,0}
	},
	{
		ii27, 	/* start receiving page */
		{send_frs_out_connect,get_page_into_buf,0,0,0},
		{0,0,0,0,0},
		{FR_OK_IN,FR_NO_CARRIER_IN,FR_ERROR_IN,0,0,0,0,0},
		{ii28,ii28,ii1,0,0,0,0,0}
	},
	{
		ii28, 	/* prepare to receive frame */
		{exp_FRS_8_in,0,0,0,0},
		{0,0,0,0,0},
		{FR_OK_IN,FR_ERROR_IN,0,0,0,0,0,0},
		{ii29,ii1,0,0,0,0,0,0}
	},
	{
		ii29, 	/* start receiving frame */
		{exp_FRH_3_in,clear_frames_in,0,0,0},
		{0,0,0,0,0},
		{FR_CONNECT_IN,FR_ERROR_IN,FR_NO_CARRIER_IN,0,0,0,0,0},
		{ii30,ii1,ii29,0,0,0,0,0}
	},
	{
		ii30, 	/* start receiving frame */
		{send_frs_out_connect,get_frame_in,0,0,0},
		{0,0,0,0,0},
		{FR_OK_IN,FR_ERROR_IN,0,0,0,0,0,0},
		{ii31,ii1,0,0,0,0,0,0}
	},	
	{
		ii31, 	/* prepare to send frame */
		{exp_FTS_8_in,0,0,0,0},
		{0,0,0,0,0},
		{FR_OK_IN,FR_ERROR_IN,0,0,0,0,0,0},
		{ii32,ii1,0,0,0,0,0,0}
	},
	{
		ii32, 	/* start sending frame */
		{exp_FTH_3_in,check_connect_out,0,0,0},
		{0,0,0,0,0},
		{FR_CONNECT_IN,FR_ERROR_IN,FR_CONNECT_OUT,0,0,0,0,0},
		{ii33,ii1,ii34,0,0,0,0,0}
	},
	{
		ii33, 	/* wait for connect out */
		{0,0,0,0,0},
		{0,0,0,0,0},
		{FR_CONNECT_OUT,FR_ERROR_IN,0,0,0,0,0,0},
		{ii35,ii1,0,0,0,0,0,0}
	},
	{
		ii34, 	/* wait for connect in */
		{0,0,0,0,0},
		{0,0,0,0,0},
		{FR_CONNECT_IN,FR_ERROR_IN,0,0,0,0,0,0},
		{ii35,ii1,0,0,0,0,0,0}
	},
	{
		ii35, 	/* send frame in */
		{send_frame_in,0,0,0,0},
		{0,0,0,0,0},
		{FR_OK_IN,FR_ERROR_IN,0,0,0,0,0,0},
		{ii36,ii1,0,0,0,0,0,0}
	},
	{
		ii36, 	/* check confirmation in */
		{check_MCF_RTN_RTP_in,0,0,0,0},
		{0,0,0,0,0},
		{FR_MCF_OUT,FR_RTN_OUT,FR_RTP_OUT,FR_DEFAULT_IN,FR_ERROR_IN,0,0,0}, 
		{ii37,ii11,ii11,ii11,ii1,0,0,0}
	},
	{
		ii37, 	/* check end of procedures */
		{check_MPS_EOP_in,0,0,0,0},
		{0,0,0,0,0},
		{FR_MPS_IN,FR_EOP_IN,FR_DEFAULT_IN,FR_ERROR_IN,0,0,0,0},
		{ii26,ii11,ii11,ii1,0,0,0,0}
	},
	{
		ii38, 	/* DCN has been received */
		{exp_ath_in,timeout_in_delete,0,0,0},
		{0,0,0,0,0},
		{FR_OK_IN,FR_ERROR_IN,0,0,0,0,0,0},
		{ii1,ii1,0,0,0,0,0,0}
	}
};

#define FR_MAX_STATES_IN (sizeof(fr_state_table_in)/sizeof(fr_state_table_in[0]))

const STATE_CONTEXT fr_state_table_out[]=
{
	{
		o0, 	/* 0 */
		{0,0,0,0,0},
		{0,0,0,0,0},
		{FR_FRS_OUT_START,FR_ERROR_OUT,0,0,0,0,0,0},
		{oo2,oo1,0,0,0,0,0,0}
	},
	{
		oo1, 	/* 1 */
		{send_disconnect,timeout_out_delete,timeout_in_delete,0,0},
		{0,0,0,0,0},
		{FR_ERROR_OUT,0,0,0,0,0,0},  // Do we need to handle other conditions?
		{oo1,0,0,0,0,0,0}
	},
	{
		oo2, 	/* 2 */
		{exp_fclass_out,timeout_out_create,0,0,0},
		{0,0,0,0,0},
		{FR_OK_OUT,FR_ERROR_OUT,0,0,0,0,0,0},
		{oo3,oo1,0,0,0,0,0,0}
	},
	{
		oo3, 	/* 3 */
		{exp_atd_out,0,0,0,0},
		{0,0,0,0,0},
		{FR_RINGBACK,FR_BUSY,FR_ERROR_OUT,FR_CONNECT_OUT,FR_NO_CARRIER_OUT,0,0,0},
		{oo4,oo1,oo1,oo6,oo1,0,0,0}
	},
	{
		oo4, 	/* 4 */
		{start_ringback,0,0,0,0},
		{0,0,0,0,0},
		{FR_RINGBACK,FR_BUSY,FR_ERROR_OUT,FR_CONNECT_OUT,FR_NO_CARRIER_OUT,0,0},
		{oo4,oo1,oo1,oo6,oo1,0,0}
	},
	{
		oo5, 	/* 5 */					
		{0,0,0,0,0},
		{0,0,0,0,0},
		{0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0}
	},
	{
 		oo6, 	/* inform IN machine of connection, get frames */
		{stop_ringback,send_frs_in_connect,get_frame_out,0,0},
		{0,0,0,0,0},
		{FR_OK_OUT,FR_NO_CARRIER_OUT,FR_ERROR_OUT,0,0,0,0,0},
		{oo7,oo1,oo1,0,0,0,0,0}
	},
	{
		oo7, 	/* prepare to send TSI DCS  */
		{exp_FTS_8_out,0,0,0,0},
		{0,0,0,0,0},
		{FR_OK_OUT,FR_ERROR_OUT,0,0,0,0,0,0},
		{oo8,oo1,0,0,0,0,0,0}
	},
	{
		oo8, 	/* start sending TSI DCS  */
		{set_timeout_out,check_connect_in,exp_FTH_3_out,0,0},
		{0,0,0,0,0},
		{FR_ERROR_OUT,FR_CONNECT_IN,FR_TIMEOUT_OUT,FR_CONNECT_OUT,0,0,0,0}, 
		{oo1,oo9,oo11,oo10,0,0,0,0}
	},
	{
		oo9, 	/* TSI DCS connected in  */
		{timeout_out_disable,0,0,0,0},
		{0,0,0,0,0},
		{FR_CONNECT_OUT,FR_ERROR_OUT,0,0,0,0,0,0},
		{oo14,oo1,0,0,0,0,0,0}
	},
	{
		oo10, 	/* wait for connect IN  */
		{check_connect_in,0,0,0,0},
		{0,0,0,0,0},
		{FR_ERROR_OUT,FR_CONNECT_IN,FR_TIMEOUT_OUT,0,0,0,0,0},
		{oo1,oo14,oo11,0,0,0,0,0}
	},
	{
		oo11, 	/* send empty  */
		{exp_DLE_ETX_out,0,0,0,0},
		{0,0,0,0,0},
		{FR_OK_OUT,FR_ERROR_OUT,0,0,0,0,0,0},
		{oo12,oo1,0,0,0,0,0,0}
	},
	{
		oo12, 	/* get NSF CSI DIS again */
		{exp_FRS_8_out,0,0,0,0},
		{0,0,0,0,0},
		{FR_OK_OUT,FR_ERROR_OUT,0,0,0,0,0,0},
		{oo13,oo1,0,0,0,0,0,0}
	},
	{
		oo13, 	/* get NSF CSI DIS again */
		{exp_FRH_3_out,clear_frames_out,0,0,0},
		{0,0,0,0,0},
		{FR_CONNECT_OUT,FR_ERROR_OUT,0,0,0,0,0,0},
		{oo6,oo1,0,0,0,0,0,0}
	},
	{
		oo14, 	/* pump frames out */
		{send_frame_out,timeout_out_disable,0,0,0}, 
		{0,0,0,0,0},
		{FR_OK_OUT,FR_ERROR_OUT,0,0,0,0,0,0},
		{oo15,oo1,0,0,0,0,0,0}
	},
	{
		oo15, 	/* check if DCS has been transmitted  */
		{check_DCS_DCN_out,0/*get_speed_from_DCS*/,0,0,0},
		{0,0,0,0,0},
		{FR_DCS_IN,FR_DEFAULT_IN,FR_DCN_IN,FR_ERROR_OUT,0,0,0,0},
		{oo16,oo12,oo40,oo1,0,0,0,0}
	},
	{
		oo16, 	/* prepare to send training  */
		{exp_FTS_8_out,0,0,0,0},
		{0,0,0,0,0},
		{FR_OK_OUT,FR_ERROR_OUT,0,0,0,0,0,0},
		{oo17,oo1,0,0,0,0,0,0}
	},
	{
		oo17, 	/* start sending training */
		{exp_FTM_xx_out,check_connect_in,0,0,0},
		{0,0,0,0,0},
		{FR_CONNECT_OUT,FR_ERROR_OUT,FR_CONNECT_IN,0,0,0,0,0},
		{oo18,oo1,oo19,0,0,0,0,0}
	},
	{
		oo18, 	/* wait for connect in */
		{check_connect_in,0,0,0,0},
		{0,0,0,0,0},
		{FR_CONNECT_IN,FR_ERROR_OUT,0,0,0,0,0,0},
		{oo20,oo1,0,0,0,0,0,0}
	},
	{
		oo19, 	/* wait for connect out  */
		{0,0,0,0,0},
		{0,0,0,0,0},
		{FR_CONNECT_OUT,FR_ERROR_OUT,0,0,0,0,0,0},
		{oo20,oo1,0,0,0,0,0,0}
	},
	{
		oo20, 	/* pump training out */
		{page_pump,0,0,0,0},
		{0,0,0,0,0},
		{FR_OK_OUT,FR_ERROR_OUT,0,0,0,0,0,0},
		{oo22,oo1,0,0,0,0,0,0}
	},
	{
		oo21, 	 
		{0,0,0,0,0},
		{0,0,0,0,0},
		{0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0}
	},
	{
		oo22, 	/* prepare to get confirmation  */
		{exp_FRH_3_out,clear_frames_out,0,0,0},
		{0,0,0,0,0},
		{FR_CONNECT_OUT,FR_ERROR_OUT,0,0,0,0,0,0},
		{oo23,oo1,0,0,0,0,0,0}
	},
	{
		oo23, 	/* inform IN of connect, get frames  */
		{send_frs_in_connect,get_frame_out,0,0,0},
		{0,0,0,0,0},
		{FR_OK_OUT,FR_ERROR_OUT,0,0,0,0,0,0},
		{oo24,oo1,0,0,0,0,0,0}
	},
	{
		oo24, 	/* decide to pump frames or pages  */
		{check_CFR_FTT_out,0,0,0,0},
		{0,0,0,0,0},
		{FR_CFR_OUT,FR_FTT_OUT,FR_DEFAULT_OUT,FR_ERROR_OUT,0,0,0,0},
		{oo25,oo7,oo7,oo1,0,0,0,0}
	},
	{
		oo25, 	/* prepare to send pages  */
		{exp_FTS_8_out,0,0,0,0},
		{0,0,0,0,0},
		{FR_OK_OUT,FR_ERROR_OUT,0,0,0,0,0,0},
		{oo26,oo1,0,0,0,0,0,0}
	},
	{
		oo26, 	/* start sending page */
		{exp_FTM_xx_out,check_connect_in,0,0,0},
		{0,0,0,0,0},
		{FR_CONNECT_OUT,FR_ERROR_OUT,FR_CONNECT_IN,0,0,0,0,0},
		{oo27,oo1,oo28,0,0,0,0,0}
	},
	{
		oo27, 	/* wait for connect in */
		{pump_leading_zeros,0,0,0,0},
		{0,0,0,0,0},
		{FR_CONNECT_IN,FR_ERROR_OUT,0,0,0,0,0,0},
		{oo29,oo1,0,0,0,0,0,0}
	},
	{
		oo28, 	/* wait for connect out  */
		{0,0,0,0,0},
		{0,0,0,0,0},
		{FR_CONNECT_OUT,FR_ERROR_OUT,0,0,0,0,0,0},
		{oo29,oo34,0,0,0,0,0,0}
	},
	{
		oo29, 	/* pump page  */
		{page_pump,0,0,0,0},
		{0,0,0,0,0},
		{FR_OK_OUT,FR_ERROR_OUT,0,0,0,0,0,0},
		{oo30,oo1,0,0,0,0,0,0}
	},
	{
		oo30, 	/* prepare to send MPS, EOP  */
		{exp_FTS_8_out,fifo_reset,0,0,0},
		{0,0,0,0,0},
		{FR_OK_OUT,FR_ERROR_OUT,0,0,0,0,0,0},
		{oo31,oo1,0,0,0,0,0,0}
	},
	{
		oo31, 	/* start sending MPS, EOP  */
		{exp_FTH_3_out,check_connect_in,0,0,0},
		{0,0,0,0,0},
		{FR_CONNECT_OUT,FR_ERROR_OUT,FR_CONNECT_IN,0,0,0,0,0},
		{oo32,oo1,oo33,0,0,0,0,0}
	},
	{
		oo32, 	/* wait for connect in  */
		{0,0,0,0,0},
		{0,0,0,0,0},
		{FR_CONNECT_IN,FR_ERROR_OUT,0,0,0,0,0,0},
		{oo34,oo1,0,0,0,0,0,0}
	},
	{
		oo33, 	/* wait for connect out */
		{0,0,0,0,0},
		{0,0,0,0,0},
		{FR_CONNECT_OUT,FR_ERROR_OUT,0,0,0,0,0,0},
		{oo34,oo1,0,0,0,0,0,0}
	},
	{
		oo34, 	/* pump frame  */
		{send_frame_out,0,0,0,0},
		{0,0,0,0,0},
		{FR_OK_OUT,FR_ERROR_OUT,0,0,0,0,0,0},
		{oo35,oo1,0,0,0,0,0,0}
	},
	{
		oo35, 	/* prepare to get frame */
		{exp_FRS_8_out,0,0,0,0},
		{0,0,0,0,0},
		{FR_OK_OUT,FR_ERROR_OUT,0,0,0,0,0,0},
		{oo36,oo1,0,0,0,0,0,0}
	},	
	{
		oo36, 	/* start receiving frame */
		{exp_FRH_3_out,clear_frames_out,0,0,0},
		{0,0,0,0,0},
		{FR_CONNECT_OUT,FR_ERROR_OUT,0,0,0,0,0,0},
		{oo37,oo1,0,0,0,0,0,0}
	},
	{
		oo37, 	/* inform IN of connect, get frame */
		{send_frs_in_connect,get_frame_out,0,0,0}, 
		{0,0,0,0,0},
		{FR_OK_OUT,FR_ERROR_OUT,0,0,0,0,0,0},
		{oo38,oo1,0,0,0,0,0,0}
	},
	{
		oo38, 	/* check for confirmation */
		{check_MCF_RTN_RTP_out,0,0,0,0}, 
		{0,0,0,0,0},
		{FR_MCF_OUT,FR_RTN_OUT,FR_RTP_OUT,FR_DEFAULT_OUT,0,0,0,0},
		{oo39,oo7,oo7,oo7,0,0,0,0}
	},
	{
		oo39, 	/* check for end of procedures */
		{check_MPS_EOP_out,0,0,0,0},
		{0,0,0,0,0},
		{FR_MPS_IN,FR_EOP_IN,FR_ERROR_OUT,0,0,0,0,0},
		{oo25,oo7,oo1,0,0,0,0,0}
	},
	{
		oo40, 	/* DCN has been sent */
		{send_to_cc_disconnect,exp_ath_out,timeout_out_delete,0,0},
		{0,0,0,0,0},
		{FR_OK_OUT,FR_ERROR_OUT,0,0,0,0,0,0},
		{o0,oo1,0,0,0,0,0,0}
	}
};

#define FR_MAX_STATES_OUT (sizeof(fr_state_table_out)/sizeof(fr_state_table_out[0]))


int fr_stm_in(int event)
{
   	int index,trnind,j;
   	int prev_state;
   	char string[20];
    
    prev_state = fr_state_in;
    
   	for(index=0;index<FR_MAX_STATES_IN && fr_state_table_in[index].state != fr_state_in;index++);
   	if(index>=FR_MAX_STATES_IN)
    {
   	
		return -1; //no entry in the table.
    }

	for(trnind=0;trnind<FR_MAX_TRANSITIONS;trnind++)
		if(fr_state_table_in[index].transition[trnind] == event)
		   break;
	if(trnind>=FR_MAX_TRANSITIONS)
	{   
		return 0; //no transitions.
	}
    //process state transition.
	
	fr_state_in = fr_state_table_in[index].next_state[trnind];
//	if(fr_state_in == ii1)
//	{
		sprintf(string, "i:%d->%d evt:%d", prev_state,fr_state_in, event);
		TimeDebugSPrintf(string);
//	}

	
	//find the next table entry.
   	for(index=0;index<FR_MAX_STATES_IN && fr_state_table_in[index].state != fr_state_in;index++);
    if(index>=FR_MAX_STATES_IN)
   	{	
   		return -1; //no entry in the table.
    }
	//perform the new state functions
	for(j=0;j<FR_MAX_FUNCTIONS && fr_state_table_in[index].service_func[j];j++)
		fr_state_table_in[index].service_func[j]((void *)0);

	return 1;			 
}



int fr_stm_out(int event)
{
   	int index,trnind,j;
   	int prev_state;
    char string[20];
    
    prev_state = fr_state_out;

   	for(index=0;index<FR_MAX_STATES_OUT && fr_state_table_out[index].state != fr_state_out;index++);
   	if(index>=FR_MAX_STATES_OUT)
    {
   	
		return -1; //no entry in the table.
    }

	for(trnind=0;trnind<FR_MAX_TRANSITIONS;trnind++)
		if(fr_state_table_out[index].transition[trnind] == event)
		   break;
	if(trnind>=FR_MAX_TRANSITIONS)
	{   
		return 0; //no transitions.
	}
    //process state transition.
	
	fr_state_out = fr_state_table_out[index].next_state[trnind];
//	if(fr_state_out == oo1)
//	{
		sprintf(string, "o:%d->%d evt:%d", prev_state,fr_state_out, event);
		TimeDebugSPrintf(string);
//	}
	
	//find the next table entry.
   	for(index=0;index<FR_MAX_STATES_OUT && fr_state_table_out[index].state != fr_state_out;index++);
    if(index>=FR_MAX_STATES_OUT)
   	{	
   		return -1; //no entry in the table.
    }
	//perform the new state functions
	for(j=0;j<FR_MAX_FUNCTIONS && fr_state_table_out[index].service_func[j];j++)
		fr_state_table_out[index].service_func[j]((void *)0);

	return 1;			 
}

