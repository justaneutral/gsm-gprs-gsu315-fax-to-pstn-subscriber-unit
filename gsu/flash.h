#ifndef __FLASH_H__
#define __FLASH_H__

int StoreParam( char name[], void *aParam, int lParam);
int RestoreParam( char name[], void *addrParam);
int OpenParam();
int Erase_Param();

void InitFlashParams(void);
char *ReadFlashData(char *Name);
int UpdateFlashData(char *Name, char *Data);
char *ReadFlashDataImage(char *Name);

int ReadAllCallData(void);
char *GetAllCallData(char *Name);
int RetainDelayedParams(char *Name, char *Data);
int StoreDelayedParams(void);
void ClearDelayedParams(void);
int ForcedPowerSaving(void);

#endif
