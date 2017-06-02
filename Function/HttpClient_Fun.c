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
#include	"RTC_Driver.h"
#include	"DeviceDao.h"

#include	"MyMem.h"
#include	"CRC16.h"
#include	"JsonToObject.h"
#include	"HttpPostTool.h"

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
static MyState_TypeDef readTimeFromServer(void);
static void queryDeviceInfo(void);
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
		if(My_Pass == readTimeFromServer())
		{
			vTaskDelay(2000 / portTICK_RATE_MS);
			queryDeviceInfo();
		}
		
		vTaskDelay(60000 / portTICK_RATE_MS);
	}
}


static MyState_TypeDef readTimeFromServer(void)
{
	HttpClientBuffer * httpClientBuffer = NULL;
	MyState_TypeDef status = My_Fail;
	
	httpClientBuffer = MyMalloc(httpClientBufferStructSize);
	
	if(httpClientBuffer)
	{
		memcpy(&(httpClientBuffer->systemSetData), getGBSystemSetData(), SystemSetDataStructSize);
		
		//组合请求参数
		sprintf(httpClientBuffer->tempBuffer, "deviceId=%s\0", httpClientBuffer->systemSetData.deviceId);
		sprintf(httpClientBuffer->httpPostBuffer.sendBuf, "POST /NCDPOCT_Server/%s HTTP/1.1\nHost: %d.%d.%d.%d:%d\nConnection: keep-alive\nContent-Length: %d\nContent-Type:application/x-www-form-urlencoded;charset=GBK\nAccept-Language: zh-CN,zh;q=0.8\n\n%s\0",
			ReadTimeUrl, httpClientBuffer->systemSetData.serverSet.serverIP.ip_1, httpClientBuffer->systemSetData.serverSet.serverIP.ip_2,
			httpClientBuffer->systemSetData.serverSet.serverIP.ip_3, httpClientBuffer->systemSetData.serverSet.serverIP.ip_4,
			httpClientBuffer->systemSetData.serverSet.serverPort, strlen(httpClientBuffer->tempBuffer), httpClientBuffer->tempBuffer);
		httpClientBuffer->httpPostBuffer.sendBufferLen = strlen(httpClientBuffer->httpPostBuffer.sendBuf);
		
		IP4_ADDR(&(httpClientBuffer->httpPostBuffer.server_ip), httpClientBuffer->systemSetData.serverSet.serverIP.ip_1, httpClientBuffer->systemSetData.serverSet.serverIP.ip_2,
			httpClientBuffer->systemSetData.serverSet.serverIP.ip_3, httpClientBuffer->systemSetData.serverSet.serverIP.ip_4);
		httpClientBuffer->httpPostBuffer.server_port = httpClientBuffer->systemSetData.serverSet.serverPort;
		
		if(My_Pass == PostData(&(httpClientBuffer->httpPostBuffer)))
		{
			if(My_Pass == ParseJsonToDateTime(httpClientBuffer->httpPostBuffer.recvBuf, &(httpClientBuffer->dateTime)))
			{
				RTC_SetTimeData(&(httpClientBuffer->dateTime));
			}
			
			//发送成功表明网络状态正常
			status = My_Pass;
		}
	}
	
	MyFree(httpClientBuffer);
	
	return status;
}

static void queryDeviceInfo(void)
{
	HttpClientBuffer * httpClientBuffer = NULL;
	Device * tempDevice = NULL;
	
	httpClientBuffer = MyMalloc(httpClientBufferStructSize);
	
	if(httpClientBuffer)
	{
		memcpy(&(httpClientBuffer->systemSetData), getGBSystemSetData(), SystemSetDataStructSize);
		
		//组合请求参数
		sprintf(httpClientBuffer->tempBuffer, "deviceId=%s\0", httpClientBuffer->systemSetData.deviceId);
		sprintf(httpClientBuffer->httpPostBuffer.sendBuf, "POST /NCDPOCT_Server/%s HTTP/1.1\nHost: %d.%d.%d.%d:%d\nConnection: keep-alive\nContent-Length: %d\nContent-Type:application/x-www-form-urlencoded;charset=GBK\nAccept-Language: zh-CN,zh;q=0.8\n\n%s\0",
			QueryDeviceByDeviceIdUrl, httpClientBuffer->systemSetData.serverSet.serverIP.ip_1, httpClientBuffer->systemSetData.serverSet.serverIP.ip_2,
			httpClientBuffer->systemSetData.serverSet.serverIP.ip_3, httpClientBuffer->systemSetData.serverSet.serverIP.ip_4,
			httpClientBuffer->systemSetData.serverSet.serverPort, strlen(httpClientBuffer->tempBuffer), httpClientBuffer->tempBuffer);
		httpClientBuffer->httpPostBuffer.sendBufferLen = strlen(httpClientBuffer->httpPostBuffer.sendBuf);
		
		IP4_ADDR(&(httpClientBuffer->httpPostBuffer.server_ip), httpClientBuffer->systemSetData.serverSet.serverIP.ip_1, httpClientBuffer->systemSetData.serverSet.serverIP.ip_2,
			httpClientBuffer->systemSetData.serverSet.serverIP.ip_3, httpClientBuffer->systemSetData.serverSet.serverIP.ip_4);
		httpClientBuffer->httpPostBuffer.server_port = httpClientBuffer->systemSetData.serverSet.serverPort;
		
		if(My_Pass == PostData(&(httpClientBuffer->httpPostBuffer)))
		{
			tempDevice = MyMalloc(DeviceStructSize*2);
			if(tempDevice)
			{
				//读取当前设备信息
				if((My_Pass == ReadDeviceFromFile(&(tempDevice[0])))
					&& (tempDevice[0].crc == CalModbusCRC16Fun1(&(tempDevice[0]), DeviceStructCrcSize))
					&& (My_Pass == ParseJsonToDevice(httpClientBuffer->httpPostBuffer.recvBuf, &(tempDevice[1])))
					&& (tempDevice[0].modifyTimeStamp < tempDevice[1].modifyTimeStamp))
				{
					SaveDeviceToFile(&(tempDevice[1]));
				}
			}
			
			MyFree(tempDevice);
			
		}
	}
	
	MyFree(httpClientBuffer);
	
}
