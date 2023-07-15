#include "motoPlus.h"

/*************************************
**	COMMON DEFINITIONS				**
**************************************/
#define	ON  			1
#define	OFF  			0

/*************************************
**	APPLICATION I/F DEFINITIONS		**
**************************************/
/* Control Time Relationship */
#define	CYCLETIME  		50				// [msec]
#define	RTC_TIME  		1				// [msec]

/* Configuration of the robot(Configuration settings must be entered manually for each) */
#define MAX_GRP			4
#define MAX_AXIS		MP_GRP_AXES_NUM
#define GRP01			0x3F			// Control Group 1
#define GRP02			0x3				// Control Group 2
#define GRP03			0x0				// Control Group 3
#define GRP04			0x0				// Control Group 4

/* B000 */
#define	NO_OP			0
#define	VAR_OPE			1
#define	RESERVED		2

/* B002 */
#define VAR_OPE_0		0
#define VAR_OPE_1		1
#define VAR_OPE_2		2
#define VAR_OPE_3		3
#define VAR_OPE_4		4
#define VAR_OPE_5		5
#define VAR_OPE_6		6
#define VAR_OPE_7		7
#define VAR_OPE_8		8


/*************************************
**	Data Structure Definition		**
**************************************/

/* Variable definitions "D" */
#define	CMD_SIZE		40
#define	CMD_START_NUM	0
#define CMD_MAX_CTRL	6
typedef struct  {
	long		GrpNo;
	long		AxisNo;
	long		UpperLimit;
	long		LowerLimit;
	long		Torque;
	}	MULTIDVAR;

typedef struct  {
	long		InputType;				//D000
	long		Res_D001;				//Reserved
	long		Res_D002;
	long		Res_D003;
	long		UseFlag[CMD_MAX_CTRL];	//D004~D009
	MULTIDVAR	Data[CMD_MAX_CTRL];		//D010~D014
										//D014~D019
										//D020~D024
										//D025~D029
										//D030~D034
										//D035~D039
} MULTIDCMD;

/* Variable definitions "B" */
#define	BVAR_SIZE		3
#define	BVAR_START_NUM	0
typedef struct  {
	long		CmdType;		// 0:No Operation, 1:Variable control of "B"/"D".
	long		CycleTime;		// Task 1 calling cycle [msec].
	long		NewCmd;			// Please refer to the source program.
} MULTIBVAR;

/*************************************
**	GLOBAL DATA DEFINITIONS			**
**************************************/
SEM_ID semid;
static int tid1, tid2;
static unsigned int		run_cnt;

/* Between task variables */
static ULONG			g_CycleTime = 0;
MP_GRP_CONFIG			g_svs_grp_config;	//Information control axis

/*************************************
**	CONST DEFINITIONS				**
**************************************/

