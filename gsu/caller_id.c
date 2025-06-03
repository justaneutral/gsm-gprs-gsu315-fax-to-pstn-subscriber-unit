#include "su.h"

#undef _DEBUG_FSK_

int sinus[256] =
{
0x0324,0x0647,0x096A,0x0C8B,0x0FAB,0x12C7,0x15E1,0x18F8,
0x1C0B,0x1F19,0x2223,0x2527,0x2826,0x2B1E,0x2E10,0x30FB,
0x33DE,0x36B9,0x398C,0x3C56,0x3F16,0x41CD,0x447A,0x471C,
0x49B3,0x4C3F,0x4EBF,0x5133,0x539A,0x55F4,0x5842,0x5A81,
0x5CB3,0x5ED6,0x60EB,0x62F1,0x64E7,0x66CE,0x68A5,0x6A6C,
0x6C23,0x6DC9,0x6F5E,0x70E1,0x7254,0x73B5,0x7503,0x7640,
0x776B,0x7883,0x7989,0x7A7C,0x7B5C,0x7C29,0x7CE2,0x7D89,
0x7E1C,0x7E9C,0x7F08,0x7F61,0x7FA6,0x7FD7,0x7FF5,0x7FFE,
0x7FF5,0x7FD7,0x7FA6,0x7F61,0x7F08,0x7E9C,0x7E1C,0x7D89,
0x7CE2,0x7C29,0x7B5C,0x7A7C,0x7989,0x7883,0x776B,0x7640,
0x7503,0x73B5,0x7254,0x70E1,0x6F5E,0x6DC9,0x6C23,0x6A6C,
0x68A5,0x66CE,0x64E7,0x62F1,0x60EB,0x5ED6,0x5CB3,0x5A81,
0x5842,0x55F5,0x539A,0x5133,0x4EBF,0x4C3F,0x49B3,0x471C,
0x447A,0x41CD,0x3F16,0x3C56,0x398C,0x36B9,0x33DE,0x30FB,
0x2E10,0x2B1E,0x2826,0x2527,0x2223,0x1F19,0x1C0B,0x18F8,
0x15E1,0x12C7,0x0FAB,0x0C8B,0x096A,0x0647,0x0324,0x0000,
0xFCDC,0xF9B9,0xF696,0xF375,0xF056,0xED39,0xEA1F,0xE708,
0xE3F5,0xE0E7,0xDDDD,0xDAD9,0xD7DA,0xD4E2,0xD1F0,0xCF05,
0xCC22,0xC947,0xC674,0xC3AA,0xC0EA,0xBE33,0xBB86,0xB8E4,
0xB64D,0xB3C1,0xB141,0xAECD,0xAC66,0xAA0C,0xA7BE,0xA57F,
0xA34D,0xA12A,0x9F15,0x9D0F,0x9B19,0x9932,0x975B,0x9594,
0x93DD,0x9237,0x90A2,0x8F1F,0x8DAC,0x8C4C,0x8AFD,0x89C0,
0x8895,0x877D,0x8677,0x8584,0x84A4,0x83D8,0x831E,0x8277,
0x81E4,0x8164,0x80F8,0x809F,0x805A,0x8029,0x800B,0x8002,
0x800B,0x8029,0x805A,0x809F,0x80F8,0x8164,0x81E4,0x8277,
0x831E,0x83D7,0x84A4,0x8584,0x8677,0x877D,0x8895,0x89C0,
0x8AFD,0x8C4B,0x8DAC,0x8F1F,0x90A2,0x9237,0x93DD,0x9594,
0x975B,0x9932,0x9B19,0x9D0F,0x9F15,0xA12A,0xA34D,0xA57F,
0xA7BE,0xAA0B,0xAC66,0xAECD,0xB141,0xB3C1,0xB64D,0xB8E4,
0xBB86,0xBE33,0xC0EA,0xC3AA,0xC674,0xC947,0xCC22,0xCF05,
0xD1F0,0xD4E1,0xD7DA,0xDAD9,0xDDDD,0xE0E7,0xE3F5,0xE708,
0xEA1E,0xED38,0xF055,0xF375,0xF696,0xF9B9,0xFCDC,0x0000
};

volatile ioport unsigned int VOICE_DATA_SWITCH; 
int into_caller_id = 0;

typedef unsigned int CID_TYPE;
enum CID_TYPE {NO_TYPE, CID_NAME, CID_NUMBER, CID_NAME_NUMBER, CID_INDICATION};

