/**
 * @file PosPrcs.c
 * @author Zishun Wang
 * @brief update the position to robot
 * @version 0.1
 * @date 2023-07-24
 * 
 * @copyright Copyright (c) 2023
 * 
 */

// #include "motoPlus.h"
#include "MacroDefine.h"

extern SEM_ID semid;

// for LOCAL DEFINITIONS
static unsigned int run_cnt;		// for moto_plus0_task.
static unsigned int cycletime; 

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

extern MP_POSVAR_DATA pos_data;
extern UINT16 B_addr;

BOOL safe_guard(int pos_limit, int  angle_limit);  //  先限制每次运动只能在一个厘米范围内
void PulseOut(UINT32 io_adr, int time);  // 对io_adr地址内触发一个时间为time的PWM

void pos_process_task(void){
    STATUS status;

    LONG BVvalue0 = 0;
    LONG BVvalue1 = 0;
    LONG rt = 0;

    rt = SetBVar(1, &BVvalue1);

    puts("Activate pos_process_task!");

    while (1)
    {
        status = mpSemTake(semid, WAIT_FOREVER);  // 一直等待TCP发送的信号
        if(status == ERROR){
            printf("semTake Error![%d]\n", run_cnt);
            continue;
        }

        rt = GetBVar(0, &BVvalue0);

        if (BVvalue0 == 1 && safe_guard(SAFE_DIST, SAFE_ANGL))
        {
            mpPutPosVarData(&pos_data, 1);
            //BVvalue1 = 1;
            //rt = SetBVar(1, &BVvalue1);
			// SetIo(10014, ON);
			//PulseOut(10014, PULSE_TIME);  // 不能太低，INFORM语言运行速率没那么快
			SetBVar(B_addr, 1);
        }
    }
    
}

BOOL safe_guard(int pos_limit, int  angle_limit){
    MP_CART_POS_RSP_DATA mp_cart_pos_rsp_data;  // 用于接收笛卡尔坐标的信息
    MP_CTRL_GRP_SEND_DATA mp_ctrl_grp_send_data;  // 当时机器人群组的时候，可以通过这个控制返回哪个机器人的信息
    mp_ctrl_grp_send_data.sCtrlGrp = 0; // 只取机器人1的信息，因为只有一台机器人
    if (mpGetCartPos(&mp_ctrl_grp_send_data, &mp_cart_pos_rsp_data) != 0)  // TODO:是否需要每次都对mp_cart_pos_rsp_data进行初始化
    {
        puts("get cart pos error!");
        return FALSE;
    }

    int i = 0;
    for (; i < 3; i++)  // 位置，因为单位是微米，um，1cm = 1000mm = 1000 000 um
    {
        if (abs(mp_cart_pos_rsp_data.lPos[i] - pos_data.ulValue[i + 2]) > pos_limit)
        {
            return FALSE;
        }
        
    }
    for (; i < 6; i++)  // 姿态
    {
        if (abs(mp_cart_pos_rsp_data.lPos[i] - pos_data.ulValue[i + 2]) > angle_limit)
        {
            return FALSE;
        }
    }
    pos_data.ulValue[0] |= (long)(mp_cart_pos_rsp_data.sConfig << 8); // 保留当前位置的设置
    pos_data.usType = MP_RESTYPE_VAR_ROBOT;  
    pos_data.usIndex = 10;			// P var number (P010)，这里确定是P010
    pos_data.ulValue[0] |= 0x0010;	// Cartesian (base coordinates) ，这个只是确定坐标轴而已
    return TRUE;
}

void PulseOut(UINT32 io_adr, int time)  // 对io_adr地址内触发一个时间为time的PWM
{
	LONG rt = 0;

	rt = SetIo(io_adr, ON);
	mpTaskDelay(time);
	rt = SetIo(io_adr, OFF);
	return;
}
