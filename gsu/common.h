/****************************************************************************/
/* File: "common.h" 														*/
/* Date: 11-14-96															*/
/* Author: Peter B. Miller													*/
/* Company: MESi															*/
/*	    10909 Lamplighter Lane, Potomac, MD 20854							*/
/* Phone: (301) 765-9668													*/
/*	E-mail: peter.miller@mesi.net											*/
/* Website: www.mesi.net													*/
/* Description: memory and common definitions for common modules.			*/
/****************************************************************************/

#ifndef COMMON_INCLUSION_
#define COMMON_INCLUSION_

	/**** APSK modulator common structure ****/

#define TX_APSK_MOD_BLOCK \
	int *fir_head;	\
	int *fir_tail;	\
	int fir_len; \
	int fir_taps; \
	INT_PM *coef_start; \
	int coef_ptr; \
	int interpolate; \
	int decimate; \
	int sym_clk_offset; \
	int sym_clk_memory; \
	int sym_clk_phase; \
	unsigned int carrier; \
	int *map_ptr; \
	int *amp_ptr; \
	unsigned int phase; \
	unsigned int Sreg; \
	int Sreg_low; \
	int fir_scale; \
	unsigned int Ereg

	/**** APSK demodulator common structures ****/

struct IIR_RESONATOR_BLOCK {
	int coef;
	int dnm1;
	int dnm2;
};

#define RX_APSK_DMOD_BLOCK \
	int (*decoder_ptr)(struct RX_BLOCK *); \
	int (*slicer_ptr)(struct RX_BLOCK *); \
	int (*timing_ptr)(struct RX_BLOCK *); \
	int baud_counter; \
	int *data_ptr; \
	int *sample_ptr; \
	int fir_taps; \
	INT_PM *coef_start; \
	int coef_ptr; \
	int sym_clk_phase; \
	int interpolate; \
	int decimate; \
	int oversample; \
	int *timing_start; \
	int I; \
	int Q; \
	int IEQ; \
	int QEQ; \
	int Iprime; \
	int Qprime; \
	int Inm1; \
	int Qnm1; \
	int Inm2; \
	int Qnm2; \
	int Inm3; \
	int Qnm3; \
	int Ihat; \
	int Qhat; \
	int Ihat_nm2; \
	int Qhat_nm2; \
	int What; \
	int *fir_ptr; \
	int IEQprime_error; \
	int QEQprime_error; \
	int EQ_MSE; \
	int EQ_2mu; \
	int COS; \
	int SIN; \
	unsigned int LO_memory; \
	unsigned int LO_frequency; \
	int LO_phase; \
	unsigned int vco_memory; \
	int phase_error; \
	int loop_memory; \
	int loop_memory_low; \
	int loop_K1; \
	int loop_K2; \
	struct IIR_RESONATOR_BLOCK PJ1; \
	struct IIR_RESONATOR_BLOCK PJ2; \
	int agc_gain; \
	int agc_K; \
	int frequency_est; \
	int sym_clk_memory; \
	int timing_threshold; \
	int coarse_error; \
	int LOS_counter; \
	int LOS_monitor; \
	int map_shift; \
	int Phat; \
	int phase; \
	int EQ_taps; \
	unsigned int Dreg; \
	int Dreg_low; \
	unsigned int pattern_reg; \
	int *demod_delay_ptr; \
	int *trace_back_ptr; \
	int *signal_map_ptr; \
	int *EC_fir_ptr; \
	int *EC_sample_ptr; \
	int EC_2mu;	\
	int EC_MSE;	\
	int EC_taps; \
	int EC_shift; \
	int RTD
	
/*++++#ifndef MESI_INTERNAL 03-22-2001  */
	/**** FSK modulator common structure ****/

#define TX_FSK_MOD_BLOCK \
	int coef_ptr; \
	int interpolate; \
	int decimate; \
	int carrier; \
	int tone_scale; \
	unsigned int vco_memory; \
	int frequency;\
	int frequency_shift

	/**** FSK demodulator common structure ****/

#define RX_FSK_DMOD_BLOCK \
	int mark_coef; \
	int space_coef; \
	int sym; \
	int coef_len; \
	int coef_ptr; \
	int interpolate; \
	int decimate; \
	int sym_nm1; \
	int sym_hat; \
	int sym_hat_nm2; \
	int sym_clk_memory; \
	int baud_counter; \
	int sym_level; \
	int LOS_threshold; \
	int LOS_memory
