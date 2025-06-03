;****************************************************************************
;* Filename: filter.inc
;* Date: 04-19-00
;* Author: Peter B. Miller
;* Company: MESi
;*		   10909 Lamplighter Lane, Potomac, MD 20854
;* Phone: (301) 765-9668
;* E-mail: peter.miller@mesi.net
;* Website: www.mesi.net
;* Description: common filter components
;****************************************************************************

	;**** Hilbert bandpass filter coefficients ****

FILTER_150_COEF				.set	5		;* (256/8000)*150	  
FILTER_300_COEF				.set	10		;* (256/8000)*300	  
FILTER_350_COEF				.set	11		;* (256/8000)*350	 
FILTER_425_COEF				.set	14		;* (256/8000)*425 
FILTER_440_COEF				.set	14		;* (256/8000)*440	 
FILTER_450_COEF				.set	14		;* (256/8000)*450	  
FILTER_460_COEF				.set	15		;* (256/8000)*460	  
FILTER_480_COEF				.set	15		;* (256/8000)*480	 
FILTER_500_COEF				.set	16		;* (256/8000)*500	   
FILTER_600_COEF				.set	19		;* (256/8000)*600	 
FILTER_620_COEF				.set	20		;* (256/8000)*620	 
FILTER_750_COEF				.set	24		;* (256/8000)*750	  
FILTER_900_COEF				.set	29		;* (256/8000)*900	  
FILTER_980_COEF				.set	31		;* (256/8000)*980	   
FILTER_1000_COEF			.set	32		;* (256/8000)*1000	  
FILTER_1050_COEF			.set	33		;* (256/8000)*1050	  
FILTER_1080_COEF			.set	35		;* (256/8000)*1080	  
FILTER_1100_COEF			.set	35		;* (256/8000)*1100	  
FILTER_1180_COEF			.set	38		;* (256/8000)*1180	  
FILTER_1200_COEF			.set	38		;* (256/8000)*1200	  
FILTER_1350_COEF			.set	43		;* (256/8000)*1350	  
FILTER_1650_COEF			.set	53		;* (256/8000)*1650	  
FILTER_1500_COEF			.set	48		;* (256/8000)*1500	  
FILTER_1700_COEF			.set	54		;* (256/8000)*1700	  
FILTER_1750_COEF			.set	56		;* (256/8000)*1750	  
FILTER_1800_COEF			.set	58		;* (256/8000)*1800	  
FILTER_1850_COEF			.set	59		;* (256/8000)*1850	  
FILTER_1950_COEF			.set	62		;* (256/8000)*1950	  
FILTER_2100_COEF			.set	67		;* (256/8000)*2100	  
FILTER_2130_COEF			.set	68		;* (256/8000)*2130 
FILTER_2225_COEF			.set	71		;* (256/8000)*2225	  
FILTER_2250_COEF			.set	72		;* (256/8000)*2250	  
FILTER_2300_COEF			.set	74		;* (256/8000)*2300 
FILTER_2400_COEF			.set	77		;* (256/8000)*2400	  
FILTER_2600_COEF			.set	83		;* (256/8000)*2600	  
FILTER_2550_COEF			.set	82		;* (256/8000)*2550	  
FILTER_2700_COEF			.set	86		;* (256/8000)*2700	  
FILTER_2750_COEF			.set	88		;* (256/8000)*2750 
FILTER_2850_COEF			.set	91		;* (256/8000)*2850	  
FILTER_2900_COEF			.set	93		;* (256/8000)*2900	  
FILTER_3000_COEF			.set	96		;* (256/8000)*3000	  
FILTER_3150_COEF			.set	101		;* (256/8000)*3150	  
FILTER_3300_COEF			.set	106		;* (256/8000)*3300	  
FILTER_3450_COEF			.set	110		;* (256/8000)*3450	  
FILTER_3600_COEF			.set	115		;* (256/8000)*3600	  
FILTER_3750_COEF			.set	120		;* (256/8000)*3750	  

RX_ANALYSIS_LEN				.set	80	
RX_ANALYSIS_COEF			.set	DFT_COEF
BROADBAND_EST_LEN	  		.set	80
BROADBAND_EST_COEF	 		.set	482		;* 32678*(1/BB_EST_LEN)*fudge
					  
	;**** thresholds ****

ONE_BY_ROOT3				.set	18919	;* 32768/sqrt(3)															
ONE_BY_ROOT2				.set	23170	;* 32768/sqrt(2)
THR_POINT5DB				.set	30934	;* 32768*10exp(-0.5 dB/20)	
THR_1DB						.set	29204	;* 32768*10exp(-1 dB/20)
THR_2DB						.set	26028	;* 32768*10exp(-2 dB/20)
THR_3DB						.set	23198	;* 32768*10exp(-3 dB/20)
THR_4DB						.set	20675	;* 32768*10exp(-4 dB/20)
THR_6DB						.set	16423	;* 32768*10exp(-6 dB/20)
THR_9DB						.set	11627	;* 32768*10exp(-9 dB/20)
THR_10DB					.set	10362	;* 32768*10exp(-10 dB/20)
THR_12DB					.set	8231	;* 32768*10exp(-12 dB/20)
THR_14DB					.set	6538	;* 32768*10exp(-14 dB/20)
THR_16DB					.set	5193	;* 32768*10exp(-16 dB/20)
THR_20DB					.set	3277	;* 32768*10exp(-20 dB/20)
THR_24DB					.set	2067	;* 32768*10exp(-24 dB/20)	
THR_28DB					.set	1305	;* 32768*10exp(-28 dB/20)
THR_30DB					.set	1036	;* 32768*10exp(-30 dB/20)
THR_33DB					.set	733		;* 32768*10exp(-33 dB/20)
THR_36DB					.set	519		;* 32768*10exp(-36 dB/20)
THR_40DB					.set	328		;* 32768*10exp(-40 dB/20)
THR_43DB					.set	232		;* 32768*10exp(-43 dB/20)
THR_48DB					.set	130		;* 32768*10exp(-48 dB/20)
THR_52DB					.set	82		;* 32768*10exp(-52 dB/20)

;****************************************************************************
