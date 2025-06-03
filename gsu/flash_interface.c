#include "su.h"

#define StartFlashAddr			0x4000

FLASH_PARAMS FlashRecords[MAX_RECORDS];
FLASH_PARAMS LastNumber, LastCall, AllCalls;

static BOOLEAN bDelayedStore = FALSE;

/*****************************************************
InitFlash Params
	Initializes the flash parameter table in
	flash to the default values.
	The parameter table has the following structure:
	Name[11]		Data[21]
*****************************************************/
void InitFlashParams(void)
{
  /*Default settings */
  memset(FlashRecords,0,sizeof(FlashRecords));
  strcpy(FlashRecords[0].NameParam, "VERSION");
  strcpy(FlashRecords[0].DataParam, VersionNumber);
  strcpy(FlashRecords[1].NameParam, "PIN");
  strcpy(FlashRecords[1].DataParam, "1234");
  strcpy(FlashRecords[2].NameParam, "AUTOPIN");
  strcpy(FlashRecords[2].DataParam, "0");
  strcpy(FlashRecords[3].NameParam, "SMSONCID");
  strcpy(FlashRecords[3].DataParam, "0");
  strcpy(FlashRecords[4].NameParam, "DATABEARER");
  strcpy(FlashRecords[4].DataParam, "AT+CBST=0,0,2\r\n");
  strcpy(FlashRecords[5].NameParam, "AUTOANSWER");
  strcpy(FlashRecords[5].DataParam, "ATS0=0\r\n");
  strcpy(FlashRecords[6].NameParam, "FRAMING");
  strcpy(FlashRecords[6].DataParam, "AT+ICF=3,4\r\n");
  strcpy(FlashRecords[7].NameParam, "BAUDRATE");
  strcpy(FlashRecords[7].DataParam, "AT+IPR=0\r\n");
  strcpy(FlashRecords[8].NameParam, "CCM");
  strcpy(FlashRecords[8].DataParam, "0");
  strcpy(FlashRecords[9].NameParam, "LASTNUMBER");
  strcpy(FlashRecords[9].DataParam, "");
  strcpy(FlashRecords[10].NameParam, "LASTCALL");
  strcpy(FlashRecords[10].DataParam, "CCM:   00:00:00");
  strcpy(FlashRecords[11].NameParam, "ALLCALLS");
  strcpy(FlashRecords[11].DataParam, "0");
  strcpy(FlashRecords[12].NameParam, "CIDMODTYPE");
  strcpy(FlashRecords[12].DataParam, "0");
  strcpy(FlashRecords[13].NameParam, "POWERSAVE");
  strcpy(FlashRecords[13].DataParam, "0");
  strcpy(FlashRecords[14].NameParam, "CHARGING");
  strcpy(FlashRecords[14].DataParam, "0");
  strcpy(FlashRecords[15].NameParam, "CHARGPARMS");
  strcpy(FlashRecords[15].DataParam, "1");
  strcpy(FlashRecords[16].NameParam, "CONNSUPV");
  strcpy(FlashRecords[16].DataParam, "");
  strcpy(FlashRecords[17].NameParam, "DISCONSUPV");
  strcpy(FlashRecords[17].DataParam, "");
  strcpy(FlashRecords[18].NameParam, "AOCSUPV");
  strcpy(FlashRecords[18].DataParam, "");
  strcpy(FlashRecords[19].NameParam, "VOLUME");
  strcpy(FlashRecords[19].DataParam, "12000060808000");

/*	// conn=rev pulse; disconn=rev pulse; aoc=rev pulses  
  strcpy(FlashRecords[16].NameParam, "CONNSUPV");
  strcpy(FlashRecords[16].DataParam, "400AA00A80000000");
  strcpy(FlashRecords[17].NameParam, "DISCONSUPV");
  strcpy(FlashRecords[17].DataParam, "400AA00A80000000");
  strcpy(FlashRecords[18].NameParam, "AOCSUPV");
  strcpy(FlashRecords[18].DataParam, "A019401980190000");
*/
/*	// conn=hard rev; disconn=hard rev norm; aoc=16kHz metering
  strcpy(FlashRecords[16].NameParam, "CONNSUPV");
  strcpy(FlashRecords[16].DataParam, "4032E0000000");
  strcpy(FlashRecords[17].NameParam, "DISCONSUPV");
  strcpy(FlashRecords[17].DataParam, "4050C0000000");
  strcpy(FlashRecords[18].NameParam, "AOCSUPV");
  strcpy(FlashRecords[18].DataParam, "9000400580000000");
*/
#ifndef _NO_FLASH_ 
  EraseRecordRegion();
  WriteRecord();
#endif    
}

