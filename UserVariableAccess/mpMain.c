/**** mpMain.c ****/
/* Copyright 2012 YASKAWA ELECTRIC All Rights reserved. */
#include "motoPlus.h"

#define VAR_TYPE_NUM 6 // B, I, D, R, S, P
#define COORD_NUM    6 // X, Y, Z, Rx, Ry, Rz

void mp_usr_var_acc_task();
int cmp_usr_var_info(MP_USR_VAR_INFO*, MP_USR_VAR_INFO*);
int cmp_pos_var_info(MP_P_VAR_BUFF *, MP_P_VAR_BUFF *);

int nTaskID1;
char var_type_name[VAR_TYPE_NUM][8] = {
	"B",
	"I",
	"D",
	"R",
	"S",
	"P"
	};

void mpUsrRoot(int arg1, int arg2, int arg3, int arg4, int arg5,
				int arg6, int arg7, int arg8, int arg9, int arg10)
{
	nTaskID1 = mpCreateTask(MP_PRI_TIME_NORMAL, MP_STACK_SIZE,
						(FUNCPTR)mp_usr_var_acc_task, arg1, arg2,
						arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
	mpExitUsrRoot;
}

void mp_usr_var_acc_task(void)
{
	int i = CLEAR;
	STATUS status = ERROR;
	MP_USR_VAR_INFO wInfo[VAR_TYPE_NUM];
	MP_USR_VAR_INFO rInfo[VAR_TYPE_NUM];

	memset(&wInfo, 0, sizeof(wInfo));
	memset(&rInfo, 0, sizeof(rInfo));

	for (i = 0; i < VAR_TYPE_NUM; i++)
	{
		wInfo[i].var_type = rInfo[i].var_type = MK_VAR_TYPE(MP_VAR_B + i);
		wInfo[i].var_no = rInfo[i].var_no = 0;
	}

	// set data
	wInfo[0].val.b = 100;
	wInfo[1].val.i = 200;
	wInfo[2].val.d = 300;
	wInfo[3].val.r = 400.004;
	sprintf(wInfo[4].val.s, "Put S var");
	wInfo[5].val.p.dtype = MP_ROBO_DTYPE;
	wInfo[5].val.p.tool_no = 0;
	wInfo[5].val.p.fig_ctrl = (MP_FIG_SIDE | MP_FIG_R180);
	for (i = 0; i < COORD_NUM; i++)
	{
		wInfo[5].val.p.data[i] = 100 + i;
	}

	FOREVER
	{
		for (i = 0; i < VAR_TYPE_NUM; i++)
		{
			status = mpPutUserVars(&wInfo[i]);
			if (status != OK)
			{
				printf("#%s%03d Error: mpPutUserVars(): %d\n",
						var_type_name[i], wInfo[i].var_no, status);
			}

			status = mpGetUserVars(&rInfo[i]);
			if (status != OK)
			{
				printf("#%s%03d Error: mpGetUserVars(): %d\n",
						var_type_name[i], rInfo[i].var_no, status);
			}

			status = cmp_usr_var_info(&wInfo[i], &rInfo[i]);
			if (status != OK)
			{
				printf("%s var Error: %d: cmp_usr_var_info()\n",
						var_type_name[i], status);
			}
			else
			{
				printf("Compare %s variable data OK\n",
						var_type_name[i]);
			}
		}

		mpTaskDelay(5000);
	}
}

int cmp_usr_var_info(MP_USR_VAR_INFO *info1, MP_USR_VAR_INFO *info2)
{
	int ret = OK;
	int var_type = MK_VAR_TYPE(info1->var_type);

	if (info1->var_type == info2->var_type)
	{
		switch(var_type)
		{
		case MP_VAR_B:
			if (info1->val.b != info2->val.b)
			{
				ret = ERROR;
			}
			break;
		case MP_VAR_I:
			if (info1->val.i != info2->val.i)
			{
				ret = ERROR;
			}
			break;
		case MP_VAR_D:
			if (info1->val.d != info2->val.d)
			{
				ret = ERROR;
			}
			break;
		case MP_VAR_R:
			if (info1->val.r != info2->val.r)
			{
				ret = ERROR;
			}
			break;
		case MP_VAR_S:
			if (strncmp(info1->val.s, info2->val.s, STR_VAR_SIZE) != 0)
			{
				ret = ERROR;
			}
			break;
		case MP_VAR_P:
			ret = cmp_pos_var_info(&(info1->val.p), &(info2->val.p));
			break;
		default:
			break;
		}
	}
	else
	{
		ret = ERROR;
	}

	return (ret);
}

int cmp_pos_var_info(MP_P_VAR_BUFF *buf1, MP_P_VAR_BUFF *buf2)
{
	int i = CLEAR;
	int ret = OK;

	if (buf1->dtype != buf2->dtype)
	{
		ret = ERROR;
	}

	if ((buf1->dtype == MP_USER_DTYPE) &&
	    (buf1->uf_no != buf2->uf_no))
	{
		ret = ERROR;
	}

	if (buf1->tool_no != buf2->tool_no)
	{
		ret = ERROR;
	}

	if ((buf1->dtype >= MP_BASE_DTYPE) &&
	    (buf1->fig_ctrl != buf2->fig_ctrl))
	{
		ret = ERROR;
	}

	for (i = CLEAR; i < MP_GRP_AXES_NUM; i++)
	{
		if (buf1->data[i] != buf2->data[i])
		{
			ret = ERROR;
			break;
		}
	}

	return (ret);
}