#define DATE_TIME_SZ			 9
#define NAME_SZ					51
#define NUMBER_SZ				21

typedef struct {
  CID_TYPE Type;
  BYTE MsgType;
  BYTE MsgLen;
  char DateTime[DATE_TIME_SZ];
  BYTE Param1Type;
  BYTE Param1Len;
  char Name[NAME_SZ];
  BYTE Param2Type;
  BYTE Param2Len;
  char Number[NUMBER_SZ];
  BYTE Param3Type;
  BYTE Param3Len;
  BYTE CkSum;
} CID_DATA;
static CID_DATA CID_MsgData;

/* external declarations */
extern int bsp0_tx_flag;
state_service_func off_timer(int timer_index);


char GetCidModulation(void)
{
  char *type;
  
  type = ReadFlashDataImage("CIDMODTYPE");
  return *type;
}

void SendFSK(int bit)
{
	int i,j;
	float MarkOffset, SpaceOffset;
	static int c3=-1;
	static float phase = 0.0;

	switch(GetCidModulation())
	{	
		case '0':	// ETSI_V23
		  MarkOffset = 41.6;
		  SpaceOffset = 67.2;
		break;
		
		case '1':	// BELL_202
		  MarkOffset = 38.4;
		  SpaceOffset = 70.4;
		break;
	}
	
	if((c3++)>1)
		c3=-1;
	
	j=(c3)?7:6;	
	
	for(i=0;i<j;i++)
	{
		if(bit)
			phase += MarkOffset;
    	else
    		phase += SpaceOffset;

		if(phase > 255.0)
			phase -= 255.0;
		
		while(!bsp0_tx_flag);
		bsp0_tx_flag = 0;
		
		DXR10 = (int)((sinus[(unsigned int)(phase)])/5);
	}
}

void MakeSymbol(int Data)
{
	unsigned int mask;

	SendFSK(0);
	for(mask = 0x0001; mask != 0x0100; mask<<=1)
		SendFSK(mask & Data);
	SendFSK(1);
}

void Send_CID(void)
{
//  char strBuf[50];
  
  switch(CID_MsgData.Type)
  {
    case CID_NAME:
    {
      CID_MsgData.MsgType = 0x80;
      TX_NameData();
    }
    break;
    case CID_NUMBER:
    {
      CID_MsgData.MsgType = 0x80;
      TX_NumberData();
    }
    break;
    case CID_NAME_NUMBER:
    {
/* Debug Stuff          
      sprintf(strBuf, "Send_CID() Time: %s Name: %s Number: %s \r\n", CID_MsgData.DateTime, CID_MsgData.Name, CID_MsgData.Number);
      DebugPrintf(strBuf); */
      
        CID_MsgData.MsgType = 0x80;
        TX_NameNumberData();
    }
    break;
    case CID_INDICATION:
    {
#if 0
      CID_MsgData.MsgType = 0x80;
//		future implementation
      CID_MsgData.Param1Len = 0;   
//      strcpy(CID_MsgData.DateTime, "01070015");
//      CID_MsgData.Param1Type = 0x01;
//      CID_MsgData.Param1Len = strlen(MMDF.DateTime);

      CID_MsgData.Param2Type = 0x0B;				// Indicator display uses name member
      if(0 == strcmp(msg, "on"))					// for convenience only
      {
        CID_MsgData.Name[0] = 0xFF;
        CID_MsgData.Name[1] = '\0';
        CID_MsgData.Param2Len = 0x01;
        DebugPrintf("caller id indicator ON.\r\n");
      }
      if(0 == strcmp(msg, "off"))
      {
        CID_MsgData.Name[0] = 0x00;
        CID_MsgData.Name[1] = '\0';
        CID_MsgData.Param2Len = 0x01;
        DebugPrintf("caller id indicator OFF.\r\n");
      }
#endif
      DebugPrintf("CID display update sent. \r\n");
      
      CID_MsgData.MsgType = 0x80;
      TX_MessageIndication();
    }
    break;
    
    default:
      DebugPrintf("No Msg Type. \r\n");
  } // end switch
  
  ClearCidParams();
}