/*++++#endif */ /* MESI_INTERNAL 03-22-2001  */

	/**** set up modem operating point, OP_POINT ****/

#ifdef VSIM /* if VSIM, calculate parameters from floating point OP_POINT */
#define OP_POINT64 			        (1/64.0)	
#define OP_POINT32 			        (1/32.0)	
#define OP_POINT16 			        (1/16.0)	
#define OP_POINT8 			        (1/8.0)	
#define OP_POINT4 			        (1/4.0)	
#define OP_POINT2 			        (1/2.0)	
#else  /* VSIM else -> don't calculate parameters from floating point OP_POINT */
#define OP_POINT64 			        512
#define OP_POINT32 			        1024
#define OP_POINT16 			        2048
#define OP_POINT8 			        4096
#define OP_POINT4 			        8192
#define OP_POINT2 			        16384
#endif /* VSIM endif -> calculate parameters from floating point OP_POINT */

#ifndef OP_POINT
#define OP_POINT 			        OP_POINT64	
/*#define OP_POINT 			        OP_POINT32  */
/*#define OP_POINT 			        OP_POINT16  */
/*#define OP_POINT 					OP_POINT8   */
/*#define OP_POINT 			        OP_POINT4   */
/*#define OP_POINT 			        OP_POINT2   */
#endif

#if OP_POINT == OP_POINT64
#define OP_POINT_SHIFT				6
#endif
#if OP_POINT == OP_POINT32
#define OP_POINT_SHIFT		        5
#endif
#if OP_POINT == OP_POINT16
#define OP_POINT_SHIFT		        4
#endif
#if OP_POINT == OP_POINT8
#define OP_POINT_SHIFT		        3
/*#ifndef MESI_INTERNAL 02-26-2001 SQUARE_ROOT_WHAT MODS */
/*#else */  /* MESI_INTERNAL 02-26-2001 SQUARE_ROOT_WHATMODS */
#ifdef VSIM                            
#define SQUARE_ROOT_WHAT	           
#endif                                 
/*#endif */ /* MESI_INTERNAL 02-26-2001 SQUARE_ROOT_WHAT MODS */
#endif
#if OP_POINT == OP_POINT4
#define OP_POINT_SHIFT		        2
#endif
#if OP_POINT == OP_POINT2
#define OP_POINT_SHIFT		        1
#endif

	/**** RCOS filter coefficients ****/

#define ROLL600 					0.75
#define OVERSAMPLE600 				1
#define INTERP600 					(3*OVERSAMPLE600)
#define DEC600 						(20*OVERSAMPLE600)
#define TAPS600		 				60
#define RCOS600_LEN 				(TAPS600*INTERP600+DEC600)
#define RX_RCOS600_LEN 				(TAPS600*INTERP600+DEC600)
#define FS600						(8000.0*INTERP600)
#define TX_TAPS600		 			3
#define TX_RCOS600_LEN 				(TX_TAPS600*2*DEC600+INTERP600+1) /* add 1 to make it even */

#define ROLL1200 					0.75
#define OVERSAMPLE1200 				4
#define INTERP1200 					(3*OVERSAMPLE1200)
#define DEC1200 					(10*OVERSAMPLE1200)
#define TAPS1200		 			18
#define RCOS1200_LEN 				(TAPS1200*INTERP1200+DEC1200)
#define FS1200						(8000.0*INTERP1200)

#define ROLL1600 					0.75
#define OVERSAMPLE1600 				2
#define INTERP1600 					(6*OVERSAMPLE1600)
#define DEC1600 					(15*OVERSAMPLE1600)
#define TAPS1600		 			18
#define RCOS1600_LEN 				(TAPS1600*INTERP1600+DEC1600)
#define FS1600						(8000.0*INTERP1600)

#define ROLL2400 					0.75
#define OVERSAMPLE2400 				8
#define INTERP2400 					(3*OVERSAMPLE2400)
#define DEC2400 					(5*OVERSAMPLE2400)
#define TAPS2400		 			16
#define RCOS2400_LEN 				(TAPS2400*INTERP2400+DEC2400)
#define FS2400						(8000.0*INTERP2400)

#define ROLL3000 					0.45
#define OVERSAMPLE3000 				8
#define INTERP3000 					(3*OVERSAMPLE3000)
#define DEC3000 					(4*OVERSAMPLE3000)
#define TAPS3000		 			16
#define RCOS3000_LEN 				(TAPS3000*INTERP3000+DEC3000)
#define FS3000						(8000.0*INTERP3000)

