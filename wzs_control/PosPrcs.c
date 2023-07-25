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

#include "motoPlus.h"

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

void PulseOut(UINT32 io_adr, int time)  // 对io_adr地址内触发一个时间为time的PWM
{
	LONG rt = 0;

	rt = SetIo(io_adr, ON);
	mpTaskDelay(time);
	rt = SetIo(io_adr, OFF);
	return;
}

void pos_process_task(void){
    STATUS status;
    unsigned int run_cnt;
    unsigned int turn;

    unsigned int case1state;

    LONG BVvalue = 0;
    LONG rt = 0;

    MP_POSVAR_DATA mp_posvar_data;

    puts("Activate pos_process_task!");

    run_cnt = 0;
    turn = 0;
    case1state = 0;

    while (1)
    {
        run_cnt++;
        status = mpSemTake(semid, WAIT_FOREVER);  // 一直等待TCP发送的信号
        if(status == ERROR){
            printf("semTake Error![%d]\n", run_cnt);
        }

        rt = GetBVar(0, &BVvalue);

        mpPutPosVarData(&mp_posvar_data, 1);
        
    }
    
}