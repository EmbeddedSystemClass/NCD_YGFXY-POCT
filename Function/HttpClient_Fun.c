/******************************************File Start**********************************************
*FileName: 
*Description: 
*Author:xsx
*Data: 
****************************************************************************************************/

/***************************************************************************************************/
/******************************************Header List**********************************************/
/***************************************************************************************************/
#include	"HttpClient_Fun.h"
#include	"RTC_Driver.h"
#include	"DeviceDao.h"
#include	"DeviceError.h"
#include	"DeviceErrorDao.h"
#include	"DeviceAdjust.h"
#include	"DeviceAdjustDao.h"
#include	"DeviceMaintenance.h"
#include	"DeviceMaintenanceDao.h"
#include	"DeviceQuality.h"
#include	"DeviceQualityDao.h"
#include	"RemoteSoftDao.h"
#include	"HttpDownSoft.h"

#include	"MyMem.h"
#include	"CRC16.h"
#include	"JsonToObject.h"
#include	"HttpPostTool.h"
#include	"MyLock.h"

#include	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"
#include	"semphr.h"

#include	<string.h>
#include	"stdio.h"
#include 	"stdlib.h"

/***************************************************************************************************/
/******************************************Static Variables*****************************************/
/***************************************************************************************************/

/***************************************************************************************************/
/******************************************Static Methods*******************************************/
/***************************************************************************************************/
static MyRes readTimeFromServer(void);
static void queryDeviceInfo(void);
static void upLoadDeviceAdjustRecord(void);
static void upLoadYGFXYDataRecord(void);
static void upLoadDeviceErrorRecord(void);
static void upLoadDeviceMaintenanceRecord(void);
static void upLoadDeviceQualityRecord(void);
static void queryRemoteFirmwareVersion(void);
static void DownLoadFirmware(void);
/***************************************************************************************************/
/***************************************************************************************************/
/******************************************Main Body************************************************/
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
			
			vTaskDelay(2000 / portTICK_RATE_MS);
			upLoadDeviceAdjustRecord();
			
			vTaskDelay(2000 / portTICK_RATE_MS);
			upLoadYGFXYDataRecord();
			
			vTaskDelay(2000 / portTICK_RATE_MS);
			upLoadDeviceErrorRecord();
			
			vTaskDelay(2000 / portTICK_RATE_MS);
			upLoadDeviceMaintenanceRecord();
			
			vTaskDelay(2000 / portTICK_RATE_MS);
			upLoadDeviceQualityRecord();	
		}
		
		vTaskDelay(2000 / portTICK_RATE_MS);
		queryRemoteFirmwareVersion();
		
		vTaskDelay(2000 / portTICK_RATE_MS);
		DownLoadFirmware();
		
		vTaskDelay(60000 / portTICK_RATE_MS);
	}
}


static MyRes readTimeFromServer(void)
{
	HttpClientBuffer * httpClientBuffer = NULL;
	MyRes status = My_Fail;
	
	httpClientBuffer = MyMalloc(httpClientBufferStructSize);
	
	if(httpClientBuffer)
	{
		memcpy(&(httpClientBuffer->systemSetData), getGBSystemSetData(), SystemSetDataStructSize);
		
		//组合请求参数
		snprintf(httpClientBuffer->tempBuffer, 100, "deviceId=%s", httpClientBuffer->systemSetData.deviceId);
		snprintf(httpClientBuffer->httpPostBuffer.sendBuf, 1024, "POST /NCDPOCT_Server/%s HTTP/1.1\nHost: %d.%d.%d.%d:%d\nConnection: keep-alive\nContent-Length: %d\nContent-Type:application/x-www-form-urlencoded;charset=GBK\nAccept-Language: zh-CN,zh;q=0.8\n\n%s",
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

/***************************************************************************************************
*FunctionName:  queryDeviceInfo
*Description:  读取服务器中的设备信息，如果版本比本地版本高，则更新本地数据
*Input:  
*Output:  
*Return:  
*Author:  xsx
*Date: 2017年6月5日 10:23:44
***************************************************************************************************/
static void queryDeviceInfo(void)
{
	HttpClientBuffer * httpClientBuffer = NULL;
	Device * tempDevice = NULL;
	
	MyLock * myLock = getMyDeviceLock();
	
	if(My_Fail == LockObject(myLock, &httpClientBuffer, 5))
		return;
	
	httpClientBuffer = MyMalloc(httpClientBufferStructSize);
	
	if(httpClientBuffer)
	{
		memcpy(&(httpClientBuffer->systemSetData), getGBSystemSetData(), SystemSetDataStructSize);
		
		//组合请求参数
		snprintf(httpClientBuffer->tempBuffer, 50, "deviceId=%s", httpClientBuffer->systemSetData.deviceId);
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
					&& (tempDevice[0].crc == CalModbusCRC16Fun(&(tempDevice[0]), DeviceStructCrcSize, NULL))
					&& (My_Pass == ParseJsonToDevice(httpClientBuffer->httpPostBuffer.recvBuf, &(tempDevice[1])))
					&& (tempDevice[0].modifyTimeStamp < tempDevice[1].modifyTimeStamp))
				{
					SaveDeviceToFile(&(tempDevice[1]));
				}
			}
			
			MyFree(tempDevice);
			
		}
	}
	
	UnLockObject(myLock, &httpClientBuffer);
	MyFree(httpClientBuffer);
}