#define ROLL3200 					0.25
#define OVERSAMPLE3200 				8
#define INTERP3200 					(4*OVERSAMPLE3200)
#define DEC3200 					(5*OVERSAMPLE3200)
#define TAPS3200		 			20 
#define RCOS3200_LEN 				(TAPS3200*INTERP3200+DEC3200)
#define FS3200						(8000.0*INTERP3200)

	/**** APSK modulator parameters ****/

#define SYM_CLK_THRESHOLD			16384

	/**** APSK demodulator parameters ****/

#ifdef VSIM /* if VSIM, calculate parameters from floating point OP_POINT */
/*++++#ifndef MESI_INTERNAL 03-13-2001 OP_POINT8 MODS */
/*#define COARSE_THR					((int)(32768.0*16.97056*OP_POINT))*/
/*++++#else   MESI_INTERNAL 03-13-2001 OP_POINT8 MODS */
#define COARSE_THR					((int)(32768.0*OP_POINT))
/*++++#endif  MESI_INTERNAL 03-13-2001 OP_POINT8 MODS */
#define TIMING_TABLE_LEN			(4*4)
#define COEF_INCR 					(0*4)
#define COEF_DECR 					(1*4)
#define FIR_INCR 					(2*4)
#define FIR_DECR 					(3*4)
	
#define REV_CORR_LEN 				16
#define REV_CORR_DELAY 				(4*REV_CORR_LEN/4)		/* 2 symbols*2x baud*(REV_CORR_LEN/4)*/
#define LOS_THRESHOLD 				((int)(32768.0*0.25*OP_POINT))
#define LOS_COUNT 					4
#define UNLOCKED 					0
#define LOCKED 						400

#define EQ_COEF_SEED 				(16384/4)
/*++++#ifndef MESI_INTERNAL 03-08-2001 OP_POINT8 MODS */
/*#define EQ_2MU_SCALE 				(15-(1+OP_POINT_SHIFT))*/
/*++++#endif  MESI_INTERNAL 03-08-2001 OP_POINT8 MODS */
#define EQ_DISABLED 				-1
#define EQ_UPDATE_DISABLED 			0
#define EQ_FIR_ENABLED 				0

#define MSE_B0 						328		/* (32768*0.01)*/
#define MSE_A1 						32440	/* (32768-MSE_B0)*/
#define AGC_REF 					((int)(32768.0*OP_POINT))
#define AGC_EST_SEED 				20287	/* -44 dB*/
#define AGC_EST_STEP 				16423

#define COS_PI_BY_4 				23170	/* 32768*cos(pi/4)*/
#define TWENTY_SIX_DEGREES 			4836
#define FOURTY_FIVE_DEGREES			8192
#define NINETY_DEGREES 				16384
#define ONE_EIGHTY_DEGREES 			32768
#define RX_DET_LEN 					80

/*#ifndef MESI_INTERNAL 03-07-2001 */
/*#define PJ_COEF_A  */ 			  /*  (1/OP_POINT) */
/*#else */  /* MESI_INTERNAL 03-07-2001 */
#define PJ_COEF_A					(2<<OP_POINT_SHIFT)
/*#endif */ /* MESI_INTERNAL 03-07-2001 */
#define PJ50_COEF600_B				-28378	/* 32768*cos(2*pi*50/600) */
#define PJ60_COEF600_B				-26510	/* 32768*cos(2*pi*60/600) */
#define PJ50_COEF1200_B				-31651	/* 32768*cos(2*pi*50/1200) */
#define PJ60_COEF1200_B				-31164	/* 32768*cos(2*pi*60/1200) */
#define PJ50_COEF1600_B				-32138	/* 32768*cos(2*pi*50/1600) */
#define PJ60_COEF1600_B				-31863	/* 32768*cos(2*pi*60/1600) */
#define PJ50_COEF2400_B				-32488	/* 32768*cos(2*pi*50/2400) */
#define PJ60_COEF2400_B				-32365	/* 32768*cos(2*pi*60/2400) */
#define PJ50_COEF3000_B				-32588  /* 32768*cos(2*pi*50/3000) */
#define PJ60_COEF3000_B				-32510  /* 32768*cos(2*pi*60/3000) */
#define PJ50_COEF3200_B				-32610  /* 32768*cos(2*pi*50/3200) */
#define PJ60_COEF3200_B				-32541  /* 32768*cos(2*pi*60/3200) */

