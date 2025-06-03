#include  "su.h"

#define WATCH_DOG_RAM port4000
volatile ioport unsigned WATCH_DOG_RAM; 

#ifdef _FAR_MODE_
#ifndef _NO_WATCHDOG_
#define watchdog_feed_ram WATCH_DOG_RAM = 0;
#else
#define watchdog_feed_ram
#endif
#endif

typedef struct _Flash
{
	u16 *begin;
	u16 *end;
	u16 *first;
} Flash, *pFlash;

typedef struct _FlRec
{
	u16 *name;
	u16 lname;
	u16 *value;
	u16 lvalue;
} FlRec, *pFlRec;


void wrt_w(volatile u16 *, u16, u16);
int read_w(volatile u16 *, u16);
void erase_s(u16 *adrs, u16 page);
u16 *allocFlash(int length);
int getFlash(void  *addr, u16 val[], u16 len);
void packPtoP(Flash *pg1, Flash *pg2);
int erase0();
int erase1();

int ccAA = 0xAA;
int cc55 = 0x55;
int cc80 = 0x80;
int cc10 = 0x10;
int ccA0 = 0xA0;
int cc30 = 0x30;

Flash flashDescr;
Flash flashDescr1;
pFlash flashpoint;


//static char strBf[MSG_PACKET_LENGTH];
/****************************************************************************/
/*  int flash_write_w(u16 *dest, u16 page, u16 data)						*/
/*                                                                          */
/*  This function writes a word of data into flash memory. The addressing	*/
/*	is the same as in the function above					                */
/*                                                                          */
/*  Parameters:                                                             */
/*		u16 *dest															*/
/*		u16 page															*/
/*		u16 data															*/
/*                                                                          */
/*  Return:                                                                 */
/*  - OK success                                                            */
/*  - ERROR failure                                                         */
/*                                                                          */
/*  Notes:                                                                  */
/*                                                                          */
/****************************************************************************/
int flash_write_w(volatile u16 *dest, u16 page, u16 data)
{
	u16	sl;
	
	wrt_w(dest, page, data);
	
	sl = 0;
	
	while(read_w(dest,page) != data) 
	{	/* wait for completion */
		watchdog_feed_ram		//delay(0); //watchdog.
		sl++;
		if(sl == 0x0FFFF) {       		/* if it takes this long, that's*/
			return -1;
		}
	}
	return 0;						/* to write the word */
}


/* void wrt_w(u16 *dest, u16 page, u16 data) */
asm("	.global .data");

asm("_wrt_w	PSHM AR1");

asm("	FRAME -1");
asm("	NOP");
asm("	STL A, .data");

asm("	.global _ccAA, _cc55, _cc80, _cc10");
asm("	STM #_ccAA, AR1");
asm("	PORTW *AR1, 0555h");

asm("	STM #_cc55, AR1");
asm("	PORTW *AR1, 02AAh");

asm("	STM #_ccA0, AR1");
asm("	PORTW *AR1, 0555h");

asm("	MVDK .data,8h");
asm("	MVDK .data+4,9h");
asm("	WRITA 5h");

asm("	FRAME 1");
asm("	POPM AR1");
asm("	FRET");

/* void read_w(u16 *source, u16 page) */
asm("_read_w	PSHM AR1");
asm("	FRAME -1");
asm("	NOP");

asm("	MVDK	.data+4, 9h");
asm("	STM		#8h, AR1");
asm("	READA   *AR1");

asm("	FRAME 1");
asm("	POPM AR1");
asm("	FRET");

/* void erase_w(u16 *adrs, u16 page) */
asm("_erase_w	NOP");

asm("	PSHM AR1");
asm("	FRAME -1");
asm("	NOP");

asm("	STL A, .data");

asm("	STM #_ccAA, AR1");
asm("	PORTW *AR1, 0555h");

asm("	STM #_cc55, AR1");
asm("	PORTW *AR1, 02AAh");

asm("	STM #_cc80, AR1");
asm("	PORTW *AR1, 0555h");

asm("	STM #_ccAA, AR1");
asm("	PORTW *AR1, 0555h");

asm("	STM #_cc55, AR1");
asm("	PORTW *AR1, 02AAh");

