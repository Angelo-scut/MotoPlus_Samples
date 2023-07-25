/**** TaskCtl2.c ****/
/* Copyright 2009 YASKAWA ELECTRIC All Rights reserved. */

#include "motoPlus.h"

// for API & FUNCTIONS
extern void moto_plus0_task(void);
extern void moto_plus1_task(void);

// for DATA
extern SEM_ID semid;  // 外部定义的semid，跨了个文件

void moto_plus0_task(void)
{
	puts("Activate moto_plus0_task!");

	while (1)
	{
		mpSemGive(semid);  // 发出信号，通过之前定义的信号槽
		mpTaskDelay(2500);  // 暂停2.5s
	}
}

void moto_plus1_task(void)
{
	STATUS status;
	unsigned int run_cnt;

	//float get_time;

	puts("Activate moto_plus1_task!");

	run_cnt = 0;
	while (1)
	{
		run_cnt++;
		status = mpSemTake(semid, WAIT_FOREVER);  // 一直等待信号的发出，收到后才退出，问题是收到了什么鬼信号？
		if (status == ERROR)
		{
			printf("semTake Error![%d]\n", run_cnt);
		}
		else
		{
			printf("moto_plus1_task Running![%d]\n", run_cnt);
		}

	}
}
