/***************************************************************************************************
*FileName:
*Description:
*Author: xsx_kair
*Data:
***************************************************************************************************/

/***************************************************************************************************/
/******************************************Header List********************************************/
/***************************************************************************************************/

#include	"LabServer_Fun.h"
#include	"Test_Fun.h"
#include	"Test_Task.h"
#include	"SystemSet_Dao.h"
#include	"Motor_Fun.h"

#include	"System_Data.h"
#include	"CRC16.h"
#include	"Define.h"
#include 	"tcpip.h"
#include	"MyMem.h"
#include	<string.h>
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
struct netconn *pxNetCon;
struct netbuf *pxRxBuffer;
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
static err_t ProcessCMD(unsigned char *buf, unsigned short len, struct netconn *pxNetCon);
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/****************************************File Start*************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************
*FunctionName: ProcessQuest
*Description: ����ͻ�������
*Input: parm -- �ͻ���
*Output: none
*Author: xsx
*Date: 2016��8��24��14:40:18
***************************************************************************************************/
void ProcessQuest(void * parm)
{
	unsigned char *pcRxString;
	unsigned short usLength;
	
	if(NULL == parm)
		return;
	
	pxNetCon = parm;
	/* We expect to immediately get data. */
	if(ERR_OK == netconn_recv( pxNetCon , &pxRxBuffer))
	{
		netbuf_data(pxRxBuffer, ( void * )&pcRxString, &usLength);

		netbuf_delete(pxRxBuffer);
		
		if(ERR_OK != ProcessCMD(pcRxString ,usLength, pxNetCon))
			;        
	}

}

static err_t ProcessCMD(unsigned char *buf, unsigned short len, struct netconn *pxNetCon)
{
	char *pxbuf1;
	char *pxbuf2;
	unsigned short temp = 0xffff;
	unsigned short i=0;
	err_t err;
	
	pxbuf1 = MyMalloc(4096);
	pxbuf2 = MyMalloc(10);
	if(pxbuf1 && pxbuf2)
	{
		memset(pxbuf1, 0, 4096);
		
		if(!strncmp( (char *)buf, "Read Device Info", 16 ))
		{
			sprintf(pxbuf1, "{\"deviceid\":\"%s\",\"devicestatus\":%d}", getGBSystemSetData()->deviceId, GetTestStatusFlorLab());
		}
		else if(!strncmp( (char *)buf, "Start Test", 10 ))
		{
			if(0 == GetTestStatusFlorLab())
			{
				StartTest(GetTestDataForLab());
				
				sprintf(pxbuf1, "OK");
			}
			else
				sprintf(pxbuf1, "Startted");
		}
		else if(!strncmp( (char *)buf, "Read Test Data", 14 ))
		{
			i = 0;
			
			sprintf(pxbuf1, "{\"data\":[");
			while(pdPASS == TakeTestPointData(&temp))
			{
				if(i == 0)
					sprintf(pxbuf2, "%d", temp);
				else
					sprintf(pxbuf2, ",%d", temp);
				strcat(pxbuf1, pxbuf2);
				i++;
			}
			sprintf(pxbuf2, "],\"status\":%d}", GetTestStatusFlorLab());
			strcat(pxbuf1, pxbuf2);
			
			if(0 == GetTestStatusFlorLab())
				MotorMoveTo(MaxLocation, 1);
		}

		err = netconn_write( pxNetCon, pxbuf1, strlen(pxbuf1), NETCONN_COPY );
	}
	
	MyFree(pxbuf1);
	MyFree(pxbuf2);
	
	return err;
}

/****************************************end of file************************************************/