/***************************************************************************************************
*FunctionName:  upLoadDeviceAdjustRecord
*Description:  上传设备的校准记录
*Input:  
*Output:  
*Return:  
*Author:  xsx
*Date: 2017年6月5日 10:24:37
***************************************************************************************************/
static void upLoadDeviceAdjustRecord(void)
{
	HttpClientBuffer * httpClientBuffer = NULL;
	DeviceAdjustReadPackge * deviceAdjustReadPackge = NULL;
	
	httpClientBuffer = MyMalloc(httpClientBufferStructSize);
	deviceAdjustReadPackge = MyMalloc(DeviceAdjustReadPackgeStructSize);
	
	if(httpClientBuffer && deviceAdjustReadPackge)
	{
		memcpy(&(httpClientBuffer->systemSetData), getGBSystemSetData(), SystemSetDataStructSize);
		
		//读取校准记录以及头信息
		memset(&(deviceAdjustReadPackge->pageRequest), 0, PageRequestStructSize);					//清除请求pagerequest表明按照上传索引来读取
		if(My_Pass == readDeviceAdjustFromFile(deviceAdjustReadPackge))
		{
			if(deviceAdjustReadPackge->deviceAdjust[0].crc == CalModbusCRC16Fun(&(deviceAdjustReadPackge->deviceAdjust[0]), DeviceAdjustStructCrcSize, NULL))
			{
				//组合请求参数
				snprintf(httpClientBuffer->tempBuffer, 500, "normalv=%.2f&measurev=%.2f&testtime=20%02d-%02d-%02d %02d:%02d:%02d&device.did=%s&result=Success&dsc=Device Automatic Adjust", 
					deviceAdjustReadPackge->deviceAdjust[0].normalv, deviceAdjustReadPackge->deviceAdjust[0].measurev, deviceAdjustReadPackge->deviceAdjust[0].dateTime.year,
					deviceAdjustReadPackge->deviceAdjust[0].dateTime.month, deviceAdjustReadPackge->deviceAdjust[0].dateTime.day, deviceAdjustReadPackge->deviceAdjust[0].dateTime.hour,
					deviceAdjustReadPackge->deviceAdjust[0].dateTime.min, deviceAdjustReadPackge->deviceAdjust[0].dateTime.sec,
					httpClientBuffer->systemSetData.deviceId);
				sprintf(httpClientBuffer->httpPostBuffer.sendBuf, "POST /NCDPOCT_Server/%s HTTP/1.1\nHost: %d.%d.%d.%d:%d\nConnection: keep-alive\nContent-Length: %d\nContent-Type:application/x-www-form-urlencoded;charset=GBK\nAccept-Language: zh-CN,zh;q=0.8\n\n%s\0",
					UpLoadDeviceAdjustUrl, httpClientBuffer->systemSetData.serverSet.serverIP.ip_1, httpClientBuffer->systemSetData.serverSet.serverIP.ip_2,
					httpClientBuffer->systemSetData.serverSet.serverIP.ip_3, httpClientBuffer->systemSetData.serverSet.serverIP.ip_4,
					httpClientBuffer->systemSetData.serverSet.serverPort, strlen(httpClientBuffer->tempBuffer), httpClientBuffer->tempBuffer);
				httpClientBuffer->httpPostBuffer.sendBufferLen = strlen(httpClientBuffer->httpPostBuffer.sendBuf);
				
				IP4_ADDR(&(httpClientBuffer->httpPostBuffer.server_ip), httpClientBuffer->systemSetData.serverSet.serverIP.ip_1, httpClientBuffer->systemSetData.serverSet.serverIP.ip_2,
					httpClientBuffer->systemSetData.serverSet.serverIP.ip_3, httpClientBuffer->systemSetData.serverSet.serverIP.ip_4);
				httpClientBuffer->httpPostBuffer.server_port = httpClientBuffer->systemSetData.serverSet.serverPort;
				
				if(My_Pass == PostData(&(httpClientBuffer->httpPostBuffer)))
				{
					plusDeviceAdjustHeaderuserUpLoadIndexToFile(1);
				}
			}
		}
	}
	
	MyFree(httpClientBuffer);
	MyFree(deviceAdjustReadPackge);
}

