
//mpMain.c
//
//This contains mpUsrRoot which is the entry point for your MotoPlus application

//ADDITIONAL INCLUDE FILES 
//(Note that motoPlus.h should be included in every source file)
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

//GLOBAL DATA DEFINITIONS
int nTaskID1;
int nTaskID2;
MSG_Q_ID msgQId;

int command_no;

//FUNCTION PROTOTYPES
void sensCommRcvTask(void);
void sensSomeWorkTask(void);

//FUNCTION DEFINITIONS
void mpUsrRoot(int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10)
{	
	msgQId = mpMsgQCreate(1, sizeof(SYS2MP_SENS_MSG), MSG_Q_FIFO);

	nTaskID1 = mpCreateTask(MP_PRI_TIME_CRITICAL, MP_STACK_SIZE, (FUNCPTR)sensCommRcvTask,
						arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);

	nTaskID2 = mpCreateTask(MP_PRI_IP_CLK_TAKE, MP_STACK_SIZE, (FUNCPTR)sensSomeWorkTask,
						arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);	
	
	mpExitUsrRoot;
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
		
		if( status == OK )
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
		
		switch(msg.main_comm)
		{
			case MP_SKILL_COMM:
				switch(msg.sub_comm)
				{
					case MP_SKILL_SEND:
						if(strcmp(msg.cmd, "corrpath") == 0)
						{
							command_no = COMMAND_1;
							printf("Command Recive  CorrPath\n\r");
						}
						else if(strcmp(msg.cmd, "spdoverride_on") == 0)
						{
							command_no = COMMAND_2;
							printf("Command Recive  SpdOverride_on\n\r");
						}
						else if(strcmp(msg.cmd, "spdoverride_off") == 0)
						{
							command_no = COMMAND_3;
							printf("Command Recive  SpdOverride_off\n\r");
						}
						else if(strcmp(msg.cmd, "spdoverride_0") == 0)
						{
							command_no = COMMAND_4;
							printf("Command Recive  spdoverride_0\n\r");
						}
						else if(strcmp(msg.cmd, "forcepathend") == 0)
						{
							command_no = COMMAND_5;
							printf("Command Recive  ForcePathEnd\n\r");
						}
						else if(strcmp(msg.cmd, "combination_test") == 0)
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
								
				switch(msg.sub_comm)
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


void sensSomeWorkTask(void)
{
	SYS2MP_SENS_MSG msg;
	int ret;
	long dy = 0;
	int cnt = 0; 
	int CorrPath_cnt = 0;
	int combination_test_state = COMBI_0;
	
	MP_POS_DATA corrpath_src_p;
	long spd_src_p[MP_GRP_NUM];
	CTRLG_T ctrl_grp = 1;
	
	
	memset(&msg, CLEAR, sizeof(SYS2MP_SENS_MSG));
	memset(&corrpath_src_p, CLEAR, sizeof(MP_POS_DATA));
	memset(&spd_src_p, CLEAR, sizeof(long) * MP_GRP_NUM);
		
	corrpath_src_p.ctrl_grp = 1;
	corrpath_src_p.grp_pos_info[0].pos_tag.data[0] = 0x3f;
	corrpath_src_p.grp_pos_info[0].pos_tag.data[3] = MP_CORR_RF_DTYPE;
	corrpath_src_p.grp_pos_info[0].pos[0] = 0;
	corrpath_src_p.grp_pos_info[0].pos[1] = 0;
	corrpath_src_p.grp_pos_info[0].pos[2] = 0;
	corrpath_src_p.grp_pos_info[0].pos[3] = 0;
	corrpath_src_p.grp_pos_info[0].pos[4] = 0;
	corrpath_src_p.grp_pos_info[0].pos[5] = 0;
		
	spd_src_p[0] = 15000;

	FOREVER
	{
		mpClkAnnounce(MP_INTERPOLATION_CLK);
		
		switch(command_no)
		{
			// Unknown
			case COMMAND_0:
				break;
		
			//PutCorrPath
			case COMMAND_1:
				corrpath_src_p.grp_pos_info[0].pos[1] = dy;
				ret = mpMeiPutCorrPath(MP_SL_ID1, &corrpath_src_p);
				if(ret != 0)
				{
					printf("mpMeiPutCorrPath Error  ret = %d\n\r", ret);
				}
			
				dy = 0;
				cnt++;
			
				//wait 1s
				if(cnt > 250)
				{
					cnt = 0;
					dy = 1000;
					CorrPath_cnt++;
					if(CorrPath_cnt > 10)
					{
						command_no = COMMAND_0;
						CorrPath_cnt = 0;
					}
				}
				
				break;
				
			//SpdOverride_on
			case COMMAND_2:
				spd_src_p[0] = 15000;
				ret = mpMeiPutSpdOverride(MP_SL_ID1, ctrl_grp, spd_src_p);
				if(ret != 0)
				{
					printf("mpMeiPutSpdOverride Error  ret = %d\n\r", ret);
				}

				command_no = COMMAND_0;
				break;
				
			//SpdOverride_off
			case COMMAND_3:
				//speed override cancel
				ret = mpMeiPutSpdOverride(MP_SL_ID1, OFF, NULL);
				if(ret != 0)
				{
					printf("mpMeiPutSpdOverride Error  ret = %d\n\r", ret);
				}

				command_no = COMMAND_0;
				break;
				
			//SpdOverride_0
			case COMMAND_4:
				spd_src_p[0] = 0;
				ret = mpMeiPutSpdOverride(MP_SL_ID1, ctrl_grp, spd_src_p);
				if(ret != 0)
				{
					printf("mpMeiPutSpdOverride Error  ret = %d\n\r", ret);
				}

				command_no = COMMAND_0;
				break;
				
			//ForcePathEnd
			case COMMAND_5:
				cnt++;
				
				//wait 3s
				if(cnt > 750)
				{
					ret = mpMeiPutForcePathEnd(MP_SL_ID1, ctrl_grp);
					if(ret != 0)
					{
						printf("mpMeiPutForcePathEnd Error  ret = %d\n\r", ret);
					}
					else
					{
						printf("mpMeiPutForcePathEnd done\n\r");
					}
					command_no = COMMAND_0;
					cnt = 0;
				}
				break;

			//combination test
			case COMMAND_6:
	
			    switch (combination_test_state) 
			    {
			    case COMBI_0:
					spd_src_p[0] = 0;
					ret = mpMeiPutSpdOverride(MP_SL_ID1, ctrl_grp, spd_src_p);
					if(ret != 0)
					{
						printf("mpMeiPutSpdOverride Error  ret = %d\n\r", ret);
					}

					combination_test_state = COMBI_1;	
					break;
					
				case COMBI_1:
					corrpath_src_p.grp_pos_info[0].pos[1] = dy;
					ret = mpMeiPutCorrPath(MP_SL_ID1, &corrpath_src_p);
					if(ret != 0)
					{
						printf("mpMeiPutCorrPath Error  ret = %d\n\r", ret);
					}
			
					dy = 0;
					cnt++;
			
					//wait 1s
					if(cnt > 250)
					{
						cnt = 0;
						dy = 1000;
						CorrPath_cnt++;
						if(CorrPath_cnt > 10)
						{
							CorrPath_cnt = 0;
							combination_test_state = COMBI_2;	
						}
					}
					break;

				case COMBI_2:
					ret = mpMeiPutForcePathEnd(MP_SL_ID1, ctrl_grp);
					if(ret != 0)
					{
						printf("mpMeiPutForcePathEnd Error  ret = %d\n\r", ret);
					}
					else
					{
						printf("mpMeiPutForcePathEnd done\n\r");
					}
					command_no = COMMAND_0;
					combination_test_state = COMBI_0;
					
					break;
					
				default:
					command_no = COMMAND_0;
					combination_test_state = COMBI_0;
					break;
				}
				break;
		
			default:
				dy = 0;
				cnt = 0;
				break;
		}
	}
}
