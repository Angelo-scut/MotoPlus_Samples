/**
 * @file TcpSvr.c
 * @author Zishun Wang
 * @brief TCP server in YASKAWA, receiving position information from PC TCP client
 * @tcpcoding "x,y,z,a,b,c,speed" corrspond to position x,y,z and angle of x,y,z
 * @version 0.1
 * @date 2023-07-24
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "motoPlus.h"

void tcp_server_task(void);
void ap_TCP_Sserver(ULONG portNo);

#define PORT        11000  // 端口号
#define BUFF_MAX    1023  // 接收数据的BUFF

void tcp_server_task(void){
    puts("Activate TCP server!");

    ap_TCP_Sserver(PORT);

    mpSuspendSelf;  // 任务结束后挂起
}

BOOL current_position(char *pos);
MP_POSVAR_DATA decoding(char *msg);

extern SEM_ID semid;

void ap_TCP_Sserver(ULONG portNo){
    int     sockHandle;
    struct  sockaddr_in     serverSockAddr;
    int     rc;

    sockHandle = mpSocket(AF_INET, SOCK_STREAM, 0);
    if (sockHandle < 0)
        return;

    memset(&serverSockAddr, 0, sizeof(serverSockAddr));
    serverSockAddr.sin_family = AF_INET;
    serverSockAddr.sin_addr.s_addr = INADDR_ANY;
    serverSockAddr.sin_port = mpHtons(portNo);

    // 将handle和socket地址绑定
    rc = mpBind(sockHandle, (struct sockaddr *)&serverSockAddr, sizeof(serverSockAddr)); 
    if (rc < 0)
        goto closeSockHandle;

    // 等待客户端连接
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
            char    recv_buff[BUFF_MAX + 1];
            char    send_buff[BUFF_MAX + 1];

            memset(recv_buff, 0, sizeof(recv_buff));
            memset(send_buff, 0, sizeof(send_buff));

            bytesRecv = mpRecv(acceptHandle, recv_buff, BUFF_MAX, 0);  // 等待接收来自client的字符

            if (bytesRecv < 0)
                break;

            if (strncmp(recv_buff, "EXIT", 4) == 0 || strncmp(recv_buff, "exit", 4) == 0)  // 如果client发送退出命令，则退出
                break;

            MP_POSVAR_DATA data_from_client = decoding(recv_buff);

            // 若没有要退出，则返回现在机器人位姿，TODO:这里要注意时序，如果client没有处于listen状态，这里的发送是否会丢包？
            if(current_position(send_buff))
                bytesSend = mpSend(acceptHandle, send_buff, bytesRecv, 0);

            if (bytesSend != bytesRecv)
                break;

            mpSemGive(semid);  // 发射信号，告诉处理的线程已经接收到新的位置信息了
            
        }
        mpClose(acceptHandle);
    }
closeSockHandle:
    mpClose(sockHandle);

    return;
}

BOOL current_position(char *pos){
    MP_CTRL_GRP_SEND_DATA mp_ctrl_grp_send_data;  // 当时机器人群组的时候，可以通过这个控制返回哪个机器人的信息
    MP_CART_POS_RSP_DATA mp_cart_pos_rsp_data;  // 用于接收笛卡尔坐标的信息

    mp_ctrl_grp_send_data.sCtrlGrp = 0; // 只取机器人1的信息，因为只有一台机器人

    if (mpGetCartPos(&mp_ctrl_grp_send_data, &mp_cart_pos_rsp_data) != 0)
    {
        puts("get cart pos error!");
        return FALSE;
    }

    char temp_str[20];
    memset(pos, 0, sizeof(pos));
    memset(temp_str, 0, sizeof(temp_str));
    int cur_idx = 0;  // 记录pos当前写入位置,一般都不可能超过1024
    for (size_t i = 0; i < MAX_CART_AXES; i++)
    {
        LONG temp_pos = mp_cart_pos_rsp_data.lPos[i];
        int idx = 0;
        while (temp_pos)
        {
            temp_str[idx++] = temp_pos % 10 + '0';
            temp_pos /= 10;
        }
        for (size_t j = idx - 1; j > -1; j--)
        {
            pos[cur_idx++] = temp_str[j];
        }
        
        if (i < MAX_CART_AXES - 1)
        {
            pos[cur_idx++] = ',';
        }
        
    }

    return TRUE;
}


MP_POSVAR_DATA decoding(char *msg){
    MP_POSVAR_DATA pos_data;
    int msg_len = strlen(msg);
    memset(&pos_data.ulValue[0], 0, (sizeof(long) * 10));
    char *token = strtok(msg, ",");
    pos_data.ulValue[2] = atoi(token);
    int cur_idx = 3;
    
    while (token != NULL)
    {
        token = strtok(NULL, ",");
        pos_data.ulValue[cur_idx] = atoi(token);
        cur_idx++;
    }

    return pos_data;
    
}