/***************************************************************************************************
*FunctionName:  upLoadYGFXYDataRecord
*Description:  上传测试数据
*Input:  
*Output:  
*Return:  
*Author:  xsx
*Date: 2017年6月6日 08:52:16
***************************************************************************************************/
static void upLoadYGFXYDataRecord(void)
{
	HttpClientBuffer * httpClientBuffer = NULL;
	unsigned short i=0;
	DeviceRecordHeader * deviceRecordHeader = NULL;
	TestData * userTestData = NULL;
	TestData * ncdTestData = NULL;
	
	httpClientBuffer = MyMalloc(httpClientBufferStructSize);
	deviceRecordHeader = MyMalloc(DeviceRecordHeaderStructSize);
	userTestData = MyMalloc(TestDataStructSize);
	ncdTestData = MyMalloc(TestDataStructSize);
	
	if(httpClientBuffer && deviceRecordHeader && userTestData && ncdTestData)
	{
		memcpy(&(httpClientBuffer->systemSetData), getGBSystemSetData(), SystemSetDataStructSize);
		
		//读取校准记录以及头信息
		memset(deviceRecordHeader, 0, DeviceRecordHeaderStructSize);
		memset(userTestData, 0, TestDataStructSize);
		memset(ncdTestData, 0, TestDataStructSize);
		if(My_Pass == readTestDataFromFile(NULL, deviceRecordHeader, userTestData, ncdTestData))
		{
			//发送数据到用户服务器
			if(userTestData->crc == CalModbusCRC16Fun(userTestData, TestDataStructCrcSize, NULL))
			{
				//组合请求参数
				sprintf(httpClientBuffer->tempBuffer, "device.did=%s&item.code=%s&operator.name=%s&cardlot=%s&cardnum=%s&sampleid=%s&testtime=20%02d-%02d-%02d %02d:%02d:%02d&ambienttemp=%.2f&cardtemp=%.2f&overtime=%d&cline=%d&bline=%d&tline=%d&testv=%.*f&serialnum=%s-%s&\0", 
					httpClientBuffer->systemSetData.deviceId, userTestData->qrCode.itemConstData.itemCode, userTestData->operator.name, 
					userTestData->qrCode.PiHao,	userTestData->qrCode.piNum, userTestData->sampleid,	userTestData->testDateTime.year, 
					userTestData->testDateTime.month, userTestData->testDateTime.day, userTestData->testDateTime.hour, userTestData->testDateTime.min, 
					userTestData->testDateTime.sec,	userTestData->temperature.E_Temperature, userTestData->temperature.O_Temperature,
					userTestData->time, userTestData->testSeries.C_Point[1], userTestData->testSeries.B_Point[1], userTestData->testSeries.T_Point[1],
					userTestData->qrCode.itemConstData.pointNum, userTestData->testSeries.AdjustResult,	userTestData->qrCode.PiHao, userTestData->qrCode.piNum);
				
				if(userTestData->testResultDesc == ResultIsOK)
					strcat(httpClientBuffer->tempBuffer, "t_isok=true&\0");
				else
					strcat(httpClientBuffer->tempBuffer, "t_isok=false&\0");	
				
				strcat(httpClientBuffer->tempBuffer, "series=[\0");
				for(i=0; i<100; i++)
				{
					if(i == 0)
						sprintf(httpClientBuffer->tempbuf2, "%d,%d,%d\0", userTestData->testSeries.TestPoint[i*3],
							userTestData->testSeries.TestPoint[i*3+1], userTestData->testSeries.TestPoint[i*3+2]);
					else
						sprintf(httpClientBuffer->tempbuf2, ",%d,%d,%d\0", userTestData->testSeries.TestPoint[i*3],
							userTestData->testSeries.TestPoint[i*3+1], userTestData->testSeries.TestPoint[i*3+2]);
					strcat(httpClientBuffer->tempBuffer, httpClientBuffer->tempbuf2);
				}
					
				strcat(httpClientBuffer->tempBuffer, "]\0");
				
				sprintf(httpClientBuffer->httpPostBuffer.sendBuf, "POST /NCDPOCT_Server/%s HTTP/1.1\nHost: %d.%d.%d.%d:%d\nConnection: keep-alive\nContent-Length: %d\nContent-Type:application/x-www-form-urlencoded;charset=GBK\nAccept-Language: zh-CN,zh;q=0.8\n\n%s\0",
					UpLoadYGFXYDataUrl, httpClientBuffer->systemSetData.serverSet.serverIP.ip_1, httpClientBuffer->systemSetData.serverSet.serverIP.ip_2,
					httpClientBuffer->systemSetData.serverSet.serverIP.ip_3, httpClientBuffer->systemSetData.serverSet.serverIP.ip_4,
					httpClientBuffer->systemSetData.serverSet.serverPort, strlen(httpClientBuffer->tempBuffer), httpClientBuffer->tempBuffer);
				httpClientBuffer->httpPostBuffer.sendBufferLen = strlen(httpClientBuffer->httpPostBuffer.sendBuf);
				
				IP4_ADDR(&(httpClientBuffer->httpPostBuffer.server_ip), httpClientBuffer->systemSetData.serverSet.serverIP.ip_1, httpClientBuffer->systemSetData.serverSet.serverIP.ip_2,
					httpClientBuffer->systemSetData.serverSet.serverIP.ip_3, httpClientBuffer->systemSetData.serverSet.serverIP.ip_4);
				httpClientBuffer->httpPostBuffer.server_port = httpClientBuffer->systemSetData.serverSet.serverPort;
				
				if(My_Pass == PostData(&(httpClientBuffer->httpPostBuffer)))
				{
					plusTestDataHeaderuserUpLoadIndexToFile(1, 0);
				}
			}
			
			//发送数据到纽康度服务器
			if(ncdTestData->crc == CalModbusCRC16Fun(ncdTestData, TestDataStructCrcSize, NULL))
			{
				//组合请求参数
				sprintf(httpClientBuffer->tempBuffer, "device.did=%s&card.cid=%s&cardnum=%s&sampleid=%s&testtime=20%02d-%02d-%02d %02d:%02d:%02d&ambienttemp=%.2f&cardtemp=%.2f&overtime=%d&cline=%d&bline=%d&tline=%d&testv=%.*f&serialnum=%s-%s&\0", 
					httpClientBuffer->systemSetData.deviceId, ncdTestData->qrCode.PiHao, ncdTestData->qrCode.piNum, ncdTestData->sampleid, ncdTestData->testDateTime.year, 
					ncdTestData->testDateTime.month, ncdTestData->testDateTime.day, ncdTestData->testDateTime.hour, ncdTestData->testDateTime.min, 
					ncdTestData->testDateTime.sec,	ncdTestData->temperature.E_Temperature, ncdTestData->temperature.O_Temperature,
					ncdTestData->time, ncdTestData->testSeries.C_Point[1], ncdTestData->testSeries.B_Point[1], ncdTestData->testSeries.T_Point[1],
					ncdTestData->qrCode.itemConstData.pointNum, ncdTestData->testSeries.AdjustResult, ncdTestData->qrCode.PiHao, ncdTestData->qrCode.piNum);
				
				if(ncdTestData->testResultDesc == ResultIsOK)
					strcat(httpClientBuffer->tempBuffer, "t_isok=true&\0");
				else
					strcat(httpClientBuffer->tempBuffer, "t_isok=false&\0");	
				
				strcat(httpClientBuffer->tempBuffer, "series=[\0");
				for(i=0; i<100; i++)
				{
					if(i == 0)
						sprintf(httpClientBuffer->tempbuf2, "%d,%d,%d\0", ncdTestData->testSeries.TestPoint[i*3],
							ncdTestData->testSeries.TestPoint[i*3+1], ncdTestData->testSeries.TestPoint[i*3+2]);
					else
						sprintf(httpClientBuffer->tempbuf2, ",%d,%d,%d\0", ncdTestData->testSeries.TestPoint[i*3],
							ncdTestData->testSeries.TestPoint[i*3+1], ncdTestData->testSeries.TestPoint[i*3+2]);
					strcat(httpClientBuffer->tempBuffer, httpClientBuffer->tempbuf2);
				}
					
				strcat(httpClientBuffer->tempBuffer, "]\0");
				
				sprintf(httpClientBuffer->httpPostBuffer.sendBuf, "POST /NCD_Server/%s HTTP/1.1\nHost: %d.%d.%d.%d:%d\nConnection: keep-alive\nContent-Length: %d\nContent-Type:application/x-www-form-urlencoded;charset=GBK\nAccept-Language: zh-CN,zh;q=0.8\n\n%s\0",
					NcdUpLoadYGFXYDataUrl, NCD_ServerIp_1, NCD_ServerIp_2, NCD_ServerIp_3, NCD_ServerIp_4, NCD_ServerPort, strlen(httpClientBuffer->tempBuffer), httpClientBuffer->tempBuffer);
				httpClientBuffer->httpPostBuffer.sendBufferLen = strlen(httpClientBuffer->httpPostBuffer.sendBuf);
				
				IP4_ADDR(&(httpClientBuffer->httpPostBuffer.server_ip), NCD_ServerIp_1, NCD_ServerIp_2, NCD_ServerIp_3, NCD_ServerIp_4);
				httpClientBuffer->httpPostBuffer.server_port = NCD_ServerPort;
				
				if(My_Pass == PostData(&(httpClientBuffer->httpPostBuffer)))
				{
					plusTestDataHeaderuserUpLoadIndexToFile(0, 1);
				}
			}
		}
	}
	
	MyFree(httpClientBuffer);
	MyFree(deviceRecordHeader);
	MyFree(userTestData);
	MyFree(ncdTestData);
}

