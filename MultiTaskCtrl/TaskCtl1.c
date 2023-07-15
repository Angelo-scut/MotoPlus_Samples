/**** TaskCtl1.c ****/
/* Copyright 2009 YASKAWA ELECTRIC All Rights reserved. */

#include "motoPlus.h"

// for GLOBAL DATA DEFINITIONS
SEM_ID semid;

// for IMPORT API & FUNCTIONS
extern void moto_plus0_task(void);
extern void moto_plus1_task(void);

// for LOCAL DEFINITIONS
static int tid1, tid2;

void mpUsrRoot(int arg1, int arg2, int arg3, int arg4, int arg5,
	       int arg6, int arg7, int arg8, int arg9, int arg10)
{
	tid1 = mpCreateTask(MP_PRI_TIME_NORMAL, MP_STACK_SIZE, (FUNCPTR)moto_plus0_task,
			    arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
	tid2 = mpCreateTask(MP_PRI_TIME_NORMAL, MP_STACK_SIZE, (FUNCPTR)moto_plus1_task,
			    arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
	semid = mpSemBCreate(SEM_Q_FIFO, SEM_EMPTY);	// バイナリセマフォ

	puts("Exit mpUsrRoot!");
	mpExitUsrRoot;	//(or) mpSuspendSelf;
}
