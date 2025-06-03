/****************************************************************************/
/* File: "c54x.h"															*/
/* Date: 4 April 1997														*/
/* Author: Peter B. Miller													*/
/* Company: Miller Engineering Services, Inc. (MESi)						*/
/*	    10909 Lamplighter Lane, Potomac, MD 20854							*/
/* Phone: (301) 765-9668													*/
/*	E-mail: peter.miller@mesi.net											*/
/* Description: structure definitions for 'C54x memory mapped				*/
/* peripherals.																*/
/****************************************************************************/

#ifndef C54X_INCLUSION_
#define C54X_INCLUSION_

	/**** board-specific reset and interrupt vector creation ****/

#ifdef TIGER542
#define TIGER54X					1
#define CREATE_VECTORS 				ENABLED
#endif
#ifdef TIGER549
#define TIGER54X					1
#define CREATE_VECTORS 				ENABLED
#endif
#ifdef TIGER5410
#define TIGER54X					1
#define CREATE_VECTORS 				ENABLED
#endif

/*++++++++10-30-00++++*/
#ifdef TESTBED
#define STEREO						ENABLED
#endif
/*++++++++10-30-00++++*/

#ifdef EVM541
#define CREATE_VECTORS 				ENABLED
#endif
#ifdef DSK542
#define CREATE_VECTORS 				DISABLED
#endif
#ifdef DSK5402
#define CREATE_VECTORS 				ENABLED
#endif

#ifdef WYLE5402
#define WYLE						ENABLED
#endif
#ifdef WYLE5409
#define WYLE						ENABLED
#endif

#ifdef WYLE
 asm("WYLE					.set	1										");
#define CREATE_VECTORS 				ENABLED
extern int ring_nm1;
extern int PSD_register;
#endif

//+++++++++09-19-00 SILABS MODS
#ifdef SI303X_DSK542
#define SI303X	
#endif
//+++++++++09-19-00 SILABS MODS

 asm("				");
 asm("	;**** reset and interrupt branch vector labels ****					");
 asm("				");
 asm("	.global	reset														");
 asm("	.global	rint0	 													");
 asm("	.global	xint0	 													");
 asm("	.global	rint1	 													");
 asm("	.global	xint1	 													");
 asm("	.global	brint0	 													");
 asm("	.global	bxint0	 													");
 asm("	.global	brint1	 													");
 asm("	.global	bxint1	 													");
 asm("	.global	brint2	 													");
 asm("	.global	bxint2	 													");
 asm("				");

/****************************************************************************/

	/**** T.I. memory-mapped peripheral addresses and field definitions ****/

