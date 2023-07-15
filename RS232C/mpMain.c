//mpMain.c
//
//This contains mpUsrRoot which is the entry point for your MotoPlus application

//ADDITIONAL INCLUDE FILES 
//(Note that MotoPlus.h should be included in every source file)
#include "motoPlus.h"

#define MP_BUFF_SIZE    1024

//GLOBAL DATA DEFINITIONS
int nTaskID1;

//FUNCTION PROTOTYPES
void mpTask1(void);
char* strupr(char *string);

//FUNCTION DEFINITIONS
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
	//TODO: Add the code for this task
    MP_RS_CONFIG    rs_config;
    int             rc;
    ULONG           snd_len;
    CHAR            snd_buff[MP_BUFF_SIZE];
    CHAR            rcv_buff[MP_BUFF_SIZE];
    ULONG           rcv_len;
    LONG            portHandle;

    printf("Simple RS232C test\n");

    rs_config.dataBit = mpRsDataBit_8;
    rs_config.stopBit = mpRsStopBit_one;
    rs_config.parity = mpRsParity_even;
    rs_config.baudRate = mpRsBaudrate_19200;

    printf("mpRsInit() : ");
    rc = mpRsInit(MP_RS_COM1, &rs_config);
    printf("rc = %d\n", rc);

    printf("mpRsOpen() : ");
    rc = mpRsOpen(MP_RS_COM1);
    printf("rc = %d\n", rc);

    if( rc >= 0 )
    {
        portHandle = rc;

        while(1) 
        {
            memset( rcv_buff, 0, sizeof(rcv_buff));
            memset( snd_buff, 0, sizeof(snd_buff));

            rcv_len = mpRsRecv(portHandle, rcv_buff, MP_BUFF_SIZE);

            if( rcv_len > 0 )
            {
                snd_len = rcv_len;
                memcpy( snd_buff, rcv_buff, sizeof(snd_buff) );
                strupr( snd_buff );
                rcv_len = mpRsSend(portHandle, snd_buff, snd_len);
            }
        }
        mpRsClose(portHandle);
    }
}

char* strupr(char *string)
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

