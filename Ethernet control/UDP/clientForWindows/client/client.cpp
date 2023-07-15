/* client.cpp : User Datagram Protocol Client */
#include   "stdafx.h"
#include   <winsock2.h>

#define PORT        11000
#define BUFF_MAX    1023

int _tmain(int argc, _TCHAR* argv[])
{
    SOCKET  sockHandle;
    struct  sockaddr_in     serverSockAddr;
    struct  sockaddr_in     clientSockAddr;
    unsigned long   serverAddr;
    int     sizeofSockAddr;
    int     bytesKeyin;
    int     bytesReceived;
    int     bytesSend;
    int     cntRcvSnd;
    int     rc;
    char    buff[BUFF_MAX + 1];
    char    keyInBuff[128];
    unsigned short  portNo;
    WSADATA data;

    printf("Simple UDP client\n");
    printf(" Please type PORT No. >" );
    fgets(keyInBuff, 128, stdin);
    portNo = atoi(keyInBuff);

    WSAStartup(MAKEWORD(2,0), &data);

    if ((sockHandle = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        printf("Error : socket() rc = %d\n", sockHandle);
        exit(1);
    }

    memset( &clientSockAddr, 0, sizeof(clientSockAddr) );
    clientSockAddr.sin_family = AF_INET;
    clientSockAddr.sin_addr.s_addr = INADDR_ANY;
    clientSockAddr.sin_port = htons(portNo);

    rc = bind(sockHandle, (struct sockaddr *)&clientSockAddr, sizeof(clientSockAddr));
    if (rc < 0)
    {
        printf("Error : bind() rc = %d\n", rc);
        goto closeSockHandle;
    }

    printf(" Please type IP addr. >" );
    fgets(keyInBuff, 128, stdin);

	memset( &serverSockAddr, 0, sizeof(serverSockAddr) );
	serverSockAddr.sin_family = AF_INET;
    serverAddr = inet_addr( keyInBuff );
    serverSockAddr.sin_addr.s_addr = serverAddr;
    serverSockAddr.sin_port = htons(portNo);
    sizeofSockAddr = sizeof(serverSockAddr);

	printf(" Please type the ASCII characters\n" );
    printf(" > ");
    cntRcvSnd = 0;
    while (1)
    {
        /* キーボード入力された文字を送信する */
        fgets(buff, BUFF_MAX, stdin);
        if(buff[0] == '\n')
        {
            break;
        }
        bytesKeyin = strlen(buff);
        bytesSend = sendto(sockHandle, buff, bytesKeyin, 0,
                      (struct sockaddr *)&serverSockAddr, sizeof(serverSockAddr));
        if (bytesSend != bytesKeyin)
        {
            printf("Error : sendto() rc = %d\n", bytesSend);
            break;
        }
        
        /* 受信データを待つ */
        cntRcvSnd++;
        printf("send(%d) %s", cntRcvSnd, buff);
        bytesReceived = recvfrom(sockHandle, buff, BUFF_MAX, 0,
                          (struct sockaddr *)&serverSockAddr, &sizeofSockAddr);
        if (bytesReceived < 0)
        {
            printf("Error : recvfrom() rc = %d\n", bytesReceived);
            break;
        }
        
        cntRcvSnd++;
        buff[bytesReceived] = '\0';
        printf("recv(%d) %s", cntRcvSnd, buff);

        if((strncmp(buff, "EXIT\n", 5) == 0)
        || (strncmp(buff, "exit\n", 5) == 0))
        {
            break;
        }
		printf(" > ");
    }
closeSockHandle:
	closesocket(sockHandle);

    WSACleanup();
    exit(0);
}