void caller_id(void)
{ 
  if(0 == CID_MsgData.Type)
    CID_Name_Number("", "", "");
    
  if('2' == GetCidModulation())	// ETSI_DTMF
  {
    TX_DTMF_Number();
  }
  else						// ETSI_V23 or ETSI_DTMF FSK
  {
    into_caller_id = 1;
  
    if(WSU003_Config)
      VOICE_DATA_SWITCH=1;
    else
      DataModeSwitch();

    WriteSingleByte(WRITE_ACTIVATE_OP_MODE);

    Send_CID();
  
    if(WSU003_Config)
      VOICE_DATA_SWITCH=0;
    else
      VoiceModeSwitch();

    into_caller_id = 0;
  }
  DebugPrintf("caller_id called\r\n");
}
void CID_MessageIndication(void)
{
  static BYTE XOR_bit = 0;
  char strS[10];
  
  if(XOR_bit)
    strcpy(strS, "on");
  else
    strcpy(strS, "off");
    
  into_caller_id = 1;
  
  if(WSU003_Config)
    VOICE_DATA_SWITCH=1;
  else
    DataModeSwitch();

  WriteSingleByte(WRITE_ACTIVATE_OP_MODE);

//  Send_CID(strS,CID_INDICATION);
  
  XOR_bit ^= 1;
  
  if(WSU003_Config)
    VOICE_DATA_SWITCH=0;
  else
    VoiceModeSwitch();

  into_caller_id = 0;
}

void CID_OnhookPrintf(char *txtmsg, char *number)
{
  int SlacState;
  
  if(OnHookStatus())
  {  
  	SlacState = GetCurrentSlacState();
  	if((ring0 == SlacState) || (ring1 == SlacState))
   	  return; 
   	
   	if(number[0] == '\0')
   		CID_Name(txtmsg);
    else
    	CID_Name_Number(txtmsg, number,"");
    	
   	send0(SLAC,SLAC_cid_messages);
  }
}

void CID_OffhookPrintf(void)
{
  int SlacState;
  
  if(OnHookStatus())
  {  
  	SlacState = GetCurrentSlacState();
  	if((ring0 == SlacState) || (ring1 == SlacState))
   	  return; //RSSI not sent!
  
   	//send0(SLAC,SLAC_cid_messages);
   	send1_delayed(200, SLAC, SLAC_cid_messages,-1,NULL);
  }
}

void CID_SendMessage(void)
{
  into_caller_id = 1;

  if(WSU003_Config)
    VOICE_DATA_SWITCH=1;
  else
    DataModeSwitch();

  WriteSingleByte(WRITE_ACTIVATE_OP_MODE);

  Send_CID();

  if(WSU003_Config)
    VOICE_DATA_SWITCH=0;
  else
    VoiceModeSwitch();
  
  into_caller_id = 0;
}

state_service_func cw_caller_id(void *p)
{    
//  off_timer(0); // SLAC_TIMER_CADENCE; turn off mute codec delay timer
  if(0 == CID_MsgData.Type)
    CID_Name_Number("", "", "");
    
  if('2' == GetCidModulation())	// ETSI_DTMF
  {
    //TX_DTMF_Number();
    return 0;
  }
  else						// ETSI_V23 or ETSI_DTMF FSK
  {
    into_caller_id = 1;

    if(WSU003_Config)
      VOICE_DATA_SWITCH=1;
    else
      DataModeSwitch();

    Send_CID();

    if(WSU003_Config)
      VOICE_DATA_SWITCH=0;
    else
      VoiceModeSwitch();

    into_caller_id = 0;
  }
  DebugPrintf("cw caller id called\r\n");
  return 0;
}

/**************************************************
Caller ID API calls
**************************************************/
void CID_Name_Number(char *pName, char *pNumber, char *pDateTime)
{  
  ClearCidParams();
  CID_MsgData.Type = CID_NAME_NUMBER;

  if(strlen(pDateTime) > (DATE_TIME_SZ -1))
    pDateTime[0] = '\0';
  
  strcpy(CID_MsgData.DateTime ,pDateTime);
  CID_MsgData.Param1Len = strlen(CID_MsgData.DateTime);
  if(0 != CID_MsgData.Param1Len)
  {
    CID_MsgData.Param1Type = 0x01;
  }
  
  if(strlen(pName) > (NAME_SZ-1) )
  	pName[NAME_SZ] = '\0';
  	
  CID_MsgData.Param2Type = 0x08;
  strcpy(CID_MsgData.Name ,pName);
  CID_MsgData.Param2Len = strlen(CID_MsgData.Name);
  if(0 == CID_MsgData.Param2Len)
  {
    CID_MsgData.Param2Type = 0x08;
    CID_MsgData.Name[0] = 0x4F;
    CID_MsgData.Name[1] = '\0';
    CID_MsgData.Param2Len = strlen(CID_MsgData.Name);
  }
  else
    CID_MsgData.Param2Type = 0x07;
  
  if(strlen(pNumber) > (NUMBER_SZ-1))
  	pNumber[NUMBER_SZ] = '\0';
  
  strcpy(CID_MsgData.Number ,pNumber);
  CID_MsgData.Param3Len = strlen(CID_MsgData.Number);
  if(0 == CID_MsgData.Param3Len)
  {
    CID_MsgData.Param3Type = 0x04;
    CID_MsgData.Number[0] = 0x4F;
    CID_MsgData.Number[1] = '\0';
    CID_MsgData.Param3Len = strlen(CID_MsgData.Number);
  }
  else
    CID_MsgData.Param3Type = 0x02;
/* Debug Stuff  
  sprintf(strBuf, "Time: %s Name: %s Number: %s \r\n", CID_MsgData.DateTime, CID_MsgData.Name, CID_MsgData.Number);
  DebugPrintf(strBuf);
  sprintf(strBuf, "Psize1: %d Psixe2: %d Psize3: %d \r\n", CID_MsgData.Param1Len, CID_MsgData.Param2Len, CID_MsgData.Param3Len);
  DebugPrintf(strBuf); */
}