asm("	STM #_cc30, AR1");

asm("	MVDK .data,8h");
asm("	MVDK .data+4,9h");

/*asm("	stm	#.data+4h, AR1");*/
asm("	WRITA *AR1");

asm("	FRAME 1");
asm("	POPM AR1");
asm("	FRET");

/*******************************************************************/
/*  int flash_read_w(u16 *source, u16 page)									*/
/*                                                                          */
/*  This function reads a word of data from flash memory. The addressing	*/
/*	is the same as in the functions above.									*/
/*                                                                          */
/*  Parameters:                                                             */
/*		u16 *source															*/
/*		u16 page															*/
/*                                                                          */
/*  Return:                                                                 */
/*  - data word from flash memory location                                  */
/*  - no documented errors                                                  */
/*                                                                          */
/*  Notes:                                                                  */
/*                                                                          */
/****************************************************************************/
u16 flash_read_w(u16 *source, u16 page)
{										/*using a unified linear flash address that is*/
										/*offset from the beginning of the flash*/
	watchdog_feed_ram		//delay(0); //watchdog.
	return read_w(source, page);
}
/*******************************************************************/

int EraseParamPage(u16 *addr)
{
    u16 sl;
    volatile unsigned long	sl0,s11;

#ifdef _SRAM_  
	erase_w(addr, 0xB);
#else
	erase_w(addr, 0x3);
#endif

	for(sl0 = 0L; sl0 != 0x001FFFFL; sl0++)
	{ 		
		watchdog_feed_ram		//delay(0); //watchdog.
	}
    
#ifdef _SRAM_
	while(read_w(addr,0xB) != 0x0FFFF)
#else
	while(read_w(addr,0x3) != 0x0FFFF)
#endif 
	{             	/*wait for completion*/
		watchdog_feed_ram		//delay(0); //watchdog.
		sl++;
		if(sl == 0x0FFFF) 
		{ 					      		/*if it takes this long, that's*/
			return -1;
		}
        for(sl0 = 0L; sl0 != 0x001FFFFL; sl0++)
        { 
        	watchdog_feed_ram		//delay(0); //watchdog.
        }
	}
	return 0;
}
/*******************************************************************/

u16 ReadParam_w(u16 *addr)
{
#ifdef _SRAM_
	return (flash_read_w(addr, 0xB));
#else
	return (flash_read_w(addr, 0x3));
#endif
}
/*******************************************************************/

u16 WriteParam_w(u16 *addr, u16 word)
{
#ifdef _SRAM_
	return (flash_write_w(addr, 0xB, word));
#else 
	return (flash_write_w(addr, 0x3, word));
#endif
}


/*******************************************************************/
/* Erase_Param - erase all in parameter flash & set empty parametr */
/*				space & open it for Store-Restore functions        */
/*		Store-Restore function may be called after Erase_Param	   */
/*         without Open_Param, but flash will be empty.			   */
/*******************************************************************/
int Erase_Param()
{
	watchdog_feed_ram
	if(0 != erase0())
	  return -1;
	watchdog_feed_ram
/*
	if(0 != erase1())
	  return -1;
*/
    flashpoint = &flashDescr;
 	return 0;
}

int erase0()
{
	flashDescr.begin = (u16 *)0x4000;
/*	flashDescr.end   = (u16 *)0x7FFF;*/
	flashDescr.end   = (u16 *)0x44FF;
	flashDescr.first = (u16 *)0;
	
	return EraseParamPage((u16 *)0x4001);

}

int erase1()
{
	flashDescr1.begin = (u16 *)0x8000;
	flashDescr1.end   = (u16 *)0xBFFF;
	flashDescr1.first = (u16 *)0;
    return EraseParamPage((u16 *)0x8001);
}

/*******************************************************************/

/*   SaveParam - save named parameter in flash */
/*  return:
		     0 - Ok,
		    -1 - No space...
		    -2 - flash destroed...  */

