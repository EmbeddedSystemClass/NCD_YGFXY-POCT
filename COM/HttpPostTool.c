/***************************************************************************************************
*FileName:
*Description:
*Author:
*Data:
***************************************************************************************************/

/***************************************************************************************************/
/******************************************ͷ�ļ�***************************************************/
/***************************************************************************************************/
#include	"HttpPostTool.h"
#include	"QueueUnits.h"
#include	"System_Data.h"
#include 	"Usart4_Driver.h"
#include	"SystemSet_Data.h"
#include	"AppFileDao.h"
#include	"IAP_Fun.h"
#include	"RemoteSoft_Data.h"
#include	"WifiFunction.h"
#include	"MyMem.h"
#include	"MyTools.h"
#include 	"Utf8ToGBK.h"

#include	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"
#include	"semphr.h"

#include	<string.h>
#include	"stdio.h"
#include 	"stdlib.h"
/***************************************************************************************************/
/**************************************�ֲ���������*************************************************/
/***************************************************************************************************/
static void PostDataByWireNet(HttpPostBuffer * httpPostBuffer);
static void PostDataByWifi(HttpPostBuffer * httpPostBuffer);
static MyState_TypeDef responeIsSuccessAndParse(HttpPostBuffer * httpPostBuffer);
/***************************************************************************************************/
/**************************************�ֲ���������*************************************************/
/***************************************************************************************************/

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************����********************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/


MyState_TypeDef PostData(HttpPostBuffer * httpPostBuffer)
{
	if(httpPostBuffer)
	{
		memset(httpPostBuffer->recvBuf, 0, SERVERRECVBUFLEN);
		httpPostBuffer->recvBufferLen = 0;
		PostDataByWireNet(httpPostBuffer);
		if(My_Pass == responeIsSuccessAndParse(httpPostBuffer))
			return My_Pass;
		
		memset(httpPostBuffer->recvBuf, 0, SERVERRECVBUFLEN);
		httpPostBuffer->recvBufferLen = 0;
		PostDataByWifi(httpPostBuffer);
		return responeIsSuccessAndParse(httpPostBuffer);
	}
	
	return My_Fail;
}

static void PostDataByWireNet(HttpPostBuffer * httpPostBuffer)
{
	err_t err;

	//��������
	httpPostBuffer->clientconn = netconn_new(NETCONN_TCP);
	//����ʧ��
	if(httpPostBuffer->clientconn == NULL)
		return;

	//�󶨱���ip
	err = netconn_bind(httpPostBuffer->clientconn, IP_ADDR_ANY, 0);
	//����ʧ��
	if(err != ERR_OK)
		goto END2;

	//��������Զ�̷�����
	err = netconn_connect(httpPostBuffer->clientconn, &httpPostBuffer->server_ip, httpPostBuffer->server_port);
	//����ʧ��
	if(err != ERR_OK)
		goto END2;
		
	//���ý������ݳ�ʱʱ��100MS
	httpPostBuffer->clientconn->recv_timeout = 2000;
		
		//��������
	err = netconn_write(httpPostBuffer->clientconn, httpPostBuffer->sendBuf, httpPostBuffer->sendBufferLen, NETCONN_COPY );
	//����ʧ��
	if(err != ERR_OK)
		goto END1;
		
	//��������
	while(ERR_OK == netconn_recv(httpPostBuffer->clientconn, &httpPostBuffer->recvNetBuf))
	{
		netbuf_copy(httpPostBuffer->recvNetBuf, httpPostBuffer->recvBuf + httpPostBuffer->recvBufferLen , httpPostBuffer->recvNetBuf->p->tot_len);
		
		httpPostBuffer->recvBufferLen += httpPostBuffer->recvNetBuf->p->tot_len;

		netbuf_delete(httpPostBuffer->recvNetBuf);
	}
		
	END1:
		netconn_close(httpPostBuffer->clientconn);
		netconn_delete(httpPostBuffer->clientconn);
		return;
		
	END2:
		netconn_delete(httpPostBuffer->clientconn);
		return;
}

static void PostDataByWifi(HttpPostBuffer * httpPostBuffer)
{
	unsigned short readSize = 0;
	
	if(My_Pass == takeWifiMutex(1000 / portTICK_RATE_MS))
	{
		//��ն�������
		while(pdPASS == ReceiveDataFromQueue(GetUsart4RXQueue(), NULL, httpPostBuffer->recvBuf, 1000, 
				&readSize, 1, 10 / portTICK_RATE_MS, 1 / portTICK_RATE_MS));
		
		//��������
		readSize = httpPostBuffer->sendBufferLen;
		while(readSize > 800)
		{
			if(My_Pass == SendDataToQueue(GetUsart4TXQueue(), NULL, httpPostBuffer->sendBuf + (httpPostBuffer->sendBufferLen - readSize), 
				800, 1, 1000 / portTICK_RATE_MS, 10 / portTICK_RATE_MS, EnableUsart4TXInterrupt))
			{
				readSize -= 800;
				vTaskDelay(100 / portTICK_RATE_MS);
			}
			else
				goto END;
		}
		
		if(readSize > 0)
		{
			if(My_Pass == SendDataToQueue(GetUsart4TXQueue(), NULL, httpPostBuffer->sendBuf + (httpPostBuffer->sendBufferLen - readSize), 
				readSize, 1, 1000 / portTICK_RATE_MS, 10 / portTICK_RATE_MS, EnableUsart4TXInterrupt))
			{
				;
			}
			else
				goto END;
		}
		
		//��������,��õȴ�1s
		readSize = 0;
		while(pdPASS == ReceiveDataFromQueue(GetUsart4RXQueue(), NULL, httpPostBuffer->recvBuf+readSize, 1000, 
			&readSize, 1, 1000 / portTICK_RATE_MS, 1000 / portTICK_RATE_MS))
		{
			httpPostBuffer->recvBufferLen += readSize;
		}
		
		END:
			giveWifixMutex();
	}
}

static MyState_TypeDef responeIsSuccessAndParse(HttpPostBuffer * httpPostBuffer)
{
	char * tempReceiveBuf = NULL;
	char * tempPoint = NULL;
	MyState_TypeDef status = My_Fail;
	
	tempReceiveBuf = MyMalloc(httpPostBuffer->recvBufferLen+10);
	
	if(tempReceiveBuf)
	{
		memcpy(tempReceiveBuf, httpPostBuffer->recvBuf, httpPostBuffer->recvBufferLen+10);
		if(true == CheckStrIsSame(tempReceiveBuf, HttpResponeOK, strlen(HttpResponeOK)))
		{
			tempPoint = strtok(tempReceiveBuf, "\r\n");
			tempPoint = strtok(NULL, "\r\n");
			tempPoint = strtok(NULL, "\r\n");
			tempPoint = strtok(NULL, "\r\n");
			tempPoint = strtok(NULL, "\r\n");
			tempPoint = strtok(NULL, "\r\n");
			tempPoint = strtok(NULL, "\r\n");
			
			utf8ConvertToGBK(tempPoint, strlen(tempPoint), httpPostBuffer->recvBuf, &(httpPostBuffer->recvBufferLen));

			status = My_Pass;
		}
	}
	
	MyFree(tempReceiveBuf);
	return status;
}
