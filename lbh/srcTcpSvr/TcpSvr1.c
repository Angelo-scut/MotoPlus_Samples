/* TcpSvr1.c */
/* Copyright 2009 YASKAWA ELECTRIC All Rights reserved. */

#include "motoPlus.h"

#define	COMMAND_0		0	// Unknown
#define COMMAND_1		1	// corrpath
#define COMMAND_2		2	// spdoverride_on
#define COMMAND_3		3	// spdoverride_off
#define COMMAND_4		4	// spdoverride_0
#define COMMAND_5		5	// forcepathend
#define COMMAND_6		6	// combination_test

#define COMBI_0			0
#define COMBI_1			1
#define COMBI_2			2
#define COMBI_3			3
// for GLOBAL DATA DEFINITIONS
SEM_ID semid;  // 信号ID

// for IMPORT API & FUNCTIONS
extern void moto_plus0_task(void);

// for LOCAL DEFINITIONS
static int tid1;

int nTaskID1;
MSG_Q_ID msgQId;

int command_no;

//FUNCTION PROTOTYPES
void sensCommRcvTask(void);

void mpUsrRoot(int arg1, int arg2, int arg3, int arg4, int arg5,
	       int arg6, int arg7, int arg8, int arg9, int arg10)
{
	msgQId = mpMsgQCreate(1, sizeof(SYS2MP_SENS_MSG), MSG_Q_FIFO);

	nTaskID1 = mpCreateTask(MP_PRI_TIME_CRITICAL, MP_STACK_SIZE, (FUNCPTR)sensCommRcvTask,
		arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);

	tid1 = mpCreateTask(MP_PRI_TIME_NORMAL, MP_STACK_SIZE, (FUNCPTR)moto_plus0_task,
			    arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);

	semid = mpSemBCreate(SEM_Q_FIFO, SEM_EMPTY);	// バイナリセマフォ
	puts("Exit mpUsrRoot!");
	mpExitUsrRoot;	//(or) mpSuspendSelf;
}

//Command Receive Task
void sensCommRcvTask(void)
{
	SYS2MP_SENS_MSG msg;
	int status;

	memset(&msg, CLEAR, sizeof(SYS2MP_SENS_MSG));

	FOREVER
	{
		mpEndSkillCommandProcess(MP_SL_ID1, &msg);
		status = mpReceiveSkillCommand(MP_SL_ID1, &msg);

		if (status == OK)
		{
			printf("mpReceiveSkillCommand OK\n\r");
			printf("main_comm %d\n\r", msg.main_comm);
			printf("sub_comm %d\n\r", msg.sub_comm);
			printf("exe_tsk %d\n\r", msg.exe_tsk);
			printf("exe_apl %d\n\r", msg.exe_apl);
			printf("comm_link %s\n\r", msg.cmd);
		}
		else
		{
			printf("mpReceiveSkillCommand Error\n\r");
			mpTaskDelay(100);
			continue;
		}

		switch (msg.main_comm)
		{
			case MP_SKILL_COMM:
				switch (msg.sub_comm)
				{
					case MP_SKILL_SEND:
						if (strcmp(msg.cmd, "corrpath") == 0)
						{
							command_no = COMMAND_1;
							printf("Command Recive  CorrPath\n\r");
						}
						else if (strcmp(msg.cmd, "spdoverride_on") == 0)
						{
							command_no = COMMAND_2;
							printf("Command Recive  SpdOverride_on\n\r");
						}
						else if (strcmp(msg.cmd, "spdoverride_off") == 0)
						{
							command_no = COMMAND_3;
							printf("Command Recive  SpdOverride_off\n\r");
						}
						else if (strcmp(msg.cmd, "spdoverride_0") == 0)
						{
							command_no = COMMAND_4;
							printf("Command Recive  spdoverride_0\n\r");
						}
						else if (strcmp(msg.cmd, "forcepathend") == 0)
						{
							command_no = COMMAND_5;
							printf("Command Recive  ForcePathEnd\n\r");
						}
						else if (strcmp(msg.cmd, "combination_test") == 0)
						{
							command_no = COMMAND_6;
							printf("Command Recive  combination_test\n\r");
						}
						else
						{
							command_no = COMMAND_0;
							printf("Unknown Command\n\r");
						}
						break;

					case MP_SKILL_END:
						printf("MP_SKILL_END\n\r");
						break;

					default:
						printf("Unknown Sub Command\n\r");
						break;
				}
				break;

			case MP_SL_RST_COMM:

				switch (msg.sub_comm)
				{
					case MP_SL_SOFTWARE_RST:
						printf("MP_SL_SOFTWARE_RST\n\r");
						break;

					case MP_SL_ALM_RST:
						printf("MP_SL_ALM_RST\n\r");
						break;

					case MP_START_SEG_CLK:
						printf("MP_START_SEG_CLK\n\r");
						break;

					case MP_SE_PRM_TRANS:
						printf("MP_SE_PRM_TRANS\n\r");
						break;

					default:
						printf("Unknown Sub Command\n\r");
						break;
				}
				break;

			default:
				printf("Unknown Main Command\n\r");
				break;
		}
	}
}