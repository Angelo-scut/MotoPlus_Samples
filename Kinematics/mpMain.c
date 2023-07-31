/* mp_main.c - MotoPlus Test Application for Real Time Process */
#include "motoPlus.h"

void mpTask1();

//GLOBAL DATA DEFINITIONS
int nTaskID1;

void mpUsrRoot(int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10)
{	
	//TODO: Add additional intialization routines.

	//Creates and starts a new task in a seperate thread of execution.
	//All arguments will be passed to the new task if the function
	//prototype will accept them.

	nTaskID1 = mpCreateTask(MP_PRI_TIME_NORMAL, MP_STACK_SIZE, (FUNCPTR)mpTask1,
						arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
	//Ends the initialization task.
	mpExitUsrRoot;
}

void mpTask1(void)
{
	int i;
	int rc;
	int err_counter;
	int grp_no;
	long pulse[MP_GRP_AXES_NUM];
	long angle[MP_GRP_AXES_NUM];
	MP_COORD rob_coord;
	MP_COORD bas_coord;
	MP_COORD coord;
	MP_FRAME rob_frame = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
	MP_FRAME bas_frame = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
	MP_FRAME frame = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
	MP_CTRL_GRP_SEND_DATA sData;
	MP_PULSE_POS_RSP_DATA rPls_data;
	MP_FB_PULSE_POS_RSP_DATA rFB_data;

	err_counter = 0;
	printf("Sample Kinematics test\n");

	while (1)
	{
		printf("\n--Robot minute movement--\n");
		sData.sCtrlGrp = 0;
		memset(&rFB_data, 0, sizeof(rFB_data));
		mpGetFBPulsePos(&sData, &rFB_data);  // 返回feedback的位置，即当前位置，对应的mpGetPulsePos()返回的是命令的位置

		grp_no = mpCtrlGrpId2GrpNo(MP_R1_GID);

		memset(pulse, 0, sizeof(pulse));
		printf("mpConvFBPulseToPulse()\n");
		while ((rc = mpConvFBPulseToPulse(grp_no, rFB_data.lPos, pulse)) < 0)  // 将反馈脉冲转成算术脉冲？有什么区别？
		{
			err_counter++;
			if (rc != E_KINEMA_FAILURE)
			{
				printf("function err\n");
				return;
			}

			mpTaskDelay(1);
		}
		printf("pulse = {");
		for (i = 0; i < MP_GRP_AXES_NUM; i++)
		{
			printf("%ld", pulse[i]);

			if (i != (MP_GRP_AXES_NUM - 1))
			{
				printf(", ");
			}
			else
			{
				printf("}\n");
			}
		}

		printf("mpConvPulseToAngle()\n");
		memset(angle, 0, sizeof(angle));
		while ((rc = mpConvPulseToAngle(grp_no, pulse, angle)) < 0)  // 将位置脉冲转换成各轴的角度值
		{
			err_counter++;
			if (rc != E_KINEMA_FAILURE)
			{
				printf("function err\n");
				return;
			}

			mpTaskDelay(1);
		}
		printf("angle = {");
		for (i = 0; i < MP_GRP_AXES_NUM; i++)
		{
			printf("%ld", angle[i]);

			if (i != (MP_GRP_AXES_NUM - 1))
			{
				printf(", ");
			}
			else
			{
				printf("}\n");
			}
		}

		printf("mpConvAxesToCartPos()\n");
		memset(&rob_coord, 0, sizeof(rob_coord));
		while ((rc = mpConvAxesToCartPos(grp_no, angle, 0, NULL, &rob_coord)) < 0)  // 将轴的角度转换成笛卡尔坐标（机器人坐标系）
		{
			err_counter++;
			if (rc != E_KINEMA_FAILURE)
			{
				printf("function err\n");
				return;
			}

			mpTaskDelay(1);
		}
		printf("rob_coord = {%ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld} \n", 
				     rob_coord.x, rob_coord.y, rob_coord.z, rob_coord.rx, 
				     rob_coord.ry, rob_coord.rz, rob_coord.ex1, rob_coord.ex2);

		printf("mpConvCartPosToAxes()\n");
		do {
			while (mpConvCartPosToAxes(grp_no, &rob_coord, 0, 0, angle, MP_KINEMA_DELTA, angle) < 0)
			{
				err_counter++;
				if (rc != E_KINEMA_FAILURE)
				{
					printf("function err\n");
					return;
				}

				mpTaskDelay(1);
			}

			printf("angle = {");
			for (i = 0; i < MP_GRP_AXES_NUM; i++)
			{
				printf("%ld", angle[i]);

				if (i != (MP_GRP_AXES_NUM - 1))
				{
					printf(", ");
				}
				else
				{
					printf("} \n");
				}
			}
		} while ((rob_coord.x = (rob_coord.x + (1 * MP_DIST_UNIT))) < (500 * MP_DIST_UNIT));

		printf("err_counter = %d\n", err_counter);

		printf("\n--base coordinates position--\n");
		sData.sCtrlGrp = 8;
		memset(&rPls_data, 0, sizeof(rPls_data));
		mpGetPulsePos(&sData, &rPls_data);

		if ((grp_no = mpCtrlGrpId2GrpNo(MP_B1_GID)) >= 0)
		{
			printf("mpConvPulseToAngle()\n");
			memset(angle, 0, sizeof(angle));
			while ((rc = mpConvPulseToAngle(grp_no, rPls_data.lPos, angle)) < 0)
			{
				err_counter++;
				if (rc != E_KINEMA_FAILURE)
				{
					printf("function err\n");
					return;
				}

				mpTaskDelay(1);
			}
			printf("angle = {");
			for (i = 0; i < MP_GRP_AXES_NUM; i++)
			{
				printf("%ld", angle[i]);

				if (i != (MP_GRP_AXES_NUM - 1))
				{
					printf(", ");
				}
				else
				{
					printf("}\n");
				}
			}

			printf("mpConvAxesToCartPos()\n");
			memset(&bas_coord, 0, sizeof(bas_coord));
			while ((rc = mpConvAxesToCartPos(grp_no, angle, 0, NULL, &bas_coord)) < 0)
			{
				err_counter++;
				if (rc != E_KINEMA_FAILURE)
				{
					printf("function err\n");
					return;
				}

				mpTaskDelay(1);
			}
			printf("bas_coord = {%ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld} \n", 
					     bas_coord.x, bas_coord.y, bas_coord.z, bas_coord.rx, 
					     bas_coord.ry, bas_coord.rz, bas_coord.ex1, bas_coord.ex2);

			printf("mpZYXeulerToFrame()\n");
			while ((rc = mpZYXeulerToFrame(&rob_coord, &rob_frame)) < 0)
			{
				err_counter++;
				printf("function err\n");
				return;
			}
			printf("rob_frame = {%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f} \n", 
					     rob_frame.nx, rob_frame.ny, rob_frame.nz, rob_frame.ox, 
					     rob_frame.oy, rob_frame.oz, rob_frame.ax, rob_frame.ay, 
					     rob_frame.az, rob_frame.px, rob_frame.py, rob_frame.pz);

			while ((rc = mpZYXeulerToFrame(&bas_coord, &bas_frame)) < 0)
			{
				err_counter++;
				printf("function err\n");
				return;
			}
			printf("bas_frame = {%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f} \n", 
					     bas_frame.nx, bas_frame.ny, bas_frame.nz, bas_frame.ox, 
					     bas_frame.oy, bas_frame.oz, bas_frame.ax, bas_frame.ay, 
					     bas_frame.az, bas_frame.px, bas_frame.py, bas_frame.pz);

			printf("mpMulFrame()\n");
			while ((rc = mpMulFrame(&bas_frame, &rob_frame, &frame)) < 0)
			{
				err_counter++;
				printf("function err\n");
				return;
			}
			printf("Mul_frame = {%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f} \n", 
					     frame.nx, frame.ny, frame.nz, frame.ox, frame.oy, frame.oz, 
					     frame.ax, frame.ay, frame.az, frame.px, frame.py, frame.pz);

			printf("mpFrameToZYXeuler()\n");
			memset(&coord, 0, sizeof(coord));
			while ((rc = mpFrameToZYXeuler(&frame, &coord)) < 0)
			{
				err_counter++;
				printf("function err\n");
				return;
			}
			printf("base_pos = {%ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld} \n", 
					    coord.x, coord.y, coord.z, coord.rx, 
					    coord.ry, coord.rz, coord.ex1, coord.ex2);
		}

		printf("err_counter = %d\n", err_counter);
		err_counter = 0;

		mpTaskDelay(10000);
	}
}
