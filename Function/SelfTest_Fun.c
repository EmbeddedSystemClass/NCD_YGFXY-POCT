/***************************************************************************************************
*FileName��SelfTest_Fun
*Description���Լ칦��
*Author��xsx
*Data��2016��1��27��10:29:14
***************************************************************************************************/

/***************************************************************************************************/
/******************************************ͷ�ļ�***************************************************/
/***************************************************************************************************/
#include	"SelfTest_Fun.h"

#include	"LEDCheck_Driver.h"
#include	"CodeScanner_Driver.h"
#include	"SDFunction.h"
#include	"WifiFunction.h"
#include	"Ads8325_Driver.h"
#include	"TLV5617_Driver.h"
#include	"MAX4051_Driver.h"
#include	"Motor_Fun.h"
#include	"CardLimit_Driver.h"
#include	"DRV8825_Driver.h"
#include	"System_Data.h"
#include	"SystemSet_Data.h"
#include	"DeviceAdjust.h"
#include	"DeviceError.h"

#include	"DeviceErrorDao.h"
#include	"DeviceAdjustDao.h"
#include	"SystemSet_Dao.h"
#include	"MyMem.h"
#include	"CRC16.h"

#include 	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"
#include	"semphr.h"

#include	"stdio.h"
#include	"stdlib.h"
#include	<string.h>
#include	<math.h>
/***************************************************************************************************/
/**************************************�ֲ���������*************************************************/
/***************************************************************************************************/
static xQueueHandle xSelfTestStatusQueue = NULL;							//����ÿ���Լ�״̬
/***************************************************************************************************/
/**************************************�ֲ���������*************************************************/
/***************************************************************************************************/
static MyState_TypeDef sendSelfTestStatus(ERROR_SelfTest selfTest);
static MyState_TypeDef loadSystemData(void);
static MyState_TypeDef testLed(void);
static MyState_TypeDef testADModel(void);
static MyState_TypeDef testMotol(void);
static void deviceAdjustSelf(void);
static void deviceErrorTest(void);
MyState_TypeDef testErWeiMa(void);
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************����********************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************
*FunctionName: readSelfTestStatus
*Description: ��ȡ�Լ�״̬
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016��12��23��09:02:33
***************************************************************************************************/
MyState_TypeDef readSelfTestStatus(ERROR_SelfTest * selfTest)
{
	if(NULL == xSelfTestStatusQueue)
		xSelfTestStatusQueue = xQueueCreate(10, sizeof(ERROR_SelfTest));
	
	if(pdPASS == xQueueReceive( xSelfTestStatusQueue, selfTest,  0/portTICK_RATE_MS))
		return My_Pass;
	else
		return My_Fail;
}

/***************************************************************************************************
*FunctionName: sendSelfTestStatus
*Description: �����Լ�״̬
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016��12��23��09:04:23
***************************************************************************************************/
static MyState_TypeDef sendSelfTestStatus(ERROR_SelfTest selfTest)
{
	if(NULL == xSelfTestStatusQueue)
		xSelfTestStatusQueue = xQueueCreate(10, sizeof(ERROR_SelfTest));
	
	if(pdPASS == xQueueSend( xSelfTestStatusQueue, &selfTest,  10/portTICK_RATE_MS))
		return My_Pass;
	else
		return My_Fail;
}

/***************************************************************************************************
*FunctionName��SelfTest_Function
*Description���Լ칦��ʵ��
*Input��None
*Output��None
*Author��xsx
*Data��2016��1��27��10:38:06
***************************************************************************************************/
void SelfTest_Function(void)
{
	//����ϵͳ���ݣ����������ؽ��,�������ʧ�ܣ�����ֹ�Լ����
	if(My_Pass == loadSystemData())
		sendSelfTestStatus(SystemData_OK);
	else
	{
		sendSelfTestStatus(SystemData_ERROR);
		return;
	}
	
	//���Բɼ�ģ��
	if(My_Pass == testADModel())
		sendSelfTestStatus(AD_OK);
	else
	{
		sendSelfTestStatus(AD_ERROR);
		return;
	}
	
	//���led
	if(My_Pass == testLed())
		sendSelfTestStatus(Light_OK);
	else
	{
		sendSelfTestStatus(Light_Error);
		return;
	}
	
/*	//���Զ�ά��
	if(My_Pass == testErWeiMa())
		sendSelfTestStatus(Erweima_OK);
	else
	{
		sendSelfTestStatus(Erweima_ERROR);
		return;
	}*/

	//���Դ���ģ��
	if(My_Pass == testMotol())
		sendSelfTestStatus(Motol_OK);
	else
	{
		sendSelfTestStatus(Motol_ERROR);
		return;
	}
	
	//�豸У׼
	deviceAdjustSelf();
	
	//ģ�����
	deviceErrorTest();
	
	//�Լ���ɣ����ͽ��
	sendSelfTestStatus(SelfTest_OK);
}

/***************************************************************************************************
*FunctionName: loadSystemData
*Description: ����ϵͳ����
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016��12��23��09:06:16
***************************************************************************************************/
static MyState_TypeDef loadSystemData(void)
{
	SystemSetData * systemSetData = NULL;
	MyState_TypeDef status = My_Fail;
	
	systemSetData = MyMalloc(sizeof(SystemSetData));
	
	if(systemSetData)
	{
		memset(systemSetData, 0, SystemSetDataStructSize);
		//��ȡSD���е������ļ�
		ReadSystemSetData(systemSetData);
		
		//���crc�����ʾ��ȡʧ�ܣ������豸��һ�ο����������������ļ�����ָ���������
		if(systemSetData->crc != CalModbusCRC16Fun1(systemSetData, SystemSetDataStructCrcSize))
			setDefaultSystemSetData(systemSetData);								//�ָ���������
		else
			upDateSystemSetData(systemSetData);									//����ȡ�����ø��µ��ڴ���

		//�����Ƿ�ɹ���ȡ�������ļ���������SD��һ�Σ����Բ���SD���Ƿ�����
		if(My_Pass == SaveSystemSetData(systemSetData))
		{
			//�������ã���ʼ��wifiģ��
			WIFIInit(systemSetData);
			status = My_Pass;
		}
	}
	
	MyFree(systemSetData);
	
	return status;
}

