/**
 * @file TaskCreator.c
 * @author Zishun Wang
 * @brief Task creator for YASKAWA robot control project
 * @date 2023-07-24
 * 
 * @copyright Copyright (c) 2023
 * 
 */

// #include "motoPlus.h"
#include "MacroDefine.h"


SEM_ID semid;

extern void tcp_server_task(void);
extern void pos_process_task(void);
extern void offsControlTask(void);
extern void sensCommRcvTask(void);

static int tid1;
static int tid2;
static int tid3;
static int tid4;

//int nTaskID1;
MSG_Q_ID msgQId;

void mpUsrRoot(int arg1, int arg2, int arg3, int arg4, int arg5,
	       int arg6, int arg7, int arg8, int arg9, int arg10)
{
	msgQId = mpMsgQCreate(1, sizeof(SYS2MP_SENS_MSG), MSG_Q_FIFO);
	tid1 = mpCreateTask(MP_PRI_TIME_NORMAL, MP_STACK_SIZE, (FUNCPTR)tcp_server_task,
			    arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
	tid2 = mpCreateTask(MP_PRI_TIME_NORMAL, MP_STACK_SIZE, (FUNCPTR)pos_process_task,
			    arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
	tid3 = mpCreateTask(MP_PRI_TIME_CRITICAL, MP_STACK_SIZE, (FUNCPTR)sensCommRcvTask,
				arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
	tid4 = mpCreateTask(MP_PRI_IP_CLK_TAKE, MP_STACK_SIZE, (FUNCPTR)offsControlTask,
				arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
	semid = mpSemBCreate(SEM_Q_FIFO, SEM_EMPTY);	// 创建信号
	puts("Exit mpUsrRoot!");
	mpExitUsrRoot;	//(or) mpSuspendSelf;
}
