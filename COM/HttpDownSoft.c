/***************************************************************************************************
*FileName:
*Description:
*Author:
*Data:
***************************************************************************************************/

/***************************************************************************************************/
/******************************************ͷ�ļ�***************************************************/
/***************************************************************************************************/
#include	"HttpDownSoft.h"
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
static void GetDataByWireNet(HttpPostBuffer * httpPostBuffer);
static void GetDataByWifi(HttpPostBuffer * httpPostBuffer);

const char * fileStartStr = "i am zhangxiong^*^!";
/***************************************************************************************************/
/**************************************�ֲ���������*************************************************/
/***************************************************************************************************/

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************����********************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/


MyRes MyDownLoadSoftFile(HttpPostBuffer * httpPostBuffer)
{
	if(httpPostBuffer)
	{
		memset(httpPostBuffer->recvBuf, 0, SERVERRECVBUFLEN);
		httpPostBuffer->recvBufferLen = 0;
		GetDataByWireNet(httpPostBuffer);
		if(My_Pass == checkNewFirmwareIsSuccessDownload())
		{
			setIsSuccessDownloadFirmware(true);
			return My_Pass;
		}
		
		memset(httpPostBuffer->recvBuf, 0, SERVERRECVBUFLEN);
		httpPostBuffer->recvBufferLen = 0;
		GetDataByWifi(httpPostBuffer);
		
		if(My_Pass == checkNewFirmwareIsSuccessDownload())
		{
			setIsSuccessDownloadFirmware(true);
			return My_Pass;
		}
	}
	
	return My_Fail;
}

static void GetDataByWireNet(HttpPostBuffer * httpPostBuffer)
{
	err_t err;
	struct pbuf *p = NULL;
	unsigned short tempValue = 0;
	char * tempPoint = NULL;

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
		p = httpPostBuffer->recvNetBuf->p;
		
		while(p)
		{
			if(httpPostBuffer->recvBufferLen == 0)
			{
				//�����ļ�ͷ
				tempPoint = strstr(p->payload, fileStartStr);
				if(tempPoint)
				{
					tempValue = tempPoint - ((char *)p->payload) + strlen(fileStartStr);
					WriteAppFile((char *)(p->payload) + tempValue, p->len-tempValue, true);
					httpPostBuffer->recvBufferLen += (p->len-tempValue);
				}
			}
			else
			{
				WriteAppFile(p->payload, p->len, false);
				httpPostBuffer->recvBufferLen += (p->len);
			}
			p = p->next;
		}
		
		vTaskDelay(10 / portTICK_RATE_MS);
		
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

static void GetDataByWifi(HttpPostBuffer * httpPostBuffer)
{
	unsigned short readSize = 0;
	unsigned short tempValue = 0;
	char * tempPoint = NULL;
	
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
		
		//��������,��õȴ�1s,��ȡ���ȹ����п���ĩβ�����ݶ�ʧ
		httpPostBuffer->recvBufferLen = 0;
		while(pdPASS == ReceiveDataFromQueue(GetUsart4RXQueue(), NULL, httpPostBuffer->recvBuf, 1000, 
			&readSize, 1, 1000 / portTICK_RATE_MS, 100 / portTICK_RATE_MS))
		{
			//��������ļ�����Ϊ0��˵����û��ʼ�����ļ�������Ҫ�����ļ�ͷ
			if(httpPostBuffer->recvBufferLen == 0)
			{
				//�����ļ�ͷ
				tempPoint = strstr(httpPostBuffer->recvBuf, fileStartStr);
				if(tempPoint)
				{
					tempValue = tempPoint - httpPostBuffer->recvBuf + strlen(fileStartStr);
					readSize -= tempValue;
					WriteAppFile(httpPostBuffer->recvBuf + tempValue, readSize, true);
					httpPostBuffer->recvBufferLen += readSize;
				}
			}
			else
			{
				WriteAppFile(httpPostBuffer->recvBuf, readSize, false);
			
				httpPostBuffer->recvBufferLen += readSize;
			}
		}
		
		END:

			giveWifixMutex();
	}
}