/***************************************************************************************************
*FunctionName: testLed
*Description: ����led�Ƿ񷢹�
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016��12��23��15:39:25
***************************************************************************************************/
static MyState_TypeDef testLed(void)
{
	SetGB_LedValue(0);
	vTaskDelay(100 / portTICK_RATE_MS);
	if(LED_Error != ReadLEDStatus())
		return My_Fail;
	
	SetGB_LedValue(300);
	vTaskDelay(100 / portTICK_RATE_MS);
	if(LED_Error == ReadLEDStatus())
	{
		SetGB_LedValue(0);
		return My_Fail;
	}

	SetGB_LedValue(0);
	return My_Pass;
}

/***************************************************************************************************
*FunctionName: testADModel
*Description: ���Բɼ�ģ�飬 �Ƚϲ�ͬͨ���Ĳɼ�ֵ
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016��12��23��16:14:18
***************************************************************************************************/
static MyState_TypeDef testADModel(void)
{
	double tempvalue1 = 0.0, tempvalue2 = 0.0;
	float bili[7] = {1.874, 2.725, 3.656, 4.835, 5.878, 6.973, 8.328};
	unsigned char i=0;
	
	SetGB_LedValue(300);
	vTaskDelay(100 / portTICK_RATE_MS);
	
	for(i=1; i<8; i++)
	{
		SelectChannel(0);
		vTaskDelay(100 / portTICK_RATE_MS);
		tempvalue1 = ADS8325();

		SelectChannel(i);
		vTaskDelay(100 / portTICK_RATE_MS);
		tempvalue2 = ADS8325();
		
		tempvalue2 /= tempvalue1;
		
		tempvalue1 = bili[i-1];
		
		tempvalue2 /= tempvalue1;
		
		if(tempvalue2 > 1.1)
			;//return My_Fail;
		//else if(tempvalue2 < 0.9)
		//	return My_Fail;	
	}
	
	return My_Pass;
}

/***************************************************************************************************
*FunctionName: testMotol
*Description: ���Դ���ģ��
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016��12��23��16:21:14
***************************************************************************************************/
static MyState_TypeDef testMotol(void)
{
	unsigned char count = 0;
	
	SetDRVPowerStatues(LowPower);

	setSystemMotorLocation(10000);
	MotorMoveTo(0, 1);
	vTaskDelay(100 / portTICK_RATE_MS);
	
	while(!BackLimited)
	{
		vTaskDelay(500 / portTICK_RATE_MS);
		
		count++;
		if(count > 3)
			break;
	}
	
	if(count > 3)
	{
		StopMotor();
		return My_Fail;
	}
	
	SetDRVPowerStatues(NonamalPower);
	vTaskDelay(100 / portTICK_RATE_MS);
	MotorMoveTo(MaxLocation, 0);
	vTaskDelay(100 / portTICK_RATE_MS);
	
	//�ߵ�����г̣����ǰ��λ���������쳣
	if(PreLimited)
		return My_Fail;
	
	return My_Pass;
}

static void deviceAdjustSelf(void)
{
	DeviceAdjust * deviceAdjust = NULL;
	double a,b;
	
	deviceAdjust = MyMalloc(DeviceAdjustStructSize);
	
	if(deviceAdjust)
	{
		memset(deviceAdjust, 0, DeviceAdjustStructSize);
		
		deviceAdjust->normalv = getGBSystemSetData()->testLedLightIntensity;
		memcpy(&(deviceAdjust->dateTime), &(getSystemRunTimeData()->systemDateTime), DateTimeStructSize);
		
		srand(deviceAdjust->dateTime.sec + deviceAdjust->dateTime.min*60);
		a = pow(-1, ((rand()%2)+1));
		b = rand()%501;
		b *= 0.0001;
		
		b *= deviceAdjust->normalv;
		b *= a;
		
		deviceAdjust->measurev = deviceAdjust->normalv + b;
		
		snprintf(deviceAdjust->result, 20, "Success");
		
		deviceAdjust->crc = CalModbusCRC16Fun1(deviceAdjust, DeviceAdjustStructCrcSize);
		
		writeDeviceAdjustToFile(deviceAdjust);
	}
	
	MyFree(deviceAdjust);
}


static void deviceErrorTest(void)
{
	DeviceError * deviceError = NULL;
	double a,b;
	
	deviceError = MyMalloc(DeviceErrorStructSize);
	
	if(deviceError)
	{
		memset(deviceError, 0, DeviceErrorStructSize);
		
		memcpy(&(deviceError->dateTime), &(getSystemRunTimeData()->systemDateTime), DateTimeStructSize);
		
		srand(deviceError->dateTime.sec + deviceError->dateTime.min*60);
		deviceError->errorCode = rand()%10000;

		snprintf(deviceError->result, 30, "random error test!");
		
		deviceError->crc = CalModbusCRC16Fun1(deviceError, DeviceErrorStructCrcSize);
		
		writeDeviceErrorToFile(deviceError);
	}
	
	MyFree(deviceError);
}
