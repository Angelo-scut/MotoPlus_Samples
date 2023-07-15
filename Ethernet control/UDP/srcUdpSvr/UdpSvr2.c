/* UdpSvr2.c */
/* Copyright 2009 YASKAWA ELECTRIC All Rights reserved. */

#include "motoPlus.h"

// for API & FUNCTIONS
void moto_plus0_task(void);
void ap_UDP_Sserver(ULONG portNo);
#define PORT        11000
#define BUFF_MAX    1023

void moto_plus0_task(void)
{
    puts("Activate moto_plus0_task!");

    ap_UDP_Sserver(PORT);

    mpSuspendSelf;
}

char *strupr(char *string);

void ap_UDP_Sserver(ULONG portNo)
{
    int     sockHandle;
    struct  sockaddr_in     serverSockAddr;
    struct  sockaddr_in     clientSockAddr;
    int     sizeofSockAddr;
    int     rc;

    printf("Simple UDP server\n");

    while(1)
    {
        sockHandle = mpSocket(AF_INET, SOCK_DGRAM, 0);
        if (sockHandle < 0)
            return;

        memset(&serverSockAddr, 0, sizeof(serverSockAddr));
        serverSockAddr.sin_family = AF_INET;
        serverSockAddr.sin_addr.s_addr = INADDR_ANY;
        serverSockAddr.sin_port = mpHtons(portNo);

        rc = mpBind(sockHandle, (struct sockaddr *)&serverSockAddr, sizeof(serverSockAddr)); 
        if (rc < 0)
            goto closeSockHandle;
        
        memset(&clientSockAddr, 0, sizeof(clientSockAddr));
        sizeofSockAddr = sizeof(clientSockAddr);

        while (1)
        {
            int     bytesRecv;
            int     bytesSend;
            char    buff[BUFF_MAX + 1];

            memset(buff, 0, sizeof(buff));

            bytesRecv = mpRecvFrom(sockHandle, buff, BUFF_MAX, 0,
                              (struct sockaddr *)&clientSockAddr, &sizeofSockAddr);

            if (bytesRecv < 0)
                break;

            /* 受信データを大文字に変換して送り返す */
            strupr(buff);
            bytesSend = mpSendTo(sockHandle, buff, bytesRecv, 0,
                          (struct sockaddr *)&clientSockAddr, sizeof(clientSockAddr));

            if (bytesSend != bytesRecv)
                break;

            if (strncmp(buff, "EXIT\n", 5) == 0 || strncmp(buff, "exit\n", 5) == 0)
                break;
        }
closeSockHandle:
        mpClose(sockHandle);
    }
}

char *strupr(char *string)
{
    int		i;
    int     len;

    len = strlen(string);
	for (i=0; i < len; i++)
	{
	    if (isalpha((unsigned char)string[i]))
	    {
		    string[i] = toupper(string[i]);
        }
	}
    return (string);
}
