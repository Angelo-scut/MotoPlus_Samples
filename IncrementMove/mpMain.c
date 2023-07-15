//(Note that motoPlus.h should be included in every source file)
#include "motoPlus.h"

//GLOBAL DATA DEFINITIONS
int nTaskID1;
int nTaskID2;

//FUNCTION PROTOTYPES
void cmd_rcv_task(void);
void work_task(void);
void set_grp_data(void);
void set_expos_data(void);

long pos[MP_GRP_NUM];
int pos_num[MP_GRP_NUM];
int move_axes[MP_GRP_NUM];
MP_EXPOS_DATA expos;

//FUNCTION DEFINITIONS
void mpUsrRoot(int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10)
{
	nTaskID1 = mpCreateTask(MP_PRI_IO_CLK_TAKE, MP_STACK_SIZE, (FUNCPTR)cmd_rcv_task,
				arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
	nTaskID2 = mpCreateTask(MP_PRI_IP_CLK_TAKE, MP_STACK_SIZE, (FUNCPTR)work_task,
				arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
	mpExitUsrRoot;
}

void cmd_rcv_task(void)
{
	USHORT val;
	MP_IO_INFO io_info;
	MP_IO_DATA io_data;

	memset(&io_info, 0, sizeof(io_info));
	memset(&io_data, 0, sizeof(io_data));

	memset(pos, 0, (sizeof(long) * MP_GRP_NUM));
	memset(pos_num, 0, (sizeof(int) * MP_GRP_NUM));
	memset(&expos, 0, sizeof(MP_EXPOS_DATA));

	io_info.ulAddr = 10010;

	FOREVER
	{
		if (mpClkAnnounce(MP_IO_CLK) == ERROR)
		{
			puts("error: mpClkAnnounce(IO_CLK)");
			mpTaskSuspend(0);
		}

		val = 0;
		mpReadIO(&io_info, &val, 1);
		if (val == OFF)
		{
			continue;
		}

		set_grp_data();

		io_data.ulAddr = 10010;
		io_data.ulValue = OFF;
		mpWriteIO(&io_data, 1);

		io_data.ulAddr = 10011;
		io_data.ulValue = ON;
		mpWriteIO(&io_data, 1);
	}
}

void work_task(void)
{
	USHORT val;
	int i, j;
	int cnt;
	int rc;
	int flg[MP_GRP_NUM];
	MP_IO_INFO io_info;
	MP_IO_DATA io_data;
	MP_USR_VAR_INFO var_info;

	cnt = 0;
	memset(flg, 0, sizeof(flg));
	memset(&io_info, 0, sizeof(io_info));
	memset(&io_data, 0, sizeof(io_data));
	memset(&var_info, 0, sizeof(var_info));

	var_info.var_type = MK_VAR_TYPE(MP_VAR_D);

	FOREVER
	{
		if (mpClkAnnounce(MP_INTERPOLATION_CLK) == ERROR)
		{
			puts("error: mpClkAnnounce(IP_CLK)");
			mpTaskSuspend(0);
		}

		val = 0;
		io_info.ulAddr = 10011;
		mpReadIO(&io_info, &val, 1);
		if (val == OFF)
		{
			continue;
		}

		set_expos_data();

		for (i = 0; i < MP_GRP_NUM; i++)
		{
			if (cnt < pos_num[i])
			{
				for (j = 0; j < MP_GRP_AXES_NUM; j++)
				{
					if ((move_axes[i] & (1 << j)) == OFF)
					{
						continue;
					}

					expos.grp_pos_info[i].pos[j] = pos[i];
				}

				flg[i] = 1;
			}
			else
			{
				expos.grp_pos_info[i].pos[0] = 0;
				flg[i] = 0;
			}
		}

		if ((flg[0] != 0) || (flg[1] != 0) || 
		    (flg[2] != 0) || (flg[3] != 0))
		{
			rc = mpExRcsIncrementMove(&expos);
			var_info.var_no = 0;
			var_info.val.d = rc;
			mpPutUserVars(&var_info);
			printf("rc = %d\n", rc);

			if (rc == OK)
			{
				cnt++;
			}
		}
		else
		{
			cnt = 0;
			io_data.ulAddr = 10011;
			io_data.ulValue = OFF;
			mpWriteIO(&io_data, 1);
		}
	}
}

void set_grp_data(void)
{
	int i;
	int ctrl_grp;
	MP_USR_VAR_INFO var_info;

	memset(&var_info, 0, sizeof(var_info));
	var_info.var_type = MK_VAR_TYPE(MP_VAR_D);

	var_info.var_no = 2;
	mpGetUserVars(&var_info);
	expos.ctrl_grp = var_info.val.d;

	var_info.var_no = 3;
	mpGetUserVars(&var_info);
	expos.m_ctrl_grp = var_info.val.d;

	var_info.var_no = 4;
	mpGetUserVars(&var_info);
	expos.s_ctrl_grp = var_info.val.d;

	ctrl_grp = expos.ctrl_grp | expos.s_ctrl_grp;
	for (i = 0; i < MP_GRP_NUM; i++)
	{
		if ((ctrl_grp & (1 << i)) == OFF)
		{
			continue;
		}

		var_info.var_no = (10 * (i + 1));
		mpGetUserVars(&var_info);
		expos.grp_pos_info[i].pos_tag.data[0] = var_info.val.d;
	}
}

void set_expos_data(void)
{
	int i;
	int ctrl_grp;
	MP_USR_VAR_INFO var_info;

	memset(&var_info, 0, sizeof(var_info));
	var_info.var_type = MK_VAR_TYPE(MP_VAR_D);

	ctrl_grp = expos.ctrl_grp | expos.s_ctrl_grp;
	for (i = 0; i < MP_GRP_NUM; i++)
	{
		if ((ctrl_grp & (1 << i)) == OFF)
		{
			continue;
		}

		var_info.var_no = ((10 * (i + 1)) + 1);
		mpGetUserVars(&var_info);
		expos.grp_pos_info[i].pos_tag.data[2] = var_info.val.d;

		var_info.var_no = ((10 * (i + 1)) + 2);
		mpGetUserVars(&var_info);
		switch (var_info.val.d)
		{
		case 0:
			expos.grp_pos_info[i].pos_tag.data[3] = MP_INC_PULSE_DTYPE;
			break;
		case 1:
			expos.grp_pos_info[i].pos_tag.data[3] = MP_INC_ANGLE_DTYPE;
			break;
		case 2:
			expos.grp_pos_info[i].pos_tag.data[3] = MP_INC_BF_DTYPE;
			break;
		case 3:
			expos.grp_pos_info[i].pos_tag.data[3] = MP_INC_RF_DTYPE;
			break;
		case 4:
			expos.grp_pos_info[i].pos_tag.data[3] = MP_INC_TF_DTYPE;
			break;
		case 5:
			expos.grp_pos_info[i].pos_tag.data[3] = MP_INC_UF_DTYPE;
			break;
		default:
			expos.grp_pos_info[i].pos_tag.data[3] = 0;
			break;
		}

		var_info.var_no = ((10 * (i + 1)) + 3);
		mpGetUserVars(&var_info);
		expos.grp_pos_info[i].pos_tag.data[4] = var_info.val.d;

		var_info.var_no = ((10 * (i + 1)) + 4);
		mpGetUserVars(&var_info);
		move_axes[i] = var_info.val.d;

		var_info.var_no = ((10 * (i + 1)) + 5);
		mpGetUserVars(&var_info);
		pos[i] = var_info.val.d;

		var_info.var_no = ((10 * (i + 1)) + 6);
		mpGetUserVars(&var_info);
		pos_num[i] = var_info.val.d;
	}
}
