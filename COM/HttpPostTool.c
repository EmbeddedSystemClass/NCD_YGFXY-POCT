/***************************************************************************************************
*FileName:
*Description:
*Author:
*Data:
***************************************************************************************************/

/***************************************************************************************************/
/******************************************头文件***************************************************/
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
/**************************************局部变量声明*************************************************/
/***************************************************************************************************/
static void PostDataByWireNet(HttpPostBuffer * httpPostBuffer);
static void PostDataByWifi(HttpPostBuffer * httpPostBuffer);
static MyState_TypeDef responeIsSuccessAndParse(HttpPostBuffer * httpPostBuffer);
/***************************************************************************************************/
/**************************************局部函数声明*************************************************/
/***************************************************************************************************/

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************正文********************************************************/
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
		
		//接收数据,最好等待1s
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