/*****************************************************
EraseRecordRegion
	Erases the entire fourth page of flash which is
	used for the parameter table.  Address ranges 
	from 0x34000 - 0x37FFF and 0z38000 - 0x3FFFF.
*****************************************************/
int EraseRecordRegion(void)
{
  static int Status;

  INTR_GLOBAL_DISABLE;
  Status = Erase_Param();
  INTR_GLOBAL_ENABLE;
  
  return Status;
}

/*****************************************************
WriteRecord
	Writes the entire parameter table into flash
	from the static structure "FlashRecords" one 
	record at a time.
*****************************************************/
int WriteRecord(void)
{
  static int Status, i;
  static WORD Addr;
  WORD *pRecords;
  
  pRecords = (WORD *)&FlashRecords;
  Addr = StartFlashAddr;
  
  INTR_GLOBAL_DISABLE;
  for(i = 0; i < sizeof(FlashRecords); i++)
  {
    if(0 != (Status = WriteParam_w(Addr++, *(pRecords+i))))
    {
      // DebugPrintf("Record NOT saved. \r\n");
      break;
    }
  }
  INTR_GLOBAL_ENABLE;
  
  return Status;
}

/*****************************************************
RetrieveRecord
	Copies the parameter table from flash into the 
	static structure "FlashRecords" one record at a
	time.
*****************************************************/
int RetrieveRecord(void)
{
  static int i;
  static WORD Addr;
  WORD *pRecords;
  
  memset(FlashRecords,0,sizeof(FlashRecords));
  pRecords = (WORD *)&FlashRecords;
  
  Addr = StartFlashAddr;
  
  INTR_GLOBAL_DISABLE;
  for(i = 0; i < sizeof(FlashRecords); i++)
  {    
    *(pRecords+i) = ReadParam_w(Addr++);
  }
  INTR_GLOBAL_ENABLE;
 
  return 0;
}

/*****************************************************
ReadFlashData
	Copies the entire parameter table from flash into
	the static structure "FlashRecords", and searches
	for the matching Name parameter.  If found
	returns the data parameter associated with the Name 
	parameter used in the search, otherwise returns 
	NULL.
*****************************************************/
char *ReadFlashData(char *Name)
{
#ifndef _NO_FLASH_
  int i;
  
  RetrieveRecord();
  for(i = 0; i < MAX_RECORDS; i++)
  {
    if(0 == strcmp(Name, FlashRecords[i].NameParam))
      return FlashRecords[i].DataParam;
  }
#endif
  return NULL;
}

/*****************************************************
UpdateFlashData
	Copies the entire parameter table from flash into
	the static structure "FlashRecords", and searches
	for the matching Name parameter.  If found updates
	the data parameter for that member only erases the
	entire parameter table region and writes update 
	table back into flash.
*****************************************************/
int UpdateFlashData(char *Name, char *Data)
{
  int Status = -1;
#ifndef _NO_FLASH_  
  int i;
  
  if((strlen(Name) > (PARAM_NAME_SZ-1)) || (strlen(Data) > (PARAM_DATA_SZ-1)))
  	return Status;
  
  RetrieveRecord();
  for(i = 0; i < MAX_RECORDS; i++)
  {
    if(0 == strcmp(Name, FlashRecords[i].NameParam))
    {
      memset(FlashRecords[i].DataParam,0, PARAM_DATA_SZ);
      strcpy(FlashRecords[i].DataParam, Data);
      if(0 == EraseRecordRegion())
      {
		Status = 0;
      	if(0 == WriteRecord())
      		Status = 0;
      }
      break;
    }
  }
#endif
  return Status;
}

