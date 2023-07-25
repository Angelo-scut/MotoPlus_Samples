/* TcpSvr2.c */
/* Copyright 2009 YASKAWA ELECTRIC All Rights reserved. */

#include "motoPlus.h"

// for API & FUNCTIONS
void moto_plus0_task(void);
void ap_TCP_Sserver(ULONG portNo);
void sensSomeWorkTask(char *string);
#define PORT        11000
#define BUFF_MAX    1023

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

int command_no;

void moto_plus0_task(void)
{
    puts("Activate moto_plus0_task!");

    ap_TCP_Sserver(PORT);

    mpSuspendSelf;
}

char *strupr(char *string);

void ap_TCP_Sserver(ULONG portNo)
{
    int     sockHandle;
    struct  sockaddr_in     serverSockAddr;
    int     rc;
	MP_CTRL_GRP_SEND_DATA mp_ctrl_grp_send_data;
	MP_CART_POS_RSP_DATA mp_cart_pos_rsp_data;

    /*printf("Simple TCP server\n");*/

    sockHandle = mpSocket(AF_INET, SOCK_STREAM, 0);
    if (sockHandle < 0)
        return;

    memset(&serverSockAddr, 0, sizeof(serverSockAddr));
    serverSockAddr.sin_family = AF_INET;
    serverSockAddr.sin_addr.s_addr = INADDR_ANY;
    serverSockAddr.sin_port = mpHtons(portNo);

    rc = mpBind(sockHandle, (struct sockaddr *)&serverSockAddr, sizeof(serverSockAddr)); 
    if (rc < 0)
        goto closeSockHandle;

    rc = mpListen(sockHandle, SOMAXCONN);
    if (rc < 0)
        goto closeSockHandle;

    while (1)
    {
        int     acceptHandle;
        struct  sockaddr_in     clientSockAddr;
        int     sizeofSockAddr;

        memset(&clientSockAddr, 0, sizeof(clientSockAddr));
        sizeofSockAddr = sizeof(clientSockAddr);

        acceptHandle = mpAccept(sockHandle, (struct sockaddr *)&clientSockAddr, &sizeofSockAddr);

        if (acceptHandle < 0)
            break;

        while( 1 )
        {
            int     bytesRecv;
            int     bytesSend;
			int     i;
			int		j;
			int     k;
			int     len;
            char    buff[BUFF_MAX + 1];
			char    val[BUFF_MAX + 1];
			char	location[BUFF_MAX + 1];
//			char	separator[20];
			long	position;

			//初始化
            memset(buff, 0, sizeof(buff));
			memset(val, 0, sizeof(val));
			memset(location, 0, sizeof(location));
//			memset(separator, 0, sizeof(separator));

			//接收TCP通讯
            bytesRecv = mpRecv(acceptHandle, buff, BUFF_MAX, 0);

			//接收不成功，退出
            if (bytesRecv < 0)
                break;

            /* 受信データを大文字に変換して送り返す */
            /*strupr(buff);*/
//            command_no=COMMAND_0;
			//判断发送来的信息的类型（功能#种类#数值）
//			len = strlen(buff);
////			command_no = buff[0];
//			//偏差类型提取即移动哪个轴
//
//			//偏差数值提取
//			k=0;
//			for (i = 4; i < len; i++) {
//				val[k++]=buff[i];
//			}

			//返回坐标系
			mp_ctrl_grp_send_data.sCtrlGrp = 0;	//  指定第一个机器人
			if (mpGetCartPos(&mp_ctrl_grp_send_data, &mp_cart_pos_rsp_data) != 0)//错误则退出循环
			{
				break;
			}
//			separator = '#';
			memset(buff, 0, sizeof(buff));
			k = 0;
			for (j = 0; j < 6; j++) {
				memcpy(position, &mp_cart_pos_rsp_data.lPos[j], (sizeof(long) * 6));
				memset(location, 0, sizeof(location));
				while (position) {
					location[k++] = position % 10 + '0';
					position = position / 10;
				}
//				strcat(buff, location);
//				if(j!=5)strcat(buff, '#');
				location[k++]='#';
			}
			for(j=0;j<sizeof(location);j++){
				buff[j]=location[j];
			}
			//(纠偏功能)功能
//			sensSomeWorkTask(val);
			
            bytesSend = mpSend(acceptHandle, buff, BUFF_MAX, 0);

//            if (bytesSend != bytesRecv)
//                break;

            if (strncmp(buff, "EXIT", 4) == 0 || strncmp(buff, "exit", 4) == 0)
                break;
        }
        mpClose(acceptHandle);
    }
closeSockHandle:
    mpClose(sockHandle);

    return;
}

//char *strupr(char *string)
//{
//    int		i;
//    int     len;
//
//    len = strlen(string);
//	for (i=0; i < len; i++)
//	{
//	    if (isalpha((unsigned char)string[i]))
//	    {
//		    string[i] = toupper(string[i]);
//        }
//	}
//    return (string);
//}
//Command Receive Task

void sensSomeWorkTask(char *string)
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
		int endProcess = 0;

		switch(command_no)
		{
			// Unknown
			case COMMAND_0:
				endProcess=1;
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
					CorrPath_cnt++;
					dy=atoi(string);
					if(CorrPath_cnt > 10)
					{
//						command_no = COMMAND_0;
						CorrPath_cnt = 0;
						endProcess = 1;
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
		if (endProcess==1) {
			break;
		}
	}
}