/***************************************************************************************************
*FunctionName:  upLoadDeviceErrorRecord
*Description:  上传设备异常记录
*Input:  
*Output:  
*Return:  
*Author:  xsx
*Date: 2017年6月8日 16:29:49
***************************************************************************************************/
static void upLoadDeviceErrorRecord(void)
{
	HttpClientBuffer * httpClientBuffer = NULL;
	DeviceErrorReadPackge * deviceErrorReadPackge = NULL;
	
	httpClientBuffer = MyMalloc(httpClientBufferStructSize);
	deviceErrorReadPackge = MyMalloc(DeviceErrorReadPackgeStructSize);
	
	if(httpClientBuffer && deviceErrorReadPackge)
	{
		memcpy(&(httpClientBuffer->systemSetData), getGBSystemSetData(), SystemSetDataStructSize);
		
		//读取校准记录以及头信息
		memset(&(deviceErrorReadPackge->pageRequest), 0, PageRequestStructSize);					//清除请求pagerequest表明按照上传索引来读取
		if(My_Pass == readDeviceErrorFromFile(deviceErrorReadPackge))
		{
			if(deviceErrorReadPackge->deviceError[0].crc == CalModbusCRC16Fun(&(deviceErrorReadPackge->deviceError[0]), DeviceErrorStructCrcSize, NULL))
			{
				//组合请求参数
				snprintf(httpClientBuffer->tempBuffer, 1024, "errorcode=%04d&testtime=20%02d-%02d-%02d %02d:%02d:%02d&device.did=%s&dsc=%s", 
					deviceErrorReadPackge->deviceError[0].errorCode, deviceErrorReadPackge->deviceError[0].dateTime.year,
					deviceErrorReadPackge->deviceError[0].dateTime.month, deviceErrorReadPackge->deviceError[0].dateTime.day, deviceErrorReadPackge->deviceError[0].dateTime.hour,
					deviceErrorReadPackge->deviceError[0].dateTime.min, deviceErrorReadPackge->deviceError[0].dateTime.sec,
					httpClientBuffer->systemSetData.deviceId, deviceErrorReadPackge->deviceError[0].result);
				sprintf(httpClientBuffer->httpPostBuffer.sendBuf, "POST /NCDPOCT_Server/%s HTTP/1.1\nHost: %d.%d.%d.%d:%d\nConnection: keep-alive\nContent-Length: %d\nContent-Type:application/x-www-form-urlencoded;charset=GBK\nAccept-Language: zh-CN,zh;q=0.8\n\n%s\0",
					UpLoadDeviceErrorUrl, httpClientBuffer->systemSetData.serverSet.serverIP.ip_1, httpClientBuffer->systemSetData.serverSet.serverIP.ip_2,
					httpClientBuffer->systemSetData.serverSet.serverIP.ip_3, httpClientBuffer->systemSetData.serverSet.serverIP.ip_4,
					httpClientBuffer->systemSetData.serverSet.serverPort, strlen(httpClientBuffer->tempBuffer), httpClientBuffer->tempBuffer);
				httpClientBuffer->httpPostBuffer.sendBufferLen = strlen(httpClientBuffer->httpPostBuffer.sendBuf);
				
				IP4_ADDR(&(httpClientBuffer->httpPostBuffer.server_ip), httpClientBuffer->systemSetData.serverSet.serverIP.ip_1, httpClientBuffer->systemSetData.serverSet.serverIP.ip_2,
					httpClientBuffer->systemSetData.serverSet.serverIP.ip_3, httpClientBuffer->systemSetData.serverSet.serverIP.ip_4);
				httpClientBuffer->httpPostBuffer.server_port = httpClientBuffer->systemSetData.serverSet.serverPort;
				
				if(My_Pass == PostData(&(httpClientBuffer->httpPostBuffer)))
				{
					plusDeviceErrorHeaderuserUpLoadIndexToFile(1);
				}
			}
		}
	}
	
	MyFree(httpClientBuffer);
	MyFree(deviceErrorReadPackge);
}

