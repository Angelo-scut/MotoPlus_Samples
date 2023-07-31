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

// #include "motoPlus.h"
#include "MacroDefine.h"

void tcp_server_task(void);
void ap_TCP_Sserver(ULONG portNo);

void tcp_server_task(void){
    puts("Activate TCP server!");

    ap_TCP_Sserver(PORT);

    mpSuspendSelf;  // 任务结束后挂起
}

BOOL encoding(char *pos);
void decoding(char *msg);

// 写个屁帮助文档，写个readme.md就算看得起自己了
// const char* help_str = "help/HELP: help document.\n The number of position argments must be 6, and speed must be 1. \n Except that corr_path and corr_speed(\%) must be in relative type, all the other arguments must be in absolutary type.\n Commands include 'line', 'line_vec',";

extern SEM_ID semid;
MP_CART_POS_RSP_DATA mp_cart_pos_rsp_data;  // 用于接收笛卡尔坐标的信息
MP_POSVAR_DATA pos_data;  // 用于设置用户变量中的位置变量
int command_no = 0;
long speed = 10000;  // 100% 速度
long offset[6];
BOOL sensor_flag = FALSE;

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

    // 用户数据初始化
    memset(&pos_data.ulValue[0], 0, sizeof(LONG) * 10);
    pos_data.usIndex = 10; // 修改的是P0010这个位置的位置变量
    pos_data.ulValue[0] |= 0x0010;  // 设置坐标轴为基座的笛卡尔坐标系

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

            

            // 若没有要退出，则返回现在机器人位姿，TODO:这里要注意时序，如果client没有处于listen状态，这里的发送是否会丢包？
            if(encoding(send_buff)){
                bytesSend = mpSend(acceptHandle, send_buff, strlen(send_buff), 0);
                if (bytesSend < 0)
                    break;
                
                decoding(recv_buff);  // 是否解码成功，解码成功还要进行安全判断,安全判断直接给到设置那边去
            }
                
            // if (bytesSend != bytesRecv)
                // break;
            
        }
        mpClose(acceptHandle);
    }
closeSockHandle:
    mpClose(sockHandle);

    return;
}

BOOL encoding(char *pos){
    MP_CTRL_GRP_SEND_DATA mp_ctrl_grp_send_data;  // 当时机器人群组的时候，可以通过这个控制返回哪个机器人的信息
    mp_ctrl_grp_send_data.sCtrlGrp = 0; // 只取机器人1的信息，因为只有一台机器人

    if (mpGetCartPos(&mp_ctrl_grp_send_data, &mp_cart_pos_rsp_data) != 0)  // TODO:是否需要每次都对mp_cart_pos_rsp_data进行初始化
    {
        puts("get cart pos error!");
        return FALSE;
    }

    char temp_str[20];
    // memset(pos, 0, sizeof(pos));  // 这里的sizeof不对哦，这里只会返回pos的地址占位数,所以只能在函数外初始化
    memset(temp_str, 0, sizeof(temp_str));
    int cur_idx = 0;  // 记录pos当前写入位置,一般都不可能超过1024
    int i = 0;
    for (; i < MAX_CART_AXES; i++)
    {
        LONG temp_pos = mp_cart_pos_rsp_data.lPos[i];
        int idx = 0;
        while (temp_pos)
        {
            temp_str[idx++] = temp_pos % 10 + '0';
            temp_pos /= 10;
        }
        int j = idx - 1;
        for (; j > -1; j--)
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


void decoding(char *msg){
    int msg_len = strlen(msg);
    long data[10];
    memset(&data[0], 0, (sizeof(long) * 10));
    // memset(&pos_data.ulValue[0], 0, (sizeof(long) * 10));
    char *token = strtok(msg, ":");
    command_no = atoi(token);
    token = strtok(msg, ",");
    int cur_idx = 0;
    
    while (token != NULL && cur_idx < 10)
    {
        // pos_data.ulValue[cur_idx++] = atoi(token);  // 如果是其他字符，即不能转成int类型的，则会返回0
        data[cur_idx++] = atoi(token);
        token = strtok(NULL, ",");  // 因为这个函数用到了静态缓存 static char* buffer, 但是要继续切分就不能输入s
        
    }

    if (token != NULL) return;  // 如果数据位超过了10位，直接丢弃

    switch (command_no)
    {
    case COMMAND_LINE:
        if (cur_idx == 6)  // 如果发过来的不是6个位姿的话，TODO:如果要加入速度这里需要修改一下,决定不直接修改速度了，要修改直接在示教器上修改
        {
            memcpy(&pos_data.ulValue[2], &data[0], (sizeof(long) * 6));
            pos_data.usIndex = POS_VAR_LINE;
            mpSemGive(semid);  // 发射信号，告诉处理的线程已经接收到新的位置信息了
        }
        break;
    case COMMAND_LINE_VEC:  // 应当由PC完成计算偏移量
        if (cur_idx == 6)
        {
            memcpy(&pos_data.ulValue[2], &data[0], (sizeof(long) * 6));
            pos_data.usIndex = POS_VAR_LINE_VEC;
            mpSemGive(semid);
        }
        break;
    case COMMAND_CORRPATH:
        if (cur_idx == 6)
        {
            memcpy(&offset[0], &data[0], (sizeof(long) * 6));
            sensor_flag = TRUE;
        }
        break;
    case COMMAND_FRCPATH_END:
        if (cur_idx == 0)
        {
            sensor_flag = TRUE;
        }
        break;
    case COMMAND_SPD_OVR_ON:
        if (cur_idx == 1)
        {
            speed = data[0];
            sensor_flag = TRUE;
        }
    case COMMAND_SPD_OVR_OFF:
        if (cur_idx == 0)
        {
            sensor_flag = TRUE;
        }
        break;
    case COMMAND_CIRCLE_PC:
        if (cur_idx == 7 && data[0] < 3)
        {
            memcpy(&pos_data.ulValue[2], &data[1], (sizeof(long) * 6));
            pos_data.usIndex = POS_VAR_CIRCLE_PC + data[0];
            mpSemGive(semid);
        }
        
    default:
        break;
    }

    return;
    
}