int StoreParam( char name[], void *aParam, int lParam)
{
	FlRec newRec;
	int *pParam;
	int i;
	
	pParam = aParam;
	
	if((ReadParam_w(flashpoint->begin) != 0xffff) || 
		(ReadParam_w(flashpoint->begin + 1) != 0xffff) || 
		(ReadParam_w(flashpoint->begin + 2) != 0xffff) ||
		(ReadParam_w(flashpoint->begin + 3) != 0xffff))
			return -1;	//No space
			
	//packParam(&flashDescr, &flashDescr1);
		
	newRec.lname = strlen(name)+1;
	newRec.lvalue = lParam;
	
	newRec.name = allocFlash(newRec.lname);
	if (newRec.name == 0) return -1; //No space
	
	newRec.value = allocFlash(newRec.lvalue);
	if (newRec.value == 0) return -1; //No space
	
	for ( i = 0; i < newRec.lname; i++) 
	{
		if(WriteParam_w(newRec.name + i, name[i]) != 0) return -2;
	}

	for ( i = 0; i < newRec.lvalue; i++) 
	{
		if(WriteParam_w(newRec.value + i, pParam[i]) != 0) return -2;
	}
	
	if(WriteParam_w(flashpoint->begin, (int) newRec.name) != 0) return -3;
	if(WriteParam_w(flashpoint->begin + 1, newRec.lname) != 0) return -3;
	if(WriteParam_w(flashpoint->begin + 2, (int) newRec.value) != 0) return -3;
	if(WriteParam_w(flashpoint->begin + 3, newRec.lvalue) != 0) return -3;
	
	flashpoint->first = flashpoint->begin;
	
	flashpoint->begin = flashpoint->begin + 4;
	return 0;
}
/*******************************************************************/
/* RestoreParam - ......    */
/* return:
			 0 - Ok,
			-2 - flash destroed...
			-3 - Parameter not found... */
			
int RestoreParam( char name[], void *addrParam)
{
	union
	{
		FlRec record;
		u16 irec[4];
	} rec;
	u16 *pointer;

	union{
		char parName[256];
		u16 iname[256];
	} Name;
	
	pointer = flashpoint->first;
	if((pointer == (u16 *)0x0000) ||(pointer == (u16 *)0xffff)) return -3;
	while (1)
	{
		getFlash(pointer, rec.irec, 4);
		if(rec.irec[0] == 0xffff) return -3;	//Not found if 0xffff
		getFlash(rec.record.name, Name.iname, rec.record.lname);
		if (strcmp(name, Name.parName) == 0) break;
	    pointer = pointer -4;
	    if (flashpoint == &flashDescr) {if (pointer < (u16 *)0x4000) return -3;}
	    else { if (pointer < (u16*)0x8000) return -3;}
	}
	// Parameter found.
	getFlash(rec.record.value, addrParam, rec.record.lvalue);	
	return 0;
}

/* allocFlash - allocate space from flash bottom */
/* return:										 */
/*			 0 - no space						 */
/*         <>0 - pointer to allocated space	     */

u16 *allocFlash(int length)
{
	flashpoint->end = flashpoint->end - length;
	return (flashpoint->end +1);
}
/*******************************************************************/

int getFlash(void  *addr, u16 val[], u16 len)
{
	int i;
	u16 *point;
	point = addr;
	
	for (i = 0; i < len; i++)
	{
		val[i] = ReadParam_w(point);
		point++;
	}                   
	return 0;
	
}

void packPtoP(Flash *pg1, Flash *pg2)
{
	union
	{
		FlRec record;
		u16 irec[4];
	} rec;
	u16 *pointer;
	u16 limit;

	union{
		char parName[256];
		u16 iname[256];
	} Name;
	
	u16 parvalue[256];

	flashpoint = pg1;
	limit = ((u16 )flashpoint ->first) & 0xC000;
	for (pointer = flashpoint->first; pointer >= (u16 *)limit; pointer = pointer -4)
	{
		getFlash(pointer, rec.irec, 4);	
		if(rec.irec[0] == 0xffff) break;	//No parameters if 0xffff
		getFlash(rec.record.name, Name.iname, rec.record.lname);

		flashpoint = pg2;
		if (RestoreParam( Name.parName, &parvalue) == -3)
		{
			flashpoint = pg1;
			RestoreParam( Name.parName, &parvalue);
			flashpoint = pg2;
			StoreParam( Name.parName, &parvalue, rec.record.lvalue);
		}
		flashpoint = pg1;
				
	}
	
}