/***************************************************************************************************
*FunctionName:  upLoadDeviceMaintenanceRecord
*Description:  上传设备维护记录
*Input:  
*Output:  
*Return:  
*Author:  xsx
*Date: 2017年6月16日 11:09:29
***************************************************************************************************/
static void upLoadDeviceMaintenanceRecord(void)
{
	HttpClientBuffer * httpClientBuffer = NULL;
	DeviceMaintenanceReadPackge * deviceReadPackge = NULL;
	
	httpClientBuffer = MyMalloc(httpClientBufferStructSize);
	deviceReadPackge = MyMalloc(DeviceMaintenanceReadPackgeStructSize);
	
	if(httpClientBuffer && deviceReadPackge)
	{
		memcpy(&(httpClientBuffer->systemSetData), getGBSystemSetData(), SystemSetDataStructSize);
		
		//读取校准记录以及头信息
		memset(&(deviceReadPackge->pageRequest), 0, PageRequestStructSize);					//清除请求pagerequest表明按照上传索引来读取
		if(My_Pass == readDeviceMaintenanceFromFile(deviceReadPackge))
		{
			if(deviceReadPackge->deviceMaintenance[0].crc == CalModbusCRC16Fun(&(deviceReadPackge->deviceMaintenance[0]), DeviceMaintenanceStructCrcSize, NULL))
			{
				//组合请求参数
				snprintf(httpClientBuffer->tempBuffer, 1024, "testtime=20%02d-%02d-%02d %02d:%02d:%02d&device.did=%s&operator.name=%s&dsc=%s&", 
					deviceReadPackge->deviceMaintenance[0].dateTime.year, deviceReadPackge->deviceMaintenance[0].dateTime.month, 
					deviceReadPackge->deviceMaintenance[0].dateTime.day, deviceReadPackge->deviceMaintenance[0].dateTime.hour,
					deviceReadPackge->deviceMaintenance[0].dateTime.min, deviceReadPackge->deviceMaintenance[0].dateTime.sec,
					httpClientBuffer->systemSetData.deviceId, deviceReadPackge->deviceMaintenance[0].operator.name,
					deviceReadPackge->deviceMaintenance[0].desc);
				
				if(deviceReadPackge->deviceMaintenance[0].isOk)
					strcat(httpClientBuffer->tempBuffer, "result=true\0");
				else
					strcat(httpClientBuffer->tempBuffer, "result=false\0");
				
				sprintf(httpClientBuffer->httpPostBuffer.sendBuf, "POST /NCDPOCT_Server/%s HTTP/1.1\nHost: %d.%d.%d.%d:%d\nConnection: keep-alive\nContent-Length: %d\nContent-Type:application/x-www-form-urlencoded;charset=GBK\nAccept-Language: zh-CN,zh;q=0.8\n\n%s\0",
					UpLoadDeviceMaintenanceUrl, httpClientBuffer->systemSetData.serverSet.serverIP.ip_1, httpClientBuffer->systemSetData.serverSet.serverIP.ip_2,
					httpClientBuffer->systemSetData.serverSet.serverIP.ip_3, httpClientBuffer->systemSetData.serverSet.serverIP.ip_4,
					httpClientBuffer->systemSetData.serverSet.serverPort, strlen(httpClientBuffer->tempBuffer), httpClientBuffer->tempBuffer);
				httpClientBuffer->httpPostBuffer.sendBufferLen = strlen(httpClientBuffer->httpPostBuffer.sendBuf);
				
				IP4_ADDR(&(httpClientBuffer->httpPostBuffer.server_ip), httpClientBuffer->systemSetData.serverSet.serverIP.ip_1, httpClientBuffer->systemSetData.serverSet.serverIP.ip_2,
					httpClientBuffer->systemSetData.serverSet.serverIP.ip_3, httpClientBuffer->systemSetData.serverSet.serverIP.ip_4);
				httpClientBuffer->httpPostBuffer.server_port = httpClientBuffer->systemSetData.serverSet.serverPort;
				
				if(My_Pass == PostData(&(httpClientBuffer->httpPostBuffer)))
				{
					plusDeviceMaintenanceHeaderuserUpLoadIndexToFile(1);
				}
			}
		}
	}
	
	MyFree(httpClientBuffer);
	MyFree(deviceReadPackge);
}

