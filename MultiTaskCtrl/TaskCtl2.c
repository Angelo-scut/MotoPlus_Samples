/**** TaskCtl2.c ****/
/* Copyright 2009 YASKAWA ELECTRIC All Rights reserved. */

#include "motoPlus.h"

// for API & FUNCTIONS
extern void moto_plus0_task(void);
extern void moto_plus1_task(void);

// for DATA
extern SEM_ID semid;

void moto_plus0_task(void)
{
	puts("Activate moto_plus0_task!");

	while (1)
	{
		mpSemGive(semid);
		mpTaskDelay(2500);
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
		status = mpSemTake(semid, WAIT_FOREVER);
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