/*****************************************************
ReadFlashDataImage
	Searches the static structure "FlashRecords"
	for the matching Name parameter.  If found
	returns the data parameter associated with the Name 
	parameter used in the search, otherwise returns 
	NULL.
*****************************************************/
char *ReadFlashDataImage(char *Name)
{
  int i;
  
  for(i = 0; i < MAX_RECORDS; i++)
  {
    if(0 == strcmp(Name, FlashRecords[i].NameParam))
      return FlashRecords[i].DataParam;
  }
  return NULL;
}

/*****************************************************
RetainDelayedParams
	Retains the call metering parameters in SRAM during 
	a call. Can be updated during the call until there 
	is time to store into flash.
*****************************************************/
int RetainDelayedParams(char *Name, char *Data)
{
  int Status = 0;

  bDelayedStore = TRUE;   
  if(!strcmp(Name, "LASTNUMBER"))
  {
    strcpy(LastNumber.NameParam, Name);
    strcpy(LastNumber.DataParam, Data);
  }
  else if(!strcmp(Name, "LASTCALL"))
  {
    strcpy(LastCall.NameParam, Name);
    strcpy(LastCall.DataParam, Data);
  }
  else if(!strcmp(Name, "ALLCALLS"))
  {
    strcpy(AllCalls.NameParam, Name);
    strcpy(AllCalls.DataParam, Data);
  }
  else
  {
    bDelayedStore = FALSE;
    Status = 1;
  }
   

  return Status;
}

int ReadAllCallData(void)
{
  int Status;
  
  Status = RetainDelayedParams("ALLCALLS", ReadFlashData("ALLCALLS"));
  bDelayedStore = FALSE;
  
  return Status;
}

char *GetAllCallData(char *Name)
{
  if(!strcmp(Name, "ALLCALLS"))
  {
    return AllCalls.DataParam;
  }
  else
    return NULL;
}

/*****************************************************
StoreDelayedParams
	Stores the latest delay call metering parameters 
	into flash following call termination (hangup).
*****************************************************/
int StoreDelayedParams(void)
{
  int Status = 1;
  int i;

  if( bDelayedStore == TRUE)
  {
    RetrieveRecord();
    for(i = 0; i < MAX_RECORDS; i++)
    {
      if(0 == strcmp("LASTNUMBER", FlashRecords[i].NameParam))
      {
        memset(FlashRecords[i].DataParam,0, PARAM_DATA_SZ);
        strcpy(FlashRecords[i].DataParam, LastNumber.DataParam);
      }
      if(0 == strcmp("LASTCALL", FlashRecords[i].NameParam))
      {
        memset(FlashRecords[i].DataParam,0, PARAM_DATA_SZ);
        strcpy(FlashRecords[i].DataParam, LastCall.DataParam);
      }
      if(0 == strcmp("ALLCALLS", FlashRecords[i].NameParam))
      {
        memset(FlashRecords[i].DataParam,0, PARAM_DATA_SZ);
        strcpy(FlashRecords[i].DataParam, AllCalls.DataParam);
      }
    }
    if(0 == EraseRecordRegion())
    {
      if(0 == WriteRecord())
        Status = 0;
    }
  }

  bDelayedStore = FALSE;
  return Status;
}

void ClearDelayedParams(void)
{
  memset(LastNumber.NameParam,0, PARAM_NAME_SZ);
  memset(LastNumber.DataParam,0, PARAM_DATA_SZ);
  memset(LastCall.NameParam,0, PARAM_NAME_SZ);
  memset(LastCall.DataParam,0, PARAM_DATA_SZ);
  memset(AllCalls.NameParam,0, PARAM_NAME_SZ);
  memset(AllCalls.DataParam,0, PARAM_DATA_SZ);
  
}

int ForcedPowerSaving(void)
{
	char *flashdata;
	
	flashdata=ReadFlashDataImage("POWERSAVE");
	
 	if(flashdata != NULL)
	{
		return ((int)flashdata[0] - 0x30);
	}
	
	return 0;
}