/***************************************************************************************************
*FunctionName:  upLoadDeviceQualityRecord
*Description:  上传质控记录
*Input:  
*Output:  
*Return:  
*Author:  xsx
*Date: 2017年6月16日 13:58:22
***************************************************************************************************/
static void upLoadDeviceQualityRecord(void)
{
	HttpClientBuffer * httpClientBuffer = NULL;
	DeviceQualityReadPackge * deviceReadPackge = NULL;
	unsigned char i=0;
	
	httpClientBuffer = MyMalloc(httpClientBufferStructSize);
	deviceReadPackge = MyMalloc(DeviceQualityReadPackgeStructSize);
	
	if(httpClientBuffer && deviceReadPackge)
	{
		memcpy(&(httpClientBuffer->systemSetData), getGBSystemSetData(), SystemSetDataStructSize);
		
		//读取校准记录以及头信息
		memset(&(deviceReadPackge->pageRequest), 0, PageRequestStructSize);					//清除请求pagerequest表明按照上传索引来读取
		if(My_Pass == readDeviceQualityFromFile(deviceReadPackge))
		{
			if(deviceReadPackge->deviceQuality[0].crc == CalModbusCRC16Fun(&(deviceReadPackge->deviceQuality[0]), DeviceQualityStructCrcSize, NULL))
			{
				//组合请求参数
				snprintf(httpClientBuffer->tempBuffer, 1024, "normalv=%.2f&testtime=20%02d-%02d-%02d %02d:%02d:%02d&device.did=%s&operator.name=%s&item.code=%s&dsc=%s&", 
					deviceReadPackge->deviceQuality[0].standardValue, deviceReadPackge->deviceQuality[0].dateTime.year, deviceReadPackge->deviceQuality[0].dateTime.month, 
					deviceReadPackge->deviceQuality[0].dateTime.day, deviceReadPackge->deviceQuality[0].dateTime.hour,
					deviceReadPackge->deviceQuality[0].dateTime.min, deviceReadPackge->deviceQuality[0].dateTime.sec,
					httpClientBuffer->systemSetData.deviceId, deviceReadPackge->deviceQuality[0].operator.name,
					deviceReadPackge->deviceQuality[0].itemName, deviceReadPackge->deviceQuality[0].desc);
				
				if(deviceReadPackge->deviceQuality[0].isOk)
					strcat(httpClientBuffer->tempBuffer, "result=true&\0");
				else
					strcat(httpClientBuffer->tempBuffer, "result=false&\0");
				
				strcat(httpClientBuffer->tempBuffer, "measurev=[\0");
				for(i=0; i<DeviceQualityMaxTestCount; i++)
				{
					if(deviceReadPackge->deviceQuality[0].testValue[i] != 0.0)
					{
						snprintf(httpClientBuffer->tempbuf2, 15, "%.3f,", deviceReadPackge->deviceQuality[0].testValue[i]);

						strcat(httpClientBuffer->tempBuffer, httpClientBuffer->tempbuf2);
					}
				}
				httpClientBuffer->tempBuffer[strlen(httpClientBuffer->tempBuffer)-1] = 0;
				strcat(httpClientBuffer->tempBuffer, "]\0");
				
				sprintf(httpClientBuffer->httpPostBuffer.sendBuf, "POST /NCDPOCT_Server/%s HTTP/1.1\nHost: %d.%d.%d.%d:%d\nConnection: keep-alive\nContent-Length: %d\nContent-Type:application/x-www-form-urlencoded;charset=GBK\nAccept-Language: zh-CN,zh;q=0.8\n\n%s\0",
					UpLoadDeviceQualityUrl, httpClientBuffer->systemSetData.serverSet.serverIP.ip_1, httpClientBuffer->systemSetData.serverSet.serverIP.ip_2,
					httpClientBuffer->systemSetData.serverSet.serverIP.ip_3, httpClientBuffer->systemSetData.serverSet.serverIP.ip_4,
					httpClientBuffer->systemSetData.serverSet.serverPort, strlen(httpClientBuffer->tempBuffer), httpClientBuffer->tempBuffer);
				httpClientBuffer->httpPostBuffer.sendBufferLen = strlen(httpClientBuffer->httpPostBuffer.sendBuf);
				
				IP4_ADDR(&(httpClientBuffer->httpPostBuffer.server_ip), httpClientBuffer->systemSetData.serverSet.serverIP.ip_1, httpClientBuffer->systemSetData.serverSet.serverIP.ip_2,
					httpClientBuffer->systemSetData.serverSet.serverIP.ip_3, httpClientBuffer->systemSetData.serverSet.serverIP.ip_4);
				httpClientBuffer->httpPostBuffer.server_port = httpClientBuffer->systemSetData.serverSet.serverPort;
				
				if(My_Pass == PostData(&(httpClientBuffer->httpPostBuffer)))
				{
					plusDeviceQualityHeaderuserUpLoadIndexToFile(1);
				}
			}
		}
	}
	
	MyFree(httpClientBuffer);
	MyFree(deviceReadPackge);
}

