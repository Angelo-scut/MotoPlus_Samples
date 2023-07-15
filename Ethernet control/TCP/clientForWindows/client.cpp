/* client.cpp : Transmission Control Protocol Client */
#include   "stdafx.h"
#include   <winsock2.h>

#define PORT        11000
#define BUFF_MAX    1023

int _tmain(int argc, _TCHAR* argv[])
{
    SOCKET  sockHandle;
    struct  sockaddr_in     serverSockAddr;
    int     cntRcvSnd;
    int     rc;
    WSADATA data;
    char    keyInBuff[128];
    unsigned short  portNo;

    printf("Simple TCP client\n");
    printf(" Please type PORT No. >");
    fgets(keyInBuff, 128, stdin);
    portNo = atoi(keyInBuff);

    WSAStartup(MAKEWORD(2,0), &data);

    if ((sockHandle = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Error : socket() rc = %d\n", sockHandle);
        exit(1);
    }

    memset(&serverSockAddr, 0, sizeof(serverSockAddr));
    serverSockAddr.sin_family = AF_INET;

    printf(" Please type IP addr. >");
    fgets(keyInBuff, 128, stdin);
    serverSockAddr.sin_addr.s_addr = inet_addr(keyInBuff);
    serverSockAddr.sin_port = htons(portNo);

    rc = connect(sockHandle, (struct sockaddr *)&serverSockAddr, sizeof(serverSockAddr));
	if (rc < 0)
    {
        printf("Error : connect() rc = %d\n", rc);
        goto closeSockHandle;
    }

    printf(" Please type the ASCII characters\n");
    printf(" > ");
    cntRcvSnd = 0;
    while (1)
    {
        int     bytesKeyin;
        int     bytesSend;
        int     bytesRecv;
        char    buff[BUFF_MAX + 1];

        /* キーボード入力された文字を送信する */
        fgets(buff, BUFF_MAX, stdin);
        bytesKeyin = strlen(buff);

        bytesSend = send(sockHandle, buff, bytesKeyin, 0);
        if (bytesSend != bytesKeyin)
        {
            printf("Error : send() rc = %d\n", bytesSend);
            break;
        }
        cntRcvSnd++;
        printf("send(%d) : %s", cntRcvSnd, buff);
        
        /* 受信データを待つ */
        bytesRecv = recv(sockHandle, buff, bytesSend, 0);
        if (bytesRecv < 0)
        {
            printf("Error : recv() rc = %d\n", bytesRecv);
            break;
        }
        
        cntRcvSnd++;
        buff[bytesRecv] = '\0';
        printf("recv(%d) : %s", cntRcvSnd, buff);

    	if ((strncmp(buff, "EXIT\n", 5) == 0) || (strncmp(buff, "exit\n", 5) == 0))
        {
            break;
        }

        printf(" > ");
    }
closeSockHandle:
    if (sockHandle >= 0)
    {
        closesocket(sockHandle);
    }
    WSACleanup();
    exit(0);
}