/*************************************
**	Function Prototype				**
**************************************/
void moto_plus0_task(void);
void moto_plus1_task(void);
/*************************************
**	useful function for application	**
**************************************/
extern STATUS GetBVar(UINT16 index, long *value);
extern STATUS GetMultiBVar(UINT16 startIdx, long *value, long nbr);
extern STATUS SetBVar(UINT16 index, long value);
extern STATUS SetDVar(UINT16 index, long value);
extern STATUS GetDVar(UINT16 index, long *value);
extern STATUS mpGetMultiDVar(UINT16 startIdx, long *values, long nbr);

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
void mpUsrRoot(int arg1, int arg2, int arg3, int arg4, int arg5,
	       int arg6, int arg7, int arg8, int arg9, int arg10)
{

	mpTaskDelay(20000);	/* telnet接続時間確保の為 */

	tid1 = mpCreateTask(MP_PRI_TIME_NORMAL, MP_STACK_SIZE, (FUNCPTR)moto_plus0_task,
			    arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
	tid2 = mpCreateTask(MP_PRI_TIME_NORMAL, MP_STACK_SIZE, (FUNCPTR)moto_plus1_task,
			    arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
	semid = mpSemBCreate(SEM_Q_FIFO, SEM_EMPTY);
	puts("Exit mpUsrRoot!");
	mpExitUsrRoot;	//(or) mpSuspendSelf;
}
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
void moto_plus0_task(void)
{
	LONG BVvalue = 0;

	puts("Activate moto_plus0_task!");
	run_cnt = 0;

	/* The initial value of semaphore transmission cycle */
	if (BVvalue == 0)
	{
		g_CycleTime = CYCLETIME/RTC_TIME;	// 50[msec]
	}
	else
	{
		g_CycleTime = BVvalue;
	}
	
	while (1)
	{
		run_cnt++;
		mpSemGive(semid);
		mpTaskDelay(g_CycleTime);
	}
}
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
void moto_plus1_task(void)
{
	STATUS status;
	unsigned int run_cnt;

	MP_SVS_HANDLE	svs_handle;
	MP_TRQ_LMT_VAL	trq_lmt;
	MP_TRQ_CTL_VAL	trq_ctl;
	MP_TRQ_CTL_VAL	trq_fb;
	MP_GRP_AXES_T	vel_fb;			// MP_GRP_AXES_T[MP_GRP_NUM][MP_GRP_AXES_NUM];

	MULTIBVAR		BVar;
	MULTIDCMD		DCmd;

	
	LONG	DVvalue = 0;
	LONG	rt = 0;
	LONG	rt_func = 0;
	LONG	CmdType = 0;
	LONG	NewCmd = 0;
	LONG	LstCmd = 0;
	LONG	grp_no, axis_no, ctrl_no;
	LONG	check_bit;
	LONG	UseCnt;

	/* Information Robot Axis */
	LONG	RobotAxisInfo[] = { GRP01, GRP02, GRP03, GRP04 };

	/* Initialization */
	run_cnt = 0;
	svs_handle = 0;

	puts("Activate moto_plus1_task!");


	while (1)
	{
		run_cnt++;
		status = mpSemTake(semid, WAIT_FOREVER);
		if (status == ERROR)
		{
			printf("semTake Error![%d]\n", run_cnt);
		}

		/*-----------------------*/
		/* Gets the variable "B" */
		/*-----------------------*/
		memset(&BVar, CLEAR, sizeof(MULTIBVAR));

		rt = GetMultiBVar(BVAR_START_NUM, (long*)&BVar, BVAR_SIZE);
		CmdType = BVar.CmdType;		// B000
		NewCmd = BVar.NewCmd;		// B002

		/* Between task variables */
		if (BVar.CycleTime == 0)
		{
			g_CycleTime = CYCLETIME/RTC_TIME;	// 50[msec]
		}
		else
		{
			g_CycleTime = BVar.CycleTime;
		}

		/*------------------------------------------------------------*/
		/*	CmdType(B000) of the value switch the contents of command */
		/*------------------------------------------------------------*/
		switch (CmdType)
		{
		case NO_OP:
			break;

		/*------------------------------*/
		/* Command from the variable	*/
		/*------------------------------*/
		case VAR_OPE:	/* */

			if( (LstCmd == NewCmd) && (NewCmd != 0) )
			{
				/* If the state does nothing in writing  */
				fprintf(stderr,"\nWaiting...");
				/* Clear variable retransmission */
				rt = SetBVar(2, CLEAR);
				break;
			}

			switch (NewCmd) /* B002 */
			{
			case VAR_OPE_0:	/* When the idle processing */

				/* Variable(B002) has completed or cleared */
				if(LstCmd != 0)
				{
					LstCmd = CLEAR;
					puts("..OK\n");
				}
				break;
				
			case VAR_OPE_1: /* mpSvsStartTrqLimit */
				memset(&g_svs_grp_config, CLEAR, sizeof(MP_GRP_CONFIG));
				memset(&DCmd, CLEAR, sizeof(MULTIDCMD));
				UseCnt = CLEAR;

				rt = mpGetMultiDVar((UINT16)CMD_START_NUM, (long *)&DCmd, CMD_SIZE);
				for(ctrl_no = 0; ctrl_no < CMD_MAX_CTRL; ctrl_no++)
				{
					if(DCmd.UseFlag[ctrl_no] != 0 )
					{
						UseCnt ++;
						g_svs_grp_config.ctrl_grp |= 0x01 << DCmd.Data[ctrl_no].GrpNo;
						g_svs_grp_config.axes_config[DCmd.Data[ctrl_no].GrpNo] |= 0x01 << DCmd.Data[ctrl_no].AxisNo;
					}
				}

				if(UseCnt != 0)
				{
					svs_handle = mpSvsStartTrqLimit(&g_svs_grp_config);
					fprintf(stderr, "mpSvsStartTrqLimit(): %d\n", svs_handle);
					fprintf(stderr, "g_svs_grp_config\n");
					fprintf(stderr, "  ctrl_grp: 0x%x \n", g_svs_grp_config.ctrl_grp );
					for(grp_no = 0; grp_no < MAX_GRP; grp_no++)
					{
							fprintf(stderr, "  axes_config[%1d]: 0x%x \n", grp_no, g_svs_grp_config.axes_config[grp_no] );
					}
				}
				rt = SetBVar(2, CLEAR);		//B002
				LstCmd = VAR_OPE_1;
					
				break;
				
			case VAR_OPE_2: /* mpSvsSetTrqLimit */

				if (svs_handle > 0)	
				{
					memset(&trq_lmt, CLEAR, sizeof(MP_TRQ_LMT_VAL));
					memset(&DCmd, CLEAR, sizeof(MULTIDCMD));
					memset(&g_svs_grp_config, CLEAR, sizeof(MP_GRP_CONFIG));

					/* Retrieving the variable "D" */
					rt = mpGetMultiDVar((UINT16)CMD_START_NUM, (long *)&DCmd, CMD_SIZE);
					UseCnt = CLEAR;

					if(DCmd.InputType == TRQ_PERCENTAGE)
					{
						trq_lmt.unit = TRQ_PERCENTAGE;
					}
					else
					{
						trq_lmt.unit = TRQ_NEWTON_METER;
					}
					
					for(ctrl_no = 0; ctrl_no < CMD_MAX_CTRL; ctrl_no++)
					{
						if(DCmd.UseFlag[ctrl_no] != 0 )
						{
							UseCnt ++;

							g_svs_grp_config.ctrl_grp |= 0x01 << DCmd.Data[ctrl_no].GrpNo;
							g_svs_grp_config.axes_config[DCmd.Data[ctrl_no].GrpNo] |= 0x01 << DCmd.Data[ctrl_no].AxisNo;

							if(UseCnt == 1)
							{
								fprintf(stderr, "mpSvsSetTrqLimit(): %d", svs_handle);
								fprintf(stderr, ", InputType: %d\n", DCmd.InputType);
								fprintf(stderr, "             UpperLimit/ LowerLimit\n");
							}

							grp_no = DCmd.Data[ctrl_no].GrpNo;
							axis_no = DCmd.Data[ctrl_no].AxisNo;
							trq_lmt.data[grp_no].max[axis_no] = DCmd.Data[ctrl_no].UpperLimit;
							trq_lmt.data[grp_no].min[axis_no] = DCmd.Data[ctrl_no].LowerLimit;

							fprintf(stderr, "%1d: Axis[%1d][%1d] %10d, %10d\n",
												UseCnt, grp_no, axis_no, trq_lmt.data[grp_no].max[axis_no], trq_lmt.data[grp_no].min[axis_no]);
						}
					}

					if(UseCnt != 0)
					{
						rt = mpSvsSetTrqLimit(svs_handle, &g_svs_grp_config, &trq_lmt);
						fprintf(stderr, "  return: %d\n", rt);
					}
					rt = SetBVar(2, CLEAR);
					LstCmd = VAR_OPE_2;
				}
				else
				{
					fprintf(stderr, "ERROR Hndle Value: %d\n", svs_handle);
					rt = SetBVar(2, CLEAR);
					LstCmd = VAR_OPE_2;
				}

				break;

			case VAR_OPE_3: /* mpSvsEndTrqLimit */

				rt_func = mpSvsEndTrqLimit(svs_handle);
				fprintf(stderr, "mpSvsEndTrqLimit(): %d", rt_func);
				fprintf(stderr, ", svs_handle : %d\n", svs_handle);

				rt = SetBVar(2, CLEAR);
				LstCmd = VAR_OPE_3;
				break;


			case VAR_OPE_4: /* mpSvsStartTrqCtrl */
				memset(&g_svs_grp_config, CLEAR, sizeof(MP_GRP_CONFIG));
				memset(&DCmd, CLEAR, sizeof(MULTIDCMD));
				UseCnt = CLEAR;

				rt = mpGetMultiDVar((UINT16)CMD_START_NUM, (long *)&DCmd, (long)CMD_SIZE);
				for(ctrl_no = 0; ctrl_no < CMD_MAX_CTRL; ctrl_no++)
				{
					if(DCmd.UseFlag[ctrl_no] != 0 )
					{
						UseCnt ++;
						g_svs_grp_config.ctrl_grp |= 0x01 << DCmd.Data[ctrl_no].GrpNo;
						g_svs_grp_config.axes_config[DCmd.Data[ctrl_no].GrpNo] |= 0x01 << DCmd.Data[ctrl_no].AxisNo;
					}
				}

				if(UseCnt != 0)
				{
					svs_handle = mpSvsStartTrqCtrl(&g_svs_grp_config);
					fprintf(stderr, "mpSvsStartTrqCtrl(): %d\n", svs_handle);
					fprintf(stderr, "g_svs_grp_config\n");
					fprintf(stderr, "  ctrl_grp: 0x%x \n", g_svs_grp_config.ctrl_grp );
					for(grp_no = 0; grp_no < MAX_GRP; grp_no++)
					{
							fprintf(stderr, "  axes_config[%1d]: 0x%x \n", grp_no, g_svs_grp_config.axes_config[grp_no] );
					}
				}
				rt = SetBVar(2, CLEAR);
				LstCmd = VAR_OPE_4;

				break;
				
			case VAR_OPE_5: /* mpSvsSetTrqCtrl */

				if (svs_handle > 0)
				{
					memset(&trq_ctl, CLEAR, sizeof(MP_TRQ_CTL_VAL));
					memset(&DCmd, CLEAR, sizeof(MULTIDCMD));
					memset(&g_svs_grp_config, CLEAR, sizeof(MP_GRP_CONFIG));

					/* Retrieving the variable "D" */
					rt = mpGetMultiDVar((UINT16)CMD_START_NUM, (long *)&DCmd, CMD_SIZE);
					UseCnt = CLEAR;

					if(DCmd.InputType == TRQ_PERCENTAGE)	// [0.01%] or [Nm]
					{
						trq_ctl.unit = TRQ_PERCENTAGE;
					}
					else
					{
						trq_ctl.unit = TRQ_NEWTON_METER;
					}

					for(ctrl_no = 0; ctrl_no < CMD_MAX_CTRL; ctrl_no++)
					{
						if(DCmd.UseFlag[ctrl_no] != 0 )
						{
							UseCnt ++;

							g_svs_grp_config.ctrl_grp |= 0x01 << DCmd.Data[ctrl_no].GrpNo;
							g_svs_grp_config.axes_config[DCmd.Data[ctrl_no].GrpNo] |= 0x01 << DCmd.Data[ctrl_no].AxisNo;

							if(UseCnt == 1)
							{
								fprintf(stderr, "mpSvsSetTrqCtrl(): %d", svs_handle);
								fprintf(stderr, ", InputType: %d\n", DCmd.InputType);
								fprintf(stderr, "                      Torque\n");
							}

							grp_no = DCmd.Data[ctrl_no].GrpNo;
							axis_no = DCmd.Data[ctrl_no].AxisNo;
							trq_ctl.data[grp_no][axis_no] = DCmd.Data[ctrl_no].Torque;
							fprintf(stderr, "%1d : Axis[%1d][%1d] %10d\n",
														UseCnt, grp_no, axis_no, trq_ctl.data[grp_no][axis_no] );
						}
					}

					if(UseCnt != 0)
					{
						rt = mpSvsSetTrqCtrl(svs_handle, &g_svs_grp_config, &trq_ctl);
						fprintf(stderr, "  return: %d\n", rt);
					}

					rt = SetBVar(2, CLEAR);
						LstCmd = VAR_OPE_5;
				}
				else
				{
					fprintf(stderr, "ERROR Hndle Value: %d\n", svs_handle);

					rt = SetBVar(2, CLEAR);
					LstCmd = VAR_OPE_5;
				}

				break;

			case VAR_OPE_6: /* mpSvsEndTrqCtrl */

				rt_func = mpSvsEndTrqCtrl(svs_handle);
				fprintf(stderr, "mpSvsEndTrqCtrl(): %d", rt_func);
				fprintf(stderr, ", svs_handle : %d\n", svs_handle);

				rt = SetBVar(2, CLEAR);
				LstCmd = VAR_OPE_6;
				break;


			case VAR_OPE_7: /* mpSvsGetVelTrqFb */
				memset(&vel_fb, CLEAR, sizeof(long) * MP_GRP_NUM * MP_GRP_AXES_NUM);
				memset(&trq_fb, CLEAR, sizeof(MP_TRQ_CTL_VAL));

				rt = GetDVar(0, (long *)&DVvalue);  	// Rfering D000: [0.01%] or [Nm]
				if(DVvalue == TRQ_PERCENTAGE) {
					trq_fb.unit = TRQ_PERCENTAGE;
				}
				else {
					trq_fb.unit = TRQ_NEWTON_METER;
				}
				

				rt = mpSvsGetVelTrqFb(vel_fb, &trq_fb);
				fprintf(stderr, "mpSvsGetVelTrqFb(): %d\n", rt);
				if(rt >= 0)
				{
					for(grp_no = 0; grp_no < MAX_GRP; grp_no++)
					{
						if(RobotAxisInfo[grp_no] != 0)
						{
							if(trq_fb.unit == TRQ_PERCENTAGE)
							{
								fprintf(stderr, "Group[%d] vel[0.1pulse/s] / trq[0.01per] \n", grp_no );
							}
							else
							{
								fprintf(stderr, "Group[%d] vel[0.1pulse/s] / trq[Nm] \n", grp_no );
							}
							for(axis_no=0; axis_no < MAX_AXIS; axis_no++)
							{
								check_bit = 1 << axis_no;
								if((RobotAxisInfo[grp_no] & check_bit) != 0)
								{
									fprintf(stderr, "  Axis[%d] %10d %8d\n", axis_no+1, vel_fb[grp_no][axis_no],  trq_fb.data[grp_no][axis_no]);
								}
							}
						}
					}
				}
				rt = SetBVar(2, CLEAR);
				LstCmd = VAR_OPE_7;
				break;

			case VAR_OPE_8: /* mpCtrlGrpId2GrpNo */

				puts("");
				puts("[GroupID] -> [GrpNo]");
				puts("--------------------");
				printf("MP_R1_GID -> %2d\n", mpCtrlGrpId2GrpNo(MP_R1_GID));
				printf("MP_R2_GID -> %2d\n", mpCtrlGrpId2GrpNo(MP_R2_GID));
				printf("MP_R3_GID -> %2d\n", mpCtrlGrpId2GrpNo(MP_R3_GID));
				printf("MP_B1_GID -> %2d\n", mpCtrlGrpId2GrpNo(MP_B1_GID));
				printf("MP_B2_GID -> %2d\n", mpCtrlGrpId2GrpNo(MP_B2_GID));
				printf("MP_S1_GID -> %2d\n", mpCtrlGrpId2GrpNo(MP_S1_GID));
				printf("MP_S2_GID -> %2d\n\n", mpCtrlGrpId2GrpNo(MP_S2_GID));
				rt = SetBVar(2, CLEAR);
				LstCmd = VAR_OPE_8;
				break;

			default:
				/* Perform a forced reset */
				rt = mpSvsForceInit();
				fprintf(stderr, "ServoAPI Reset!!\nmpSvsForceInit(): %d\n", rt);

				LstCmd = NewCmd;
				rt = SetBVar(2, CLEAR);
				break;

			}
			break;

		case RESERVED:
			break;

		default:
			break;
		}
	}
}