#define C5410_DRR12					0x0031
#define C5410_DXR12					0x0033
#define C5410_DRR11					0x0041
#define C5410_DXR11					0x0043
#define C5410_DRR10					0x0021
#define C5410_DXR10					0x0023

 asm(";****************************************************************		");
 asm(";* T.I. memory-mapped peripheral addresses and field definitions 		");
 asm(";****************************************************************		");
 asm("				");
 asm("	.mmregs																");
 asm("				");
 asm("	;**** standard serial ports ****									");
 asm("				");
 asm("DLB					.set	0002h									");
 asm("XRDY_BIT				.set	0800h									");
 asm("XIOEN					.set	2000h									");
 asm("RIOEN					.set	1000h									");
 asm("DX_STAT				.set	0020h									");
 asm("DR_STAT				.set	0010h									");
 asm("																		");
 asm("HPINT_MASK			.set	0200h									");
 asm("RINT0_MASK			.set	0010h									");
 asm("XINT0_MASK			.set	0020h									");
 asm("RINT1_MASK			.set	0040h									");
 asm("XINT1_MASK			.set	0080h									");
 asm("TRINT_MASK			.set	0040h									");
 asm("TXINT_MASK			.set	0080h									");
 asm("				");
 asm("	;**** McBSP0 ****													");
 asm("				");
 asm("DRR20					.set	0020h									");
 asm("DRR10					.set	0021h									");
 asm("DXR20					.set	0022h									");
 asm("DXR10					.set	0023h									");
 asm("SPSA0					.set	0038h									");
 asm("SPSD0					.set	0039h									");
 asm("BRINT0_MASK			.set	0010h									");
 asm("BXINT0_MASK			.set	0020h									");
 asm("				");
 asm("	;**** McBSP1 ****													");
 asm("				");
 asm("DRR21					.set	0040h									");
 asm("DRR11					.set	0041h									");
 asm("DXR21					.set	0042h									");
 asm("DXR11					.set	0043h									");
 asm("SPSA1					.set	0048h									");
 asm("SPSD1					.set	0049h									");
 asm("BRINT1_MASK			.set	0400h									");
 asm("BXINT1_MASK			.set	0800h									");
 asm("				");
 asm("	;**** McBSP2 ****													");
 asm("				");
 asm("DRR22					.set	0030h									");
 asm("DRR12					.set	0031h									");
 asm("DXR22					.set	0032h									");
 asm("DXR12					.set	0033h									");
 asm("SPSA2					.set	0034h									");
 asm("SPSD2					.set	0035h									");
 asm("BRINT2_MASK			.set	0040h									");
 asm("BXINT2_MASK			.set	0080h									");
 asm("				");
 asm("	;**** McBSP subaddress registers ****								");
 asm("				");
 asm("SPCR1_SUBADDR			.set	0000h									");
 asm("SPCR2_SUBADDR			.set	0001h									");
 asm("RCR1_SUBADDR			.set	0002h									");
 asm("RCR2_SUBADDR			.set	0003h									");
 asm("XCR1_SUBADDR			.set	0004h									");
 asm("XCR2_SUBADDR			.set	0005h									");
 asm("SRGR1_SUBADDR			.set	0006h									");
 asm("SRGR2_SUBADDR			.set	0007h									");
 asm("MCR1_SUBADDR			.set	0008h									");
 asm("MCR2_SUBADDR			.set	0009h									");
 asm("RCERA_SUBADDR			.set	000ah									");
 asm("RCERB_SUBADDR			.set	000bh									");
 asm("XCERA_SUBADDR			.set	000ch									");
 asm("XCERB_SUBADDR			.set	000dh									");
 asm("PCR_SUBADDR			.set	000eh									");
 asm("				");
 asm("	;**** McBSP control bits ****										");
 asm("				");
 asm("SPCR2_XRST_BIT 		.set	0001h									");
 asm("SPCR2_XRDY_BIT 		.set	0002h									");
 asm("				");
/****************************************************************************/

 asm(";****************************************************************		");
 asm(";* Configure the serial port for:										");
 asm(";* 	FSM=continuous frame sync mode									");
 asm(";* 	external CLKX (clock)											");
 asm(";* 	external FSX (frame sync)										");
 asm(";****************************************************************		");
 asm("				");
 asm("SPC_RST				.set	0008h									");
 asm("SPC_ENBL				.set	00c8h									");

#ifdef TIGER54X
#define HW_SYSTEM_DELAY				36

 asm(";****************************************************************		");
 asm(";* Tiger54x hardware control and status registers 					");
 asm(";****************************************************************		");
 asm("				");
 asm("STATUS_REG0			.set	4000h									");
 asm("PSQ					.set	0004h									");
 asm("				");
 asm("CONTROL_REG0			.set	4000h									");
 asm("EPROMDIS				.set	0001h									");
 asm("URESET				.set	0002h									");
 asm("CTFCLR				.set	0004h									");
 asm("C542IRQ				.set	0008h									");
 asm("				");
 asm("CONTROL_REG1			.set	4001h									");
 asm("OFFHOOK				.set	0001h									");
 asm("PORTSEL				.set	0002h									");
 asm("RCHAN					.set	0004h									");
 asm("LCHAN					.set	0008h									");
 asm("CCS					.set	0010h									");
 asm("OSCSEL				.set	0020h									");
 asm("MF6					.set	0040h									");
 asm("MF7					.set	0080h									");
 asm("MF8					.set	0100h									");
 asm("GPOUT0				.set	0200h									");
 asm("GPOUT1				.set	0400h									");
 asm("				");
 asm("CLOCK_CODEC_REG		.set	4002h									");
 asm("DUAL_DAC_PORT			.set	4003h									");
 asm("				");
 asm("UART_REG6				.set	8006h									");
 asm("UART_RI_BIT			.set	0040h									");
#ifdef TIGER542
 asm("SWWSR_INIT			.set	2000h	;* 2 waits I/O, 0 ext memory 	");
#endif
#ifdef TIGER549
 asm("SWWSR_INIT			.set	0a249h	;* 2 waits I/O, 1 ext memory 	");
#endif
#ifdef TIGER5410
 asm("SWWSR_INIT			.set	0a249h	;* 2 waits I/O, 1 ext memory 	");
