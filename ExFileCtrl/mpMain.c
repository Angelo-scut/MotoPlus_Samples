//mpMain.c
//
//This contains mpUsrRoot which is the entry point for your MotoPlus application

//ADDITIONAL INCLUDE FILES 
//(Note that motoPlus.h should be included in every source file)
#include "motoPlus.h"


//GLOBAL DATA DEFINITIONS
int nTaskID1;

//FUNCTION PROTOTYPES
void mpTask1(void);

//FUNCTION DEFINITIONS
void mpUsrRoot(int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10)
{	
	nTaskID1 = mpCreateTask(MP_PRI_TIME_NORMAL, MP_STACK_SIZE, (FUNCPTR)mpTask1,
						arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
	
	mpExitUsrRoot;
}

void mpTask1(void)
{
	int fd;
	int i;
	char R_FileData[3000];
	char W_FileData[3000];
	char filename[100];
	LONG FileCnt;
	LONG ret;
	size_t File_len = 0;
	
	memset(R_FileData, '\0', sizeof(char) * 3000);
	memset(W_FileData, '\0', sizeof(char) * 3000);


	mpTaskDelay(30000);

	
	printf("ExFile Test Start\n\r");
	
	ret = mpSaveFile(MP_DRV_ID_DRAM, "", "TESTJOB1.JBI");
	if(ret == -1)
		printf("mpSaveFile Error\n\r");

	memset(filename, '\0', sizeof(char) * 100);
	strcpy(filename, MP_DRAM_DEV_DOS);
	strcat(filename, "\\TESTJOB1.JBI");

	fd = mpOpen(filename, O_RDONLY, 0);
	if(fd == -1)
		printf("mpOpen Error\n\r");
		
	ret = mpRead(fd, R_FileData, 3000);
	if(ret == -1)
		printf("mpRead Error\n\r");
		
	ret = mpClose(fd);
	if(ret == -1)
		printf("mpClose Error\n\r");
		
	printf("\n\r");
		
	File_len = strlen(R_FileData);
	strcpy(W_FileData, R_FileData);
	
	printf("\n\r");
	
	printf("Read TESTJOB1 End\n\r\n\r");

	//Change Job Name
	for(i=0; i<(int)File_len; i++)
	{			
		if(strncmp(&R_FileData[i], "//NAME", 6) == 0)
		{
			printf("Change Job Name\n\r");
			strncpy(&W_FileData[i], "//NAME TESTJOB2", 15);
			break;
		}
	}
	
	printf("%s", W_FileData);
	
	memset(filename, '\0', sizeof(char) * 100);
	strcpy(filename, MP_DRAM_DEV_DOS);
	strcat(filename, "\\TESTJOB2.JBI");
			
	fd = mpCreate(filename, O_RDWR);
	if(fd < 0)
		printf("mpCreate Error\n\r");

	ret = mpWrite(fd, W_FileData, File_len);
	if(ret == -1)
		printf("mpWrite Error\n\r");
	
	ret = mpClose(fd);
	if(ret == -1)
		printf("mpClose Error\n\r");


	ret = mpLoadFile(MP_DRV_ID_DRAM, "", "TESTJOB2.JBI");		
	if(ret != 0)
		printf("mpLoadFile Error %d\n\r", ret);
	

	
	ret = mpRefreshFileList(MP_EXT_ID_JBI);
	if(ret == -1)
		printf("mpRefreshFileList Error\n\r");

	FileCnt = mpGetFileCount();
	if(ret == -1)
		printf("mpGetFileCount Error\n\r");
	else
		printf("mpGetFileCount FileCount = %d\n\r", FileCnt);


	for(i = 0; i < FileCnt; i++)
	{
		ret = mpGetFileName(i, filename);
		if(ret == -1)
			printf("mpGetFileName Error\n\r");
		else
			printf("mpGetFileName FileName = %s\n\r", filename);
	}

}
