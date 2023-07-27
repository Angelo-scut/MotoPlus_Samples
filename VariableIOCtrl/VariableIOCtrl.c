/**** VariableIOCtrl.c ****/
/* Copyright 2009 YASKAWA ELECTRIC All Rights reserved. */

#include "motoPlus.h"

// for External definition
extern void moto_plus0_task(void);
extern void moto_plus1_task(void);

// for DATA
extern SEM_ID semid;

// for LOCAL DEFINITIONS
static unsigned int run_cnt;		// for moto_plus0_task.
static unsigned int cycletime; 

#define	CYCLETIME  	50			//[msec]
#define	RTC_TIME  	1			//[msec]
#define	MOVE_DISTANCE  	50000	//[micro meter] 微米，毫米是millimeter
#define	PULSE_TIME  20			//[msec]

#define	ON  	1
#define	OFF  	0

/**************************************
**	useful function for application ***
***************************************/

extern STATUS GetBVar(UINT16 index, long *value);
extern STATUS GetMultiBVar(UINT16 startIdx, long *value, long nbr);
extern STATUS SetBVar(UINT16 index, long value);
extern STATUS GetIVar(UINT16 index, long *value);
extern STATUS SetIVar(UINT16 index, long value);
extern STATUS SetDVar(UINT16 index, long value);
extern STATUS SetMultiDVar(UINT16 startIdx, long *values, long nbr);
extern STATUS GetMultiDVar(UINT16 startIdx, long *values, long nbr);
extern STATUS GetXVar(UINT16 type, UINT16 index, char *value);
extern STATUS GetIo(UINT32 ioAddr, USHORT *signal);
extern STATUS SetIo(UINT32 ioAddr, USHORT signal);
extern STATUS SetMultiIo(UINT32 ioStartAddr, USHORT *signal, long nbr);
extern STATUS WriteIO(UINT32 addr, UINT16 value);
extern STATUS ReadIO(UINT32 addr, UINT16* value);

void PulseOut(UINT32 io_adr, int time)
{
	LONG rt = 0;

	rt = SetIo(io_adr, ON);
	mpTaskDelay(time);
	rt = SetIo(io_adr, OFF);
	return;
}

void moto_plus0_task(void)
{
	LONG BVvalue = 0;
	LONG rt = 0;
	
	puts("Activate moto_plus0_task!");

	run_cnt = 0;
	cycletime = 0;
	
	while (1)
	{
		run_cnt++;

		mpSemGive(semid);

		rt = GetBVar(1, &BVvalue);		// B001
		if (BVvalue == 0) 
			cycletime = CYCLETIME/RTC_TIME;
		else
			cycletime = BVvalue * 10;
		mpTaskDelay(cycletime);
	}
}

void moto_plus1_task(void)
{
	STATUS status;
	unsigned int run_cnt;
	unsigned int turn;
	
	unsigned int case1state;

	LONG BVvalue = 0;
	LONG rt = 0;
		
	//float get_time;
	MP_POSVAR_DATA mp_posvar_data;
	
	MP_CTRL_GRP_SEND_DATA mp_ctrl_grp_send_data;
	MP_CART_POS_RSP_DATA mp_cart_pos_rsp_data;	

	///	

	puts("Activate moto_plus1_task!");

	run_cnt = 0;
	turn = 0;
	case1state = 0;

	while (1)
	{
		run_cnt++;
		status = mpSemTake(semid, WAIT_FOREVER);
		if (status == ERROR)
		{
			printf("semTake Error![%d]\n", run_cnt);
		}

		rt = GetBVar(0, &BVvalue);  		// Rfering B000

		if (BVvalue != 1) case1state = 0;  	// initilize case1stae if (b000 != 1)
		switch (BVvalue)
		{
		case 0:    // Initilaize data
			turn=0;
			break;

		case 1: // only first time get current cart pos, 
				// but all the time set P10 got current pos + distance or - distance
			
			if (case1state == 0) 
			{    	// get curent cart position 
				mp_ctrl_grp_send_data.sCtrlGrp = 0;	//  R1
				if (mpGetCartPos(&mp_ctrl_grp_send_data, &mp_cart_pos_rsp_data) != 0)  
				{
					puts("get cart pos error");
					break;
				}
				// Copy current position to P variable
				memset(&mp_posvar_data.ulValue[0], 0, (sizeof(long) * 10 )); // all clear
				memcpy(&mp_posvar_data.ulValue[2], &mp_cart_pos_rsp_data.lPos[0], (sizeof(long) * 6));  
				mp_posvar_data.ulValue[0] |= (long)(mp_cart_pos_rsp_data.sConfig << 8);  // 按位计算，左移8位，也就是用低8位覆盖高8位,这是俯仰等定义
				case1state = 1;
			}
			mp_posvar_data.usType = MP_RESTYPE_VAR_ROBOT;  
			mp_posvar_data.usIndex = 10;			// P var number (P010)，这里确定是P010
			mp_posvar_data.ulValue[0] |= 0x0010;	// Cartesian (base coordinates) ，这个只是确定坐标轴而已

			if (turn == 0)  // set 50mm or -50mm to z direction data depending on turn vale
			{
				mp_posvar_data.ulValue[4] = mp_posvar_data.ulValue[4] + MOVE_DISTANCE;
				turn = 1;
				mpPutPosVarData(&mp_posvar_data, 1);

				rt = SetIo(10012, ON);		// #10012 --> ON
				rt = SetIo(10013, OFF);		// #10013 --> OFF
				PulseOut(10014, PULSE_TIME);    // #10014 pulse out 20msec
			}
			else
			{
				mp_posvar_data.ulValue[4] = mp_posvar_data.ulValue[4] - MOVE_DISTANCE;
				turn = 0;
				mpPutPosVarData(&mp_posvar_data, 1);

				rt = SetIo(10012, OFF);		// #10012 --> OFF
				rt = SetIo(10013, ON);		// #10013 --> ON
				PulseOut(10014, PULSE_TIME);  	// #10014 pulse out 20msec
			}
			break;

		case 2: 
		case 3:
			break;

		default:
			break;
		}
	}
}

