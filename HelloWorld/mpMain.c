//mpMain.c
//
//This contains mpUsrRoot which is the entry point for your MotoPlus application

//ADDITIONAL INCLUDE FILES 
//(Note that motoPlus.h should be included in every source file)
#include "motoPlus.h"

//GLOBAL DATA DEFINITIONS
int nTaskID1;

//FUNCTION PROTOTYPES
void moto_plus0_task(void);
//FUNCTION DEFINITIONS

void mpUsrRoot(int arg1, int arg2, int arg3, int arg4, int arg5,
	       int arg6, int arg7, int arg8, int arg9, int arg10)
{
	nTaskID1 = mpCreateTask(MP_PRI_TIME_NORMAL, MP_STACK_SIZE, (FUNCPTR)moto_plus0_task, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);

			mpExitUsrRoot;		
}

void moto_plus0_task(void)
{
	while (1)
	{
		puts("Hello World!");
		mpTaskDelay(2500);
	}
}
