/* TcpSvr2.c */
/* Copyright 2009 YASKAWA ELECTRIC All Rights reserved. */

#include "motoPlus.h"

// for API & FUNCTIONS
void moto_plus0_task(void);
void ap_TCP_Sserver(ULONG portNo);
#define PORT        11000
#define BUFF_MAX    1023

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

    printf("Simple TCP server\n");

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
            char    buff[BUFF_MAX + 1];

            memset(buff, 0, sizeof(buff));

            bytesRecv = mpRecv(acceptHandle, buff, BUFF_MAX, 0);

            if (bytesRecv < 0)
                break;

            /* 受信データを大文字に変換して送り返す */
            strupr(buff);
            bytesSend = mpSend(acceptHandle, buff, bytesRecv, 0);

            if (bytesSend != bytesRecv)
                break;

            if (strncmp(buff, "EXIT", 4) == 0 || strncmp(buff, "exit", 4) == 0)
                break;
        }
        mpClose(acceptHandle);
    }
closeSockHandle:
    mpClose(sockHandle);

    return;
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