#else  /* VSIM else -> don't calculate parameters from floating point OP_POINT */

#define COARSE_THR 					(6*2*724)	/* 6*(2*OP_POINT*SQRT(2)) */
#define TIMING_TABLE_LEN			(4*4)
#define COEF_INCR 					(0*4)
#define COEF_DECR 					(1*4)
#define FIR_INCR 					(2*4)
#define FIR_DECR 					(3*4)
	
#define REV_CORR_LEN 				16
#define REV_CORR_DELAY 				(4*REV_CORR_LEN/4)	/* 2 symbols*2x baud*(REV_CORR_LEN/4)*/
#define LOS_THRESHOLD 	    		128	    /* (OP_POINT*0.25)*/
#define LOS_COUNT 					4
#define UNLOCKED 					0
#define LOCKED 						400

#define EQ_COEF_SEED 				(16384/4)
#define EQ_2MU_SCALE 				8
#define EQ_DISABLED 				-1
#define EQ_UPDATE_DISABLED 			0
#define EQ_FIR_ENABLED 				0

#define MSE_B0 						328		/* (32768*0.01)*/
#define MSE_A1 						32440	/* (32768-MSE_B0)*/
#define AGC_REF 					512	    /* OP_POINT^2 scaled up by 64*/
#define AGC_EST_SEED 				20287	/* -44 dB*/
#define AGC_EST_STEP 				16423

#define COS_PI_BY_4 				23170	/* 32768*cos(pi/4)*/
#define TWENTY_SIX_DEGREES 			4836
#define FOURTY_FIVE_DEGREES			8192
#define NINETY_DEGREES 				16384
#define ONE_EIGHTY_DEGREES 			32768
#define RX_DET_LEN 					80

#define PJ_COEF_A					64		/* 1/OP_POINT */
#define PJ50_COEF600_B				-28378	/* 32768*cos(2*pi*50/600) */
#define PJ60_COEF600_B				-26510	/* 32768*cos(2*pi*60/600) */
#define PJ50_COEF1200_B				-31651	/* 32768*cos(2*pi*50/1200) */
#define PJ60_COEF1200_B				-31164	/* 32768*cos(2*pi*60/1200) */
#define PJ50_COEF1600_B				-32138	/* 32768*cos(2*pi*50/1600) */
#define PJ60_COEF1600_B				-31863	/* 32768*cos(2*pi*60/1600) */
#define PJ50_COEF2400_B				-32488	/* 32768*cos(2*pi*50/2400) */
#define PJ60_COEF2400_B				-32365	/* 32768*cos(2*pi*60/2400) */
#define PJ50_COEF3000_B				-32588  /* 32768*cos(2*pi*50/3000) */
#define PJ60_COEF3000_B				-32510  /* 32768*cos(2*pi*60/3000) */
#define PJ50_COEF3200_B				-32610  /* 32768*cos(2*pi*50/3200) */
#define PJ60_COEF3200_B				-32541  /* 32768*cos(2*pi*60/3200) */

#endif /* VSIM endif -> calculate parameters from floating point OP_POINT */

	/**** Tx_scale attenuation values ****/

#define ATTENUATE_2DB				26029	/* 32768*10exp(-2 dB/20)	*/
#define ATTENUATE_3DB				23198	/* 32768*10exp(-3 dB/20)	*/
#define ATTENUATE_6DB				16423	/* 32768*10exp(-6 dB/20)	*/
#define ATTENUATE_7DB				14636	/* 32768*10exp(-7 dB/20)	*/
#define ATTENUATE_9DB				11627	/* 32768*10exp(-9 dB/20)	*/
#define ATTENUATE_10DB				10362	/* 32768*10exp(-10 dB/20)	*/
#define ATTENUATE_12DB				8231	/* 32768*10exp(-12 dB/20)	*/
#define ATTENUATE_16DB				5192	/* 32768*10exp(-16 dB/20)	*/
#define ATTENUATE_20DB				3277	/* 32768*10exp(-20 dB/20)	*/
#define ATTENUATE_28DB				1305	/* 32768*10exp(-28 dB/20)	*/
#define ATTENUATE_30DB				1036	/* 32768*10exp(-30 dB/20)	*/

/****************************************************************************/

#endif	/* inclusion */

