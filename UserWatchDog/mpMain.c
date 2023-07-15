#include "motoPlus.h"
#include "funclib.h"

/*--------------------------------------------------------------------------------------------------*/
#define LapTimeMon		ON		/* Processing time measurement for debug							*/
#define DebugPrint		ON
#define DVAL_NUM		20		/* Variable number of parameters "D"								*/
/*--------------------------------------------------------------------------------------------------*/
// for Local Variables definition
SEM_ID	semid;
INT32	ip_tid,tn_tid;
/*--------------------------------------------------------------------------------------------------*/
FLOAT	ScanTime = 0., LapTime = 0.;
/*--------------------------------------------------------------------------------------------------*/
LONG				DVvalue[DVAL_NUM];
MP_USR_WDG_ROUTINE	wdRoutine[2];
LONG				ClearCnt[2];
MP_WDG_HANDLE		handle[2];
int					delay[2];
/*--------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
// for Local Functions definition
static void mp_seg_mon_task(void);
static void mp_para_get_task(void);
static void GetParameter(void);
static void fncwdRoutine0(MP_WDG_HANDLE);
static void fncwdRoutine1(MP_WDG_HANDLE);
/*--------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------*/
void mpUsrRoot(int arg1, int arg2, int arg3, int arg4, int arg5,
		      int arg6, int arg7, int arg8, int arg9, int arg10)
{

	mpTaskDelay(10000);       // 10sec wait
	
	semid = mpSemBCreate(SEM_Q_FIFO, SEM_EMPTY);
#if DebugPrint == ON
	printf("mpSemBCreate semid: %p\n", semid);
#endif

	ip_tid  = mpCreateTask(MP_PRI_IP_CLK_TAKE,   MP_STACK_SIZE, (FUNCPTR)mp_seg_mon_task,
			    arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
	tn_tid = mpCreateTask(MP_PRI_TIME_NORMAL, MP_STACK_SIZE, (FUNCPTR)mp_para_get_task,
			    arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);

	mpExitUsrRoot;
}


/*--------------------------------------------------------------------------------------------------*/
static void mp_para_get_task(void)
{
#if DebugPrint == ON
	puts("Activate mp_para_get_task!");
#endif	
	mpTaskDelay(1000);		// 1sec wait

	FOREVER
	{
		GetParameter( );
		mpTaskDelay(1000);	// 1sec wait;
	}
}

/*--------------------------------------------------------------------------------------------------*/
static void mp_seg_mon_task(void)
{
	int 	rc, i;
	LONG	Cnt[2];
	LONG	DVvalue_last[2];
#if DebugPrint == ON
	puts("Activate mp_seg_mon_task!");
#endif
	wdRoutine[0] = (MP_USR_WDG_ROUTINE)fncwdRoutine0;
	wdRoutine[1] = (MP_USR_WDG_ROUTINE)fncwdRoutine1;
#if DebugPrint == ON
	printf("fnc ptr!! [%d] [%d]\n", wdRoutine[0], wdRoutine[1]);
#endif
	GetParameter( );

	FOREVER
	{
		if (mpClkAnnounce(MP_INTERPOLATION_CLK) == ERROR)
		{
#if DebugPrint == ON
			puts("Err: RTP IP_CLK ANNOUNCE");
#endif
			mpTaskSuspend(ip_tid);
		}
		
		for( i = 0; i <2; i++ )
		{
			//D061,D062
			if( (DVvalue_last[i] == 0) && (DVvalue[11 + i] == 1) )
			{
				Cnt[i] = 0;
				rc = mpUsrWdogStart( handle[i] );
				printf("WdogStart(%d)!! [handle = %d] [rc = %d]\n",
										i, handle[i], rc);
			}
			DVvalue_last[i] = DVvalue[11 + i];
			
			if( DVvalue[11 + i] == 1 )
			{
				if( ClearCnt[i] > Cnt[i] )
				{
					++Cnt[i];
					if( ClearCnt[i] == Cnt[i] )
					{
						rc = mpUsrWdogClear( handle[i] );
						Cnt[i] = 0;
					}
				}
			}
		}
	}
}

/*--------------------------------------------------------------------------------------------------*/
static void GetParameter(void)
{
	int rc;
	int index;
	
	
	LONG	DVvalue050, DVvalue051, DVvalue052, DVvalue053;
	
	//Save DVar oldvalue
	DVvalue050 = DVvalue[0];	//WdogCreate
	DVvalue051 = DVvalue[1];	//WdogDelete
	DVvalue052 = DVvalue[2];	//WdogStart
	DVvalue053 = DVvalue[3];	//WdogClear

	//Get DVar value
	if( mpGetMultiDVar(50, DVvalue, DVAL_NUM) == 0 )
	{
		
		//Get index value : D054
		if( DVvalue[4] < 2 )
		{
			index = (int)DVvalue[4];
		}
		else
		{
			index = 0;
		}
		
		//Get delay value:  D055,D056
		delay[0] = (int)DVvalue[5];
		delay[1] = (int)DVvalue[6];
		//Get ClearCnt value:  D058,D059
		ClearCnt[0] = DVvalue[8];
		ClearCnt[1] = DVvalue[9];

		//WdogCreate
		if( (DVvalue050 == 0) && (DVvalue[0] == 1) ) //D050 "0"->"1"
		{
			handle[index] = mpUsrWdogCreate( delay[index] , wdRoutine[index] );
			printf("WdogCreate(%d)!! [delay = %d] [wdRoutine = %d] [handle = %d]\n",
									index, delay[index], wdRoutine[index], handle[index]);
		}
		
		//WdogDelete
		if( (DVvalue051 == 0) && (DVvalue[1] == 1) ) //D051 "0"->"1"
		{
			rc = mpUsrWdogDelete( handle[index] );
			printf("WdogDelete(%d)!! [handle = %d] [rc = %d]\n",
									index, handle[index], rc);
		}
		
		//WdogStart
		if( (DVvalue052 == 0) && (DVvalue[2] == 1) ) //D052 "0"->"1"
		{
			rc = mpUsrWdogStart( handle[index] );
			printf("WdogStart(%d)!! [handle = %d] [rc = %d]\n",
									index, handle[index], rc);
		}
		
		//WdogClear
		if( (DVvalue053 == 0) && (DVvalue[3] == 1) ) //D053 "0"->"1"
		{
			rc = mpUsrWdogClear( handle[index] );
			printf("WdogClear(%d)!! [handle = %d] [rc = %d]\n",
									index, handle[index], rc);
		}
	}
}

/*--------------------------------------------------------------------------------------------------*/
static void fncwdRoutine0( MP_WDG_HANDLE lhandle )
{
	if( WriteIO(1000050, 50) != 0)
	{
		puts("Error!! (WriteIO_0)\n");
	}
	else
	{
		printf("wdRoutine0!! (WriteIO_0) [handle = %d]\n", lhandle);
	}
}

/*--------------------------------------------------------------------------------------------------*/
static void fncwdRoutine1(MP_WDG_HANDLE lhandle )
{
	if( WriteIO(1000051, 51) != 0)
	{
		puts("Error!! (WriteIO_1)\n");
	}
	else
	{
		printf("wdRoutine1!! (WriteIO_1) [handle = %d]\n", lhandle);
	}
}
/*--------------------------------------------------------------------------------------------------*/