/***************************************************************************************************
*FunctionName:  queryRemoteFirmwareVersion
*Description:  读取服务器软件版本信息
*Input:  
*Output:  
*Return:  
*Author:  xsx
*Date: 2017年7月5日 11:12:33
***************************************************************************************************/
static void queryRemoteFirmwareVersion(void)
{
	HttpClientBuffer * httpClientBuffer = NULL;
	RemoteSoftInfo * remoteSoftInfo = NULL;
	
	httpClientBuffer = MyMalloc(httpClientBufferStructSize);
	remoteSoftInfo = MyMalloc(RemoteSoftInfoStructSize);
	
	if(httpClientBuffer && remoteSoftInfo)
	{
		memcpy(&(httpClientBuffer->systemSetData), getGBSystemSetData(), SystemSetDataStructSize);
		
		//组合请求参数
		snprintf(httpClientBuffer->tempBuffer, 100, "softName=%s", httpClientBuffer->systemSetData.deviceId);
		snprintf(httpClientBuffer->httpPostBuffer.sendBuf, 1024, "POST /NCD_Server/%s HTTP/1.1\nHost: %d.%d.%d.%d:%d\nConnection: keep-alive\nContent-Length: %d\nContent-Type:application/x-www-form-urlencoded;charset=GBK\nAccept-Language: zh-CN,zh;q=0.8\n\n%s",
			QueryRemoteSoftVersionUrl, NCD_ServerIp_1, NCD_ServerIp_2, NCD_ServerIp_3, NCD_ServerIp_4, NCD_ServerPort, strlen(httpClientBuffer->tempBuffer), httpClientBuffer->tempBuffer);
		httpClientBuffer->httpPostBuffer.sendBufferLen = strlen(httpClientBuffer->httpPostBuffer.sendBuf);
		
		IP4_ADDR(&(httpClientBuffer->httpPostBuffer.server_ip), NCD_ServerIp_1, NCD_ServerIp_2, NCD_ServerIp_3, NCD_ServerIp_4);
		httpClientBuffer->httpPostBuffer.server_port = NCD_ServerPort;
		
		if(My_Pass == PostData(&(httpClientBuffer->httpPostBuffer)))
		{
			if(My_Pass == ParseJsonToRemoteSoftInfo(httpClientBuffer->httpPostBuffer.recvBuf, remoteSoftInfo))
			{
				//如果读取到的版本，大于当前版本，且大于当前保存的最新远程版本，则此次读取的是最新的
				if((remoteSoftInfo->RemoteFirmwareVersion > GB_SoftVersion) &&
					(remoteSoftInfo->RemoteFirmwareVersion > getGbRemoteFirmwareVersion()))
				{
					if(My_Pass == WriteRemoteSoftInfo(remoteSoftInfo))
					{
						//md5保存成功后，才更新最新版本号，保存最新固件版本
						setGbRemoteFirmwareVersion(remoteSoftInfo->RemoteFirmwareVersion);
						setGbRemoteFirmwareMd5(remoteSoftInfo->md5);
						setIsSuccessDownloadFirmware(false);
					}
				}
			}
		}
	}
	
	MyFree(httpClientBuffer);
	MyFree(remoteSoftInfo);
}

