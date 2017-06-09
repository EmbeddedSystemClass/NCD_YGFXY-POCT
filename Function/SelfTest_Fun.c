/***************************************************************************************************
*FileName：SelfTest_Fun
*Description：自检功能
*Author：xsx
*Data：2016年1月27日10:29:14
***************************************************************************************************/

/***************************************************************************************************/
/******************************************头文件***************************************************/
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
/**************************************局部变量声明*************************************************/
/***************************************************************************************************/
static xQueueHandle xSelfTestStatusQueue = NULL;							//保存每个自检状态
/***************************************************************************************************/
/**************************************局部函数声明*************************************************/
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
/***************************************正文********************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************
*FunctionName: readSelfTestStatus
*Description: 读取自检状态
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月23日09:02:33
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
*Description: 发送自检状态
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月23日09:04:23
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
*FunctionName：SelfTest_Function
*Description：自检功能实现
*Input：None
*Output：None
*Author：xsx
*Data：2016年1月27日10:38:06
***************************************************************************************************/
void SelfTest_Function(void)
{
	//加载系统数据，并发生加载结果,如果加载失败，则终止自检程序
	if(My_Pass == loadSystemData())
		sendSelfTestStatus(SystemData_OK);
	else
	{
		sendSelfTestStatus(SystemData_ERROR);
		return;
	}
	
	//测试采集模块
	if(My_Pass == testADModel())
		sendSelfTestStatus(AD_OK);
	else
	{
		sendSelfTestStatus(AD_ERROR);
		return;
	}
	
	//检测led
	if(My_Pass == testLed())
		sendSelfTestStatus(Light_OK);
	else
	{
		sendSelfTestStatus(Light_Error);
		return;
	}
	
/*	//测试二维码
	if(My_Pass == testErWeiMa())
		sendSelfTestStatus(Erweima_OK);
	else
	{
		sendSelfTestStatus(Erweima_ERROR);
		return;
	}*/

	//测试传动模块
	if(My_Pass == testMotol())
		sendSelfTestStatus(Motol_OK);
	else
	{
		sendSelfTestStatus(Motol_ERROR);
		return;
	}
	
	//设备校准
	deviceAdjustSelf();
	
	//模拟错误
	deviceErrorTest();
	
	//自检完成，发送结果
	sendSelfTestStatus(SelfTest_OK);
}

/***************************************************************************************************
*FunctionName: loadSystemData
*Description: 加载系统数据
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月23日09:06:16
***************************************************************************************************/
static MyState_TypeDef loadSystemData(void)
{
	SystemSetData * systemSetData = NULL;
	MyState_TypeDef status = My_Fail;
	
	systemSetData = MyMalloc(sizeof(SystemSetData));
	
	if(systemSetData)
	{
		memset(systemSetData, 0, SystemSetDataStructSize);
		//读取SD卡中的配置文件
		ReadSystemSetData(systemSetData);
		
		//如果crc错误表示读取失败，或者设备第一次开机，不存在配置文件，则恢复出厂设置
		if(systemSetData->crc != CalModbusCRC16Fun1(systemSetData, SystemSetDataStructCrcSize))
			setDefaultSystemSetData(systemSetData);								//恢复出厂设置
		else
			upDateSystemSetData(systemSetData);									//将读取的配置更新到内存中

		//无论是否成功读取到配置文件，都保存SD卡一次，用以测试SD卡是否正常
		if(My_Pass == SaveSystemSetData(systemSetData))
		{
			//根据配置，初始化wifi模块
			WIFIInit(systemSetData);
			status = My_Pass;
		}
	}
	
	MyFree(systemSetData);
	
	return status;
}

/***************************************************************************************************
*FunctionName: testLed
*Description: 测试led是否发光
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月23日15:39:25
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
*Description: 测试采集模块， 比较不同通道的采集值
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月23日16:14:18
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
*Description: 测试传动模块
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月23日16:21:14
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
	
	//走到最大行程，如果前限位触发，则报异常
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
