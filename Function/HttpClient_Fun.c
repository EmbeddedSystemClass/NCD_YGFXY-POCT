/***************************************************************************************************
*FileName:
*Description:
*Author:
*Data:
***************************************************************************************************/

/***************************************************************************************************/
/******************************************头文件***************************************************/
/***************************************************************************************************/
#include	"HttpClient_Fun.h"
#include	"HttpPostTool.h"

#include	"MyMem.h"
#include	"CRC16.h"

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

/***************************************************************************************************/
/**************************************局部函数声明*************************************************/
/***************************************************************************************************/

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************正文********************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

void UpLoadFunction(void)
{
	while(1)
	{

	
		vTaskDelay(60000 / portTICK_RATE_MS);
	}
}

MyState_TypeDef readTimeFromServer(void)
{
	HttpClientBuffer * httpClientBuffer = NULL;
	MyState_TypeDef status = My_Fail;
	
	httpClientBuffer = MyMalloc(httpClientBufferStructSize);
	
	if(httpClientBuffer)
	{
		memcpy(&(httpClientBuffer->systemSetData), getGBSystemSetData(), SystemSetDataStructSize);
		
		//组合请求参数
		sprintf(httpClientBuffer->tempBuffer, "deviceId=%s\0", httpClientBuffer->systemSetData.device.deviceid);
		sprintf(httpClientBuffer->httpPostBuffer.sendBuf, "POST /NCDPOCT_Server/%s HTTP/1.1\nHost: %d.%d.%d.%d:%d\nConnection: keep-alive\nContent-Length: %d\nContent-Type:application/x-www-form-urlencoded;charset=GBK\nAccept-Language: zh-CN,zh;q=0.8\n\n%s\0",
			ReadTimeUrl, httpClientBuffer->systemSetData.serverSet.serverIP.ip_1, httpClientBuffer->systemSetData.serverSet.serverIP.ip_2,
			httpClientBuffer->systemSetData.serverSet.serverIP.ip_3, httpClientBuffer->systemSetData.serverSet.serverIP.ip_4,
			httpClientBuffer->systemSetData.serverSet.serverPort, strlen(httpClientBuffer->tempBuffer), httpClientBuffer->tempBuffer);
		httpClientBuffer->httpPostBuffer.sendBufferLen = strlen(httpClientBuffer->httpPostBuffer.sendBuf);
		
		IP4_ADDR(&(httpClientBuffer->httpPostBuffer.server_ip), httpClientBuffer->systemSetData.serverSet.serverIP.ip_1, httpClientBuffer->systemSetData.serverSet.serverIP.ip_2,
			httpClientBuffer->systemSetData.serverSet.serverIP.ip_3, httpClientBuffer->systemSetData.serverSet.serverIP.ip_4);
		httpClientBuffer->httpPostBuffer.server_port = httpClientBuffer->systemSetData.serverSet.serverPort;
		
		memset(httpClientBuffer->httpPostBuffer.recvBuf, 0, 100);
	}
	
	MyFree(httpClientBuffer);
	
	return status;
}

void pullDeviceInfo(void)
{
	
}
