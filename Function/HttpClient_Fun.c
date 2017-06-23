/***************************************************************************************************
*FileName:
*Description:
*Author:
*Data:
***************************************************************************************************/

/***************************************************************************************************/
/******************************************ͷ�ļ�***************************************************/
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
/**************************************�ֲ���������*************************************************/
/***************************************************************************************************/

/***************************************************************************************************/
/**************************************�ֲ���������*************************************************/
/***************************************************************************************************/
static MyState_TypeDef readTimeFromServer(void);
static void queryDeviceInfo(void);
static void upLoadDeviceAdjustRecord(void);
static void upLoadYGFXYDataRecord(void);
static void upLoadDeviceErrorRecord(void);
static void upLoadDeviceMaintenanceRecord(void);
static void upLoadDeviceQualityRecord(void);
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************����********************************************************/
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
		
		//����������
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
			
			//���ͳɹ���������״̬����
			status = My_Pass;
		}
	}
	
	MyFree(httpClientBuffer);
	
	return status;
}

/***************************************************************************************************
*FunctionName:  queryDeviceInfo
*Description:  ��ȡ�������е��豸��Ϣ������汾�ȱ��ذ汾�ߣ�����±�������
*Input:  
*Output:  
*Return:  
*Author:  xsx
*Date: 2017��6��5�� 10:23:44
***************************************************************************************************/
static void queryDeviceInfo(void)
{
	HttpClientBuffer * httpClientBuffer = NULL;
	Device * tempDevice = NULL;
	
	httpClientBuffer = MyMalloc(httpClientBufferStructSize);
	
	if(httpClientBuffer)
	{
		memcpy(&(httpClientBuffer->systemSetData), getGBSystemSetData(), SystemSetDataStructSize);
		
		//����������
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
				//��ȡ��ǰ�豸��Ϣ
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

/***************************************************************************************************
*FunctionName:  upLoadDeviceAdjustRecord
*Description:  �ϴ��豸��У׼��¼
*Input:  
*Output:  
*Return:  
*Author:  xsx
*Date: 2017��6��5�� 10:24:37
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
		
		//��ȡУ׼��¼�Լ�ͷ��Ϣ
		memset(&(deviceAdjustReadPackge->pageRequest), 0, PageRequestStructSize);					//�������pagerequest���������ϴ���������ȡ
		if(My_Pass == readDeviceAdjustFromFile(deviceAdjustReadPackge))
		{
			if(deviceAdjustReadPackge->deviceAdjust[0].crc == CalModbusCRC16Fun1(&(deviceAdjustReadPackge->deviceAdjust[0]), DeviceAdjustStructCrcSize))
			{
				//����������
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
					plusDeviceAdjustHeaderUpLoadIndexToFile(1);
				}
			}
		}
	}
	
	MyFree(httpClientBuffer);
	MyFree(deviceAdjustReadPackge);
}

/***************************************************************************************************
*FunctionName:  upLoadYGFXYDataRecord
*Description:  �ϴ���������
*Input:  
*Output:  
*Return:  
*Author:  xsx
*Date: 2017��6��6�� 08:52:16
***************************************************************************************************/
static void upLoadYGFXYDataRecord(void)
{
	HttpClientBuffer * httpClientBuffer = NULL;
	unsigned short i=0;
	TestDataRecordReadPackage * testDataRecordReadPackage = NULL;
	
	httpClientBuffer = MyMalloc(httpClientBufferStructSize);
	testDataRecordReadPackage = MyMalloc(TestDataRecordReadPackageStructSize);
	
	if(httpClientBuffer && testDataRecordReadPackage)
	{
		memcpy(&(httpClientBuffer->systemSetData), getGBSystemSetData(), SystemSetDataStructSize);
		
		//��ȡУ׼��¼�Լ�ͷ��Ϣ
		memset(&(testDataRecordReadPackage->pageRequest), 0, PageRequestStructSize);					//�������pagerequest���������ϴ���������ȡ
		if(My_Pass == readTestDataFromFile(testDataRecordReadPackage))
		{
			if(testDataRecordReadPackage->testData[0].crc == CalModbusCRC16Fun1(&(testDataRecordReadPackage->testData[0]), TestDataStructCrcSize))
			{
				//����������
				sprintf(httpClientBuffer->tempBuffer, "device.did=%s&item.code=%s&operator.name=%s&cardlot=%s&cardnum=%s&sampleid=%s&testtime=20%02d-%02d-%02d %02d:%02d:%02d&ambienttemp=%.2f&cardtemp=%.2f&overtime=%d&cline=%d&bline=%d&tline=%d&testv=%.*f&serialnum=%s-%s&\0", 
					httpClientBuffer->systemSetData.deviceId, testDataRecordReadPackage->testData[0].qrCode.itemConstData.itemCode, 
					testDataRecordReadPackage->testData[0].operator.name, testDataRecordReadPackage->testData[0].qrCode.PiHao,
					testDataRecordReadPackage->testData[0].qrCode.piNum, testDataRecordReadPackage->testData[0].sampleid,
					testDataRecordReadPackage->testData[0].testDateTime.year, testDataRecordReadPackage->testData[0].testDateTime.month, 
					testDataRecordReadPackage->testData[0].testDateTime.day, testDataRecordReadPackage->testData[0].testDateTime.hour,
					testDataRecordReadPackage->testData[0].testDateTime.min, testDataRecordReadPackage->testData[0].testDateTime.sec,
					testDataRecordReadPackage->testData[0].temperature.E_Temperature, testDataRecordReadPackage->testData[0].temperature.O_Temperature,
					testDataRecordReadPackage->testData[0].time, testDataRecordReadPackage->testData[0].testSeries.C_Point[1],
					testDataRecordReadPackage->testData[0].testSeries.B_Point[1], testDataRecordReadPackage->testData[0].testSeries.T_Point[1],
					testDataRecordReadPackage->testData[0].qrCode.itemConstData.pointNum, testDataRecordReadPackage->testData[0].testSeries.AdjustResult,
					testDataRecordReadPackage->testData[0].qrCode.PiHao, testDataRecordReadPackage->testData[0].qrCode.piNum);
				
				if(testDataRecordReadPackage->testData[0].testResultDesc == ResultIsOK)
					strcat(httpClientBuffer->tempBuffer, "t_isok=true&\0");
				else
					strcat(httpClientBuffer->tempBuffer, "t_isok=false&\0");	
				
				strcat(httpClientBuffer->tempBuffer, "series=[\0");
				for(i=0; i<100; i++)
				{
					if(i == 0)
						sprintf(httpClientBuffer->tempbuf2, "%d,%d,%d\0", testDataRecordReadPackage->testData[0].testSeries.TestPoint[i*3],
							testDataRecordReadPackage->testData[0].testSeries.TestPoint[i*3+1], testDataRecordReadPackage->testData[0].testSeries.TestPoint[i*3+2]);
					else
						sprintf(httpClientBuffer->tempbuf2, ",%d,%d,%d\0", testDataRecordReadPackage->testData[0].testSeries.TestPoint[i*3],
							testDataRecordReadPackage->testData[0].testSeries.TestPoint[i*3+1], testDataRecordReadPackage->testData[0].testSeries.TestPoint[i*3+2]);
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
					plusTestDataHeaderUpLoadIndexToFile(1);
				}
			}
		}
	}
	
	MyFree(httpClientBuffer);
	MyFree(testDataRecordReadPackage);
}

/***************************************************************************************************
*FunctionName:  upLoadDeviceErrorRecord
*Description:  �ϴ��豸�쳣��¼
*Input:  
*Output:  
*Return:  
*Author:  xsx
*Date: 2017��6��8�� 16:29:49
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
		
		//��ȡУ׼��¼�Լ�ͷ��Ϣ
		memset(&(deviceErrorReadPackge->pageRequest), 0, PageRequestStructSize);					//�������pagerequest���������ϴ���������ȡ
		if(My_Pass == readDeviceErrorFromFile(deviceErrorReadPackge))
		{
			if(deviceErrorReadPackge->deviceError[0].crc == CalModbusCRC16Fun1(&(deviceErrorReadPackge->deviceError[0]), DeviceErrorStructCrcSize))
			{
				//����������
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
					plusDeviceErrorHeaderUpLoadIndexToFile(1);
				}
			}
		}
	}
	
	MyFree(httpClientBuffer);
	MyFree(deviceErrorReadPackge);
}

/***************************************************************************************************
*FunctionName:  upLoadDeviceMaintenanceRecord
*Description:  �ϴ��豸ά����¼
*Input:  
*Output:  
*Return:  
*Author:  xsx
*Date: 2017��6��16�� 11:09:29
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
		
		//��ȡУ׼��¼�Լ�ͷ��Ϣ
		memset(&(deviceReadPackge->pageRequest), 0, PageRequestStructSize);					//�������pagerequest���������ϴ���������ȡ
		if(My_Pass == readDeviceMaintenanceFromFile(deviceReadPackge))
		{
			if(deviceReadPackge->deviceMaintenance[0].crc == CalModbusCRC16Fun1(&(deviceReadPackge->deviceMaintenance[0]), DeviceMaintenanceStructCrcSize))
			{
				//����������
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
					plusDeviceMaintenanceHeaderUpLoadIndexToFile(1);
				}
			}
		}
	}
	
	MyFree(httpClientBuffer);
	MyFree(deviceReadPackge);
}

/***************************************************************************************************
*FunctionName:  upLoadDeviceQualityRecord
*Description:  �ϴ��ʿؼ�¼
*Input:  
*Output:  
*Return:  
*Author:  xsx
*Date: 2017��6��16�� 13:58:22
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
		
		//��ȡУ׼��¼�Լ�ͷ��Ϣ
		memset(&(deviceReadPackge->pageRequest), 0, PageRequestStructSize);					//�������pagerequest���������ϴ���������ȡ
		if(My_Pass == readDeviceQualityFromFile(deviceReadPackge))
		{
			if(deviceReadPackge->deviceQuality[0].crc == CalModbusCRC16Fun1(&(deviceReadPackge->deviceQuality[0]), DeviceQualityStructCrcSize))
			{
				//����������
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
					plusDeviceQualityHeaderUpLoadIndexToFile(1);
				}
			}
		}
	}
	
	MyFree(httpClientBuffer);
	MyFree(deviceReadPackge);
}
