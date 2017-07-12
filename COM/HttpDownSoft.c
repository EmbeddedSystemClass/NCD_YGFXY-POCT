/***************************************************************************************************
*FileName:
*Description:
*Author:
*Data:
***************************************************************************************************/

/***************************************************************************************************/
/******************************************头文件***************************************************/
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
/**************************************局部变量声明*************************************************/
/***************************************************************************************************/
static void GetDataByWireNet(HttpPostBuffer * httpPostBuffer);
static void GetDataByWifi(HttpPostBuffer * httpPostBuffer);

const char * fileStartStr = "i am zhangxiong^*^!";
/***************************************************************************************************/
/**************************************局部函数声明*************************************************/
/***************************************************************************************************/

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************正文********************************************************/
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

	//创建连接
	httpPostBuffer->clientconn = netconn_new(NETCONN_TCP);
	//创建失败
	if(httpPostBuffer->clientconn == NULL)
		return;

	//绑定本地ip
	err = netconn_bind(httpPostBuffer->clientconn, IP_ADDR_ANY, 0);
	//连接失败
	if(err != ERR_OK)
		goto END2;

	//尝试连接远程服务器
	err = netconn_connect(httpPostBuffer->clientconn, &httpPostBuffer->server_ip, httpPostBuffer->server_port);
	//连接失败
	if(err != ERR_OK)
		goto END2;
		
	//设置接收数据超时时间100MS
	httpPostBuffer->clientconn->recv_timeout = 2000;
		
		//发送数据
	err = netconn_write(httpPostBuffer->clientconn, httpPostBuffer->sendBuf, httpPostBuffer->sendBufferLen, NETCONN_COPY );
	//发送失败
	if(err != ERR_OK)
		goto END1;
	
	//接收数据
	while(ERR_OK == netconn_recv(httpPostBuffer->clientconn, &httpPostBuffer->recvNetBuf))
	{
		p = httpPostBuffer->recvNetBuf->p;
		
		while(p)
		{
			if(httpPostBuffer->recvBufferLen == 0)
			{
				//查找文件头
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
		
		//清空队列数据
		while(pdPASS == ReceiveDataFromQueue(GetUsart4RXQueue(), NULL, httpPostBuffer->recvBuf, 1000, 
				&readSize, 1, 10 / portTICK_RATE_MS, 1 / portTICK_RATE_MS));
		
		//发送数据
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
		
		//接收数据,最好等待1s,读取长度过大有可能末尾的数据丢失
		httpPostBuffer->recvBufferLen = 0;
		while(pdPASS == ReceiveDataFromQueue(GetUsart4RXQueue(), NULL, httpPostBuffer->recvBuf, 1000, 
			&readSize, 1, 1000 / portTICK_RATE_MS, 100 / portTICK_RATE_MS))
		{
			//如果接收文件长度为0，说明还没开始接收文件，则需要查找文件头
			if(httpPostBuffer->recvBufferLen == 0)
			{
				//查找文件头
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