void CID_Name(char *pName)
{
  ClearCidParams();
  CID_MsgData.Type = CID_NAME;
  
  if(strlen(pName) > (NAME_SZ-1) )
  	pName[NAME_SZ] = '\0';
  		
  strcpy(CID_MsgData.Name ,pName);
  CID_MsgData.Param2Len = strlen(CID_MsgData.Name);
  if(0 == CID_MsgData.Param2Len)
  {
    CID_MsgData.Param2Type = 0x08;
    CID_MsgData.Name[0] = 0x4F;
    CID_MsgData.Name[1] = '\0';
  }
  else
    CID_MsgData.Param2Type = 0x07;
}

void CID_Number(char *pNumber)
{
  ClearCidParams();
  CID_MsgData.Type = CID_NUMBER;
  
  if(strlen(pNumber) > (NUMBER_SZ-1))
  	pNumber[NUMBER_SZ] = '\0';
  
  strcpy(CID_MsgData.Number ,pNumber);
  CID_MsgData.Param3Len = strlen(CID_MsgData.Number);
  if(0 == CID_MsgData.Param3Len)
  {
    CID_MsgData.Param3Type = 0x04;
    CID_MsgData.Number[0] = 0x4F;
    CID_MsgData.Number[1] = '\0';
    CID_MsgData.Param3Len = strlen(CID_MsgData.Number);
  }
  else
    CID_MsgData.Param3Type = 0x02;
}

/**************************************************
Low level Caller ID transmission driver
**************************************************/
void TX_DTMF_Number(void)
{
  int i;
  
  clear_dialing_parameters(0);
  if('O' == CID_MsgData.Number[0])
  {
    DialingParams.DialingParams[DialingParams.ndx++] = 'B';
    DialingParams.DialingParams[DialingParams.ndx++] = '0';
    DialingParams.DialingParams[DialingParams.ndx++] = '0';
    DialingParams.DialingParams[DialingParams.ndx++] = 'C';
  }
  else
  {
    DialingParams.DialingParams[DialingParams.ndx++] = 'A';
    for(i = 0; i < CID_MsgData.Param3Len; i++)   // Number Only
      DialingParams.DialingParams[DialingParams.ndx++] = CID_MsgData.Number[i];
    DialingParams.DialingParams[DialingParams.ndx++] = 'C';
  }
  send0(SLAC, SLAC_send_dtmf_cid);
}

void TX_NameData(void)
{
  int i;
 
  CID_MsgData.MsgLen = 2 +  CID_MsgData.Param2Len;
  
  CID_MsgData.CkSum = 0;

  CID_MsgData.CkSum = CID_MsgData.MsgType + CID_MsgData.MsgLen
     + CID_MsgData.Param2Type + CID_MsgData.Param2Len;
  for(i = 0; i < CID_MsgData.Param2Len; i++)
    CID_MsgData.CkSum += CID_MsgData.Name[i];

  CID_MsgData.CkSum = (~(CID_MsgData.CkSum % 256)+1) & 0xff;
  
  bsp0_tx_flag=0;
 
  ActivateBsp0(0);
    
  for(i=0; i<300; i++)
    SendFSK(i & 0x0001);

  for(i=0; i<180; i++)
    SendFSK(1);

  MakeSymbol(CID_MsgData.MsgType);
  MakeSymbol(CID_MsgData.MsgLen);
  
  MakeSymbol(CID_MsgData.Param2Type);
  MakeSymbol(CID_MsgData.Param2Len);
  for(i = 0; i < CID_MsgData.Param2Len; i++)
    MakeSymbol(CID_MsgData.Name[i]);

  MakeSymbol(CID_MsgData.CkSum);

  for(i=0;i<30;i++)
    SendFSK(1);
  
  DeactiveBsp0();
}

