#ifndef __CALLCONTROL_H__
#define __CALLCONTROL_H__

#include "su.h"

typedef struct SendMsgList
{
	VFI dst;
	mbx_typ type;
} SendMsgList, *SndMsgLst;

typedef struct ToDoList
{
	state_service_func (*service_func)(void *);
	void				*arg;
} ToDoList, *pToDoList;

typedef struct StateTable
{
	VFI dst;
	mbx_typ type;
	int nextState;
	struct ToDoList *functlst;
	
} StateTable, *StsTbl;


typedef struct StateList
{
	 struct StateTable *start;
	
    
} StateList, *StsLst;


void tsk_callcontrol(int);
state_service_func SendMessageBunch(void *p);

#endif //__CALLCONTROL_H__