#endif
#endif

#ifdef EVM541
#define HW_SYSTEM_DELAY				6
 asm("EVM541_TX_SCALE		.set	21402	;* -3.7 dB attenuation 			");

 asm(";****************************************************************		");
 asm(";* EVM541 hardware control and status registers 						");
 asm(";****************************************************************		");
 asm("				");
 asm("TARGET_CONTROL_REG	.set	0014h									");
 asm("EVM541_RESET_BIT		.set	8000h									");
 asm("EVM541_SEC_INIT		.set	3										");
 asm("EVM541_A_REG			.set	0124h		;* Fclk=144kHz				");
 asm("EVM541_B_REG			.set	0212h		;* Fsample=8.0 kHz			");
 asm("EVM541_GAIN_REG		.set	040dh		;* Rx=12 dB, Tx=0dB			");
 asm("EVM541_ANALOG_REG		.set	0502h		;* HPF enbl, AUXIN enbl		");
#endif

#ifdef DSK542
#define HW_SYSTEM_DELAY				11

 asm(";****************************************************************		");
 asm(";* DSK542 hardware control and status registers 						");
 asm(";****************************************************************		");
 asm("				");
 asm("TARGET_CONTROL_REG	.set	0014h									");
 asm("DSK542_RESET_BIT		.set	8000h									");
 asm("DSK542_SEC_INIT		.set	3										");
 asm("DSK542_A_REG			.set	0124h		;* Fclk=144kHz				");
 asm("DSK542_B_REG			.set	0210h		;* Fsample=8.0 kHz			");
 asm("DSK542_ANALOG_REG		.set	0502h		;* HPF enbl, AUXIN enbl		");
 asm("DSK542_GAIN_REG		.set	0406h		;* Rx=0 dB, Tx=-6dB			");
#endif

#ifdef DSK5402
//+++++++++09-19-00 SILABS MODS
//#define HW_SYSTEM_DELAY					39
#ifdef SI303X
#define HW_SYSTEM_DELAY					29
#else
#define HW_SYSTEM_DELAY					39
#endif
//+++++++++09-19-00 SILABS MODS

 asm(";****************************************************************		");
 asm(";* DSK5402 hardware control and status registers 						");
 asm(";****************************************************************		");
 asm("				");
 asm("SEC_INIT				.set	1										");
 asm("AD50_CONTROL4			.set	40ah	;* AD50 Control register 4:		");
 asm("										;* Analog input gain=12dB 		");
 asm("										;* Analog output gain=-12dB		");
// asm("AD50_CONTROL4			.set	409h	;* AD50 Control register 4:		");
// asm("										;* Analog input gain=12dB 		");
// asm("										;* Analog output gain=-6dB		");
 asm("SWWSR_INIT			.set	09249h	;* 1 waits I/O, 1 ext memory 	");
 asm("BSCR_INIT				.set	08866h	;* no bank switching	 		");
 asm("				");
 asm("CNTL1_PORT			.set	0000h	;* CNTL1 I/O address			");
 asm("STAT_PORT				.set	0001h	;* STST I/O address				");
 asm("DMCNTL_PORT			.set	0002h	;* DMCNTL I/O address			");
 asm("DBIO_PORT				.set	0003h	;* DBIO I/O address				");
 asm("CNTL2_PORT			.set	0004h	;* CNTL2 I/O address			");
 asm("SEM0_PORT				.set	0005h	;* SEM0 I/O address				");
 asm("SEM1_PORT				.set	0006h	;* SEM1 I/O address				");
 asm("DAAOH_BIT				.set	0080h	;* DAA offhook control bit		");
 asm("DAARING_BIT	 		.set	0001h	;* DAA ring detect bit			");
 asm("DBIO_RST_BIT			.set	0001h	;* DBIO reset bit				");
 asm("DBIO_OFHK_BIT			.set	0002h	;* DBIO off hook bit			");
 asm("DBIO_RGDT_BIT			.set	0004h	;* DBIO ring detect bit			");
 asm("BSPSEL0	 		    .set	0001h	;* McBSP Select Control     	");
 asm("BSPSEL1	 		    .set	0002h	;* McBSP Select Control     	");
 asm("				");
 asm("RING_SCALE			.set	4										");
 asm("RING_AVG_LEN			.set	2048									");
 asm("RING_B0				.set	(32768/RING_AVG_LEN)					");
 asm("RING_A1				.set	((32768-RING_B0)>>RING_SCALE)			");
 asm("RING_THRESHOLD		.set	4096	;* 1/8							");