void TX_NumberData(void)
{
  int i;

  CID_MsgData.MsgLen = 4 + CID_MsgData.Param2Len + CID_MsgData.Param3Len;
  
  CID_MsgData.CkSum = 0;

  CID_MsgData.CkSum = CID_MsgData.MsgType + CID_MsgData.MsgLen
     + CID_MsgData.Param2Type + CID_MsgData.Param2Len
     + CID_MsgData.Param3Type + CID_MsgData.Param3Len;
       
  for(i = 0; i < CID_MsgData.Param2Len; i++)   // Name
    CID_MsgData.CkSum += CID_MsgData.Name[i];
  for(i = 0; i < CID_MsgData.Param3Len; i++)   // Number
    CID_MsgData.CkSum += CID_MsgData.Number[i];

  CID_MsgData.CkSum = (~(CID_MsgData.CkSum % 256)+1) & 0xff;
  
  bsp0_tx_flag=0;
 
  ActivateBsp0(0);
    
  for(i=0; i<300; i++)
    SendFSK(i & 0x0001);

  for(i=0; i<180; i++)
    SendFSK(1);

  MakeSymbol(CID_MsgData.MsgType);
  MakeSymbol(CID_MsgData.MsgLen);
  
  MakeSymbol(CID_MsgData.Param2Type);
  MakeSymbol(CID_MsgData.Param2Len);
  for(i = 0; i < CID_MsgData.Param2Len; i++)
    MakeSymbol(CID_MsgData.Name[i]);
    
  MakeSymbol(CID_MsgData.Param3Type);
  MakeSymbol(CID_MsgData.Param3Len);
  for(i = 0; i < CID_MsgData.Param3Len; i++)
    MakeSymbol(CID_MsgData.Number[i]);

  MakeSymbol(CID_MsgData.CkSum);

  for(i=0;i<30;i++)
    SendFSK(1);
  
  DeactiveBsp0();
}

void TX_NameNumberData(void)
{
  int i;

  if(0 != CID_MsgData.Param1Len)
  {  // then, valid time exists
    CID_MsgData.MsgLen = 6 + CID_MsgData.Param1Len + CID_MsgData.Param2Len + CID_MsgData.Param3Len;
  }
  else
    CID_MsgData.MsgLen = 4 + CID_MsgData.Param2Len + CID_MsgData.Param3Len;
  
  CID_MsgData.CkSum = 0;
  
  if(0 != CID_MsgData.Param1Len)
  {
    CID_MsgData.CkSum = CID_MsgData.MsgType + CID_MsgData.MsgLen
     + CID_MsgData.Param1Type + CID_MsgData.Param1Len
     + CID_MsgData.Param2Type + CID_MsgData.Param2Len
     + CID_MsgData.Param3Type + CID_MsgData.Param3Len;
   
    for(i = 0; i < CID_MsgData.Param1Len; i++)   // TimeDate
      CID_MsgData.CkSum += CID_MsgData.DateTime[i];
    for(i = 0; i < CID_MsgData.Param2Len; i++)   // Name
      CID_MsgData.CkSum += CID_MsgData.Name[i];
    for(i = 0; i < CID_MsgData.Param3Len; i++)   // Number
      CID_MsgData.CkSum += CID_MsgData.Number[i];
  }
  else
  {
    CID_MsgData.CkSum = CID_MsgData.MsgType + CID_MsgData.MsgLen
     + CID_MsgData.Param2Type + CID_MsgData.Param2Len
     + CID_MsgData.Param3Type + CID_MsgData.Param3Len;
     
    for(i = 0; i < CID_MsgData.Param2Len; i++)   // Name
      CID_MsgData.CkSum += CID_MsgData.Name[i];
    for(i = 0; i < CID_MsgData.Param3Len; i++)   // Number
      CID_MsgData.CkSum += CID_MsgData.Number[i];
  }

  CID_MsgData.CkSum = (~(CID_MsgData.CkSum % 256)+1) & 0xff;

  bsp0_tx_flag=0;
 
  ActivateBsp0(0);
    
  for(i=0; i<300; i++)
    SendFSK(i & 0x0001);

  for(i=0; i<180; i++)
    SendFSK(1);

  MakeSymbol(CID_MsgData.MsgType);
  MakeSymbol(CID_MsgData.MsgLen);
  
  if(0 != CID_MsgData.Param1Len)
  {
    MakeSymbol(CID_MsgData.Param1Type);
    MakeSymbol(CID_MsgData.Param1Len);
    for(i = 0; i < CID_MsgData.Param1Len; i++)
      MakeSymbol(CID_MsgData.DateTime[i]);
  }
  MakeSymbol(CID_MsgData.Param2Type);
  MakeSymbol(CID_MsgData.Param2Len);
  for(i = 0; i < CID_MsgData.Param2Len; i++)
    MakeSymbol(CID_MsgData.Name[i]);
    
  MakeSymbol(CID_MsgData.Param3Type);
  MakeSymbol(CID_MsgData.Param3Len);
  for(i = 0; i < CID_MsgData.Param3Len; i++)
    MakeSymbol(CID_MsgData.Number[i]);
  
  MakeSymbol(CID_MsgData.CkSum);

  for(i=0;i<30;i++)
    SendFSK(1);
  
  DeactiveBsp0();
}