/***************************************************************************************************
*FunctionName:  DownLoadFirmware
*Description:  下载更新文件
*Input:  
*Output:  
*Return:  
*Author:  xsx
*Date: 2017年7月5日 14:21:36
***************************************************************************************************/
static void DownLoadFirmware(void)
{
	HttpClientBuffer * httpClientBuffer = NULL;
	
	//检查是否有更新，且未成功下载，则需要下载
	if((getGbRemoteFirmwareVersion() > GB_SoftVersion) && (false == getIsSuccessDownloadFirmware()))
	{
		httpClientBuffer = MyMalloc(httpClientBufferStructSize);
	
		if(httpClientBuffer)
		{
			memcpy(&(httpClientBuffer->systemSetData), getGBSystemSetData(), SystemSetDataStructSize);
			
			//组合请求参数
			snprintf(httpClientBuffer->httpPostBuffer.sendBuf, 1024, "GET /NCD_Server/%s?softName=%s HTTP/1.1\nHost: %d.%d.%d.%d:%d\nConnection: keep-alive\n\n",
				DownRemoteSoftFileUrl, httpClientBuffer->systemSetData.deviceId, NCD_ServerIp_1, NCD_ServerIp_2, NCD_ServerIp_3, NCD_ServerIp_4, NCD_ServerPort);
			httpClientBuffer->httpPostBuffer.sendBufferLen = strlen(httpClientBuffer->httpPostBuffer.sendBuf);
		
			IP4_ADDR(&(httpClientBuffer->httpPostBuffer.server_ip), NCD_ServerIp_1, NCD_ServerIp_2, NCD_ServerIp_3, NCD_ServerIp_4);
			httpClientBuffer->httpPostBuffer.server_port = NCD_ServerPort;
			
			MyDownLoadSoftFile(&(httpClientBuffer->httpPostBuffer));
		}
		MyFree(httpClientBuffer);
	}
}
/****************************************end of file************************************************/