#endif

#ifdef SI303X_DSK542
#define HW_SYSTEM_DELAY				11
//+++++++++09-19-00 SILABS MODS
#endif

#ifdef SI303X
//+++++++++09-19-00 SILABS MODS
 asm(";****************************************************************		");
 asm(";* SI303X EVB hardware control and status registers 					");
 asm(";****************************************************************		");
 asm("				");
 asm("SI303X_DSK_SEC_INIT		.set	0001h								");
 asm("SI303X_DSK_SEC_COMM_FLAG	.set	8000h								");
 asm("RW_BIT				.set	2000h									");
 asm("OHE_BIT				.set	0002h									");
 asm("OH_BIT				.set	0001h									");
 asm("RDT_BIT				.set	0004h									");
 asm("FDT_BIT				.set	0040h									");
 asm("REGISTER1				.set	0100h									");
 asm("REGISTER2				.set	0200h									");
 asm("REGISTER3				.set	0300h									");
 asm("REGISTER4				.set	0400h									");
 asm("REGISTER5				.set	0500h									");
 asm("REGISTER6				.set	0600h									");
 asm("REGISTER7				.set	0700h									");
 asm("REGISTER8				.set	0800h									");
 asm("REGISTER9				.set	0900h									");
 asm("REGISTER10			.set	0a00h									");
 asm("REGISTER11			.set	0b00h									");
 asm("REGISTER12			.set	0c00h									");
 asm("REGISTER13			.set	0d00h									");
 asm("REGISTER14			.set	0e00h									");
 asm("REGISTER15			.set	0f00h									");
 asm("				");
#endif

#ifdef AD73311
#define HW_SYSTEM_DELAY				11

 asm(";****************************************************************		");
 asm(";* AD73311 hardware control and status registers 						");
 asm(";****************************************************************		");
 asm("				");
 asm("ADDRESS_FIELD			.set	3800h									");
 asm("CONTROL_BIT			.set	8000h									");
 asm("READ_BIT				.set	4000h									");
 asm("DEVICE_ADDRESS		.set	0000h									");
 asm("CRA_ADDRESS			.set	0000h									");
 asm("CRB_ADDRESS			.set	0100h									");
 asm("CRC_ADDRESS			.set	0200h									");
 asm("CRD_ADDRESS			.set	0300h									");
 asm("CRE_ADDRESS			.set	0400h									");
 asm("				");
 asm("REGISTER_A			.set	8001h	;* switch to data mode			");
 asm("REGISTER_B			.set	8133h	;* MCLK=11.289 mHz				");
 asm("REGISTER_C			.set	8279h	;* power up						");
 asm("REGISTER_D			.set	8320h	;* input/output gain:			");
 asm("										;* input gain=0 dB				");
 asm("										;* output gain=0 dB				");
 asm("REGISTER_E			.set	8000h	;* no advance/interp			");
 asm("				");
 asm("AD73311_PORT			.set	0		;* I/O port for AD73311 reset	");
 asm("AD73311_RESET			.set	1										");
 asm("AD73311_ENABLE		.set	0										");
 asm("SWWSR_INIT			.set	07fffh	;* max waits I/O, max ext memory");
 asm("BSCR_INIT				.set	00002h									");
#endif