void TX_MessageIndication(void)
{
  int i;
  
  if(0 != CID_MsgData.Param1Len)
  {  // then, valid time exists
    CID_MsgData.MsgLen = 6 + CID_MsgData.Param1Len + CID_MsgData.Param2Len + CID_MsgData.Param3Len;
  }
  else
    CID_MsgData.MsgLen = 2 +  CID_MsgData.Param2Len;
  
  CID_MsgData.CkSum = 0;

  if(0 != CID_MsgData.Param1Len)
  {  // then, valid time exists
    CID_MsgData.CkSum = CID_MsgData.MsgType + CID_MsgData.MsgLen
      + CID_MsgData.Param1Type + CID_MsgData.Param1Len
      + CID_MsgData.Param2Type + CID_MsgData.Param2Len;
    for(i = 0; i < CID_MsgData.Param1Len; i++)
      CID_MsgData.CkSum += CID_MsgData.DateTime[i];
    for(i = 0; i < CID_MsgData.Param2Len; i++)
      CID_MsgData.CkSum += CID_MsgData.Name[i];
  }
  else
  {
    CID_MsgData.CkSum = CID_MsgData.MsgType + CID_MsgData.MsgLen
     + CID_MsgData.Param2Type + CID_MsgData.Param2Len;
  for(i = 0; i < CID_MsgData.Param2Len; i++)
    CID_MsgData.CkSum += CID_MsgData.Name[i];
  }

  CID_MsgData.CkSum = (~(CID_MsgData.CkSum % 256)+1) & 0xff;
  
  bsp0_tx_flag=0;
 
  ActivateBsp0(0);
    
  for(i=0; i<300; i++)
    SendFSK(i & 0x0001);

  for(i=0; i<180; i++)
    SendFSK(1);

  MakeSymbol(CID_MsgData.MsgType);
  MakeSymbol(CID_MsgData.MsgLen);
  
  if(0 != CID_MsgData.Param1Len)
  {  
    MakeSymbol(CID_MsgData.Param1Type);
    MakeSymbol(CID_MsgData.Param1Len);
    for(i = 0; i < CID_MsgData.Param1Len; i++)
      MakeSymbol(CID_MsgData.DateTime[i]);
  }

  MakeSymbol(CID_MsgData.Param2Type);
  MakeSymbol(CID_MsgData.Param2Len);
  for(i = 0; i < CID_MsgData.Param2Len; i++)
    MakeSymbol(CID_MsgData.Name[i]);

  MakeSymbol(CID_MsgData.CkSum);

  for(i=0;i<30;i++)
    SendFSK(1);
  
  DeactiveBsp0();
}

void ClearCidParams(void)
{
  CID_MsgData.Type = 0;
  CID_MsgData.MsgType = 0;
  memset(CID_MsgData.DateTime, 0, DATE_TIME_SZ);
  CID_MsgData.Param1Type = 0;
  CID_MsgData.Param1Len = 0;
  memset(CID_MsgData.Name, 0, NAME_SZ);
  CID_MsgData.Param2Type = 0;
  CID_MsgData.Param2Len = 0;
  memset(CID_MsgData.Number, 0, NUMBER_SZ);
  CID_MsgData.Param3Type = 0;
  CID_MsgData.Param3Len = 0;
  CID_MsgData.CkSum = 0;
}

