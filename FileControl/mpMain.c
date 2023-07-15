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

	//Ends the initialization task.
	mpExitUsrRoot;
}


void mpTask1(void)
{
	char filename[100];
	
	char str[100];
	char ReadStr[100];
	
	int fd;
	DIR* pDir;

	char tempc[256];
	
	int ret;
	struct dirent* ret_dir;
	STATUS ret_sts;
	struct stat pStat;
	
	
	
	memset(filename, '\0', 100);
	memset(str, '\0', 100);
	strcpy(filename, MP_DRAM_DEV_DOS);
	strcat(filename, "\\TESTFILE1.dat");
	
	
	mpTaskDelay(20000);
	
	
	
	ret_sts = mpRemove(filename);
	if(ret_sts == 0)
	{
		printf("Initialize\n\r");
	}

	fd = mpCreate(filename, O_RDWR);

	if(fd >= 0)
	{
		strcpy(str, "File Write Test\n\r");
				
		ret = mpWrite(fd, str, (size_t)strlen(str));
		if(ret == -1)
		{
			printf("mpWrite Error \n\r");
		}
			
		ret = mpLseek(fd, 0, SEEK_SET);
		if(ret == -1)
		{
			printf("mpLseek Error \n\r");
		}
	
		memset(ReadStr, '\0', 100);

		ret = mpRead(fd, ReadStr, 100);
		if(ret == -1)
		{
			printf("mpRead Error \n\r");
		}
		else
		{
			printf("mpRead Success\n\r");
			printf(" %s\n\r", ReadStr);
		}
		
		ret = mpIoctl(fd, FIOGETNAME, (int)tempc);
		if(ret == -1 )
		{
			printf("mpIoctl Error \n\r");
		}
		else
		{
			printf("mpIoctl Success\n\r");
			printf(" File name = %s\n\r", tempc);
		}
		

		ret = mpLseek(fd, 5, SEEK_SET);
		if(ret == -1)
		{
			printf("mpLseek Error \n\r");
		}
		

		memset(ReadStr, '\0', 100);

		ret = mpRead(fd, ReadStr, 100);
		if(ret == -1)
		{
			printf("mpRead Error \n\r");
		}
		else
		{
			printf("mpRead Success\n\r");
			printf(" %s\n\r", ReadStr);
		}
		
		ret_sts = mpFstat(fd, &pStat);
		if(ret_sts == -1)
		{
			printf("mpFstat Error\n\r");
		}
		else
		{
			printf("mpFstat Complete\n\r");
			printf("st_dev = %u\n\r", pStat.st_dev);
			printf("st_ino = %u\n\r", pStat.st_ino);
			printf("st_mode = %u\n\r", pStat.st_mode);
			printf("st_nlink = %d\n\r", pStat.st_nlink);
			printf("st_uid = %d\n\r", pStat.st_uid);
			printf("st_gid = %d\n\r", pStat.st_gid);
			printf("st_rdev = %u\n\r", pStat.st_rdev);
			printf("st_size = %u\n\r", pStat.st_size);
			printf("st_atime = %u\n\r", pStat.st_atime);
			printf("st_mtime = %u\n\r", pStat.st_mtime);
			printf("st_ctime = %u\n\r", pStat.st_ctime);
			printf("st_blksize = %u\n\r", pStat.st_blksize);
			printf("st_blocks = %u\n\r", pStat.st_blocks);
			printf("st_attrib = %u\n\r\n\r", pStat.st_attrib);
		}

		
		ret_sts = mpClose(fd);
		if(ret_sts == -1)
		{
			printf("mpClose Error \n\r");
		}
	}
	else
	{
		printf("mpCreate Error!");
	}
	

	pDir = mpOpendir(MP_DRAM_DEV_DOS);

	if((int)pDir > 0)
	{
		ret_dir = mpReaddir(pDir);
		if(ret_dir->d_name != "")
		{
			printf("mpReaddir Success\n\r");
			printf(" %s\n\r", ret_dir->d_name);
		}
		else
		{
			printf("mpReaddir Failed\n\r");
		}

		ret_sts = mpClosedir(pDir);

		if(ret_sts == -1)
		{
			printf("mpClosedir Failed\n\r");
		}
	}
	else
	{
		printf("mpOpendir Failed\n\r");
	}
	
	printf("End\n\r");
}