#ifdef WYLE
#define HW_SYSTEM_DELAY					39

 asm(";****************************************************************		");
 asm(";* Wyle Reference hardware control and status registers 				");
 asm(";****************************************************************		");
 asm("				");
 asm("SEC_INIT				.set	1										");
 asm("AD50_CONTROL4			.set	40ah	;* AD50 Control register 4:		");
 asm("										;* Analog input gain=12dB 		");
 asm("										;* Analog output gain=-12dB		");
 asm("SWWSR_INIT			.set	0ffffh	;* 7 waits I/O, 7 ext memory 	");
 asm("BSCR_INIT				.set	08866h	;* no bank switching	 		");
 asm("				");
 asm("DAARING_BIT	 		.set	0001h	;* DAA ring detect bit			");
 asm("				");
 asm("RING_SCALE			.set	4										");
 asm("RING_AVG_LEN			.set	2048									");
 asm("RING_B0				.set	(32768/RING_AVG_LEN)					");
 asm("RING_A1				.set	((32768-RING_B0)>>RING_SCALE)			");
 asm("RING_THRESHOLD		.set	4096	;* 1/8							");
 asm(" .if $isdefed(\"WYLE5402\")											");
 asm("	.asg	SPSA1,SPSA													");
 asm("	.asg	SPSD1,SPSD													");
 asm("	.asg	DXR11,DXR1													");
 asm("	.asg	DRR11,DRR1													");
 asm("	.asg	BRINT1_MASK,BRINT_MASK										");
 asm("	.asg	BXINT1_MASK,BXINT_MASK										");
 asm(" .endif																");
 asm(" .if $isdefed(\"WYLE5409\")											");
 asm("	.asg	SPSA2,SPSA													");
 asm("	.asg	SPSD2,SPSD													");
 asm("	.asg	DXR12,DXR1													");
 asm("	.asg	DRR12,DRR1													");
 asm("	.asg	BRINT2_MASK,BRINT_MASK										");
 asm("	.asg	BXINT2_MASK,BXINT_MASK										");
 asm(" .endif																");

//++++++++++05-28-00
 asm(";*********************************************************************");
 asm(";* PSD_DIRECTION determines input or output. Set bit to 1 for output.	");
 asm(";* PSD register bit definitions:										");
 asm(";* 	OIOO OIIO														");
 asm(";* 	---- ----														");
 asm(";* 	|||| ||||_ bit 0: CTS											");
 asm(";* 	|||| |||_ bit 1: DTR											");
 asm(";* 	|||| ||_ bit 2: RTS												");
 asm(";* 	|||| |_ bit 3: DSR												");
 asm(";* 	||||_ bit 4: DCD												");
 asm(";* 	|||_ bit 5: DAA CID snoop enable								");
 asm(";* 	||_ bit 6: Ring detect											");
 asm(";* 	|_ bit 7: LED													");
 asm(";*																	");
 asm(";*********************************************************************");
 asm("				");
 asm("PSD_DIRECTION			.set	08007h    ;* in program space.			");
 asm("										  ;* PSD_DIRECTION determines 	");
 asm("										  ;* input or output. Set bit 	");
 asm("										  ;* to 1 for output.			");
 asm("PSD_OUTPUT			.set	08005h    ;* in program space.			");
 asm("PSD_INPUT				.set	08001h    ;* in program space			");

 asm("PSD_OUTPUT_MASK		.set	000b9h     ;* Bit mask for outputs		");
 asm("PSD_INPUT_MASK		.set	~(PSD_OUTPUT_MASK)						");
 asm("PSD_CTS				.set	00001h									");
 asm("PSD_DTR_IN			.set	00002h									");
 asm("PSD_RTS_IN			.set	00004h									");
 asm("PSD_DSR				.set	00008h									");
 asm("PSD_DCD				.set	00010h									");
 asm("PSD_DAA_CID			.set	00020h									");
 asm("PSD_RING_IN			.set	00040h									");
 asm("PSD_LED				.set	00080h									");
//++++++++++05-28-00
#endif

	/**** defaults ****/

#ifndef HW_SYSTEM_DELAY
#define HW_SYSTEM_DELAY				0
#endif

/****************************************************************************/

	/**** structures for peripherals ****/

struct TIMER {
	unsigned int TIM;
	unsigned int PRD;
	unsigned int TCR;
	};

struct SPC {
	unsigned int DRR;
	unsigned int DXR;
	unsigned int SPC;
	unsigned int SPCE;
	};

	/**** hardware control functions ****/

extern void start_timer(void);
extern int stop_timer(void);
extern void write_sample_to_DAC(int);
extern int read_sample_from_ADC(void);
extern void dump_init(void);
extern void dump_write(int);
extern int dump[];
extern int *dump_ptr;
extern int HW_system_delay;
extern void init_hardware(void);
extern int go_off_hook(void);
extern int go_on_hook(void);
extern int wait_billing_delay(void);
extern int poll_ring_indicator(void);

extern void turn_LED0_on(void);
extern void turn_LED0_off(void);
extern void turn_LED1_on(void);
extern void turn_LED1_off(void);
extern void turn_LED2_on(void);
extern void turn_LED2_off(void);
extern void set_GPOUT0(void);
extern void clear_GPOUT0(void);
extern void set_GPOUT1(void);
extern void clear_GPOUT1(void);
/****************************************************************************/

#endif	/* inclusion */		
					   		
