/***************************************************************************************************
*FileName: MaintenancePage
*Description: 设备维护
*Author:xsx
*Data: 2017年6月14日 15:51:52
***************************************************************************************************/

/***************************************************************************************************/
/******************************************头文件***************************************************/
/***************************************************************************************************/
#include	"QualityPage.h"

#include	"CardLimit_Driver.h"
#include	"LCD_Driver.h"
#include	"Define.h"
#include	"MyMem.h"
#include	"CRC16.h"
#include	"MyTools.h"

#include	"Motor_Fun.h"
#include	"Quality_Data.h"
#include	"SystemSetPage.h"
#include	"SelectUserPage.h"
#include	"CodeScan_Task.h"
#include	"Test_Task.h"
#include	"System_Data.h"
#include	"DeviceQualityDao.h"

#include	<string.h>
#include	"stdio.h"
#include 	"stdlib.h"
/***************************************************************************************************/
/**************************************变量声明*************************************************/
/***************************************************************************************************/
static QualityPageBuffer * pageBuffer = NULL;
/***************************************************************************************************/
/**************************************局部函数声明*************************************************/
/***************************************************************************************************/
static void activityStart(void);
static void activityInput(unsigned char *pbuf , unsigned short len);
static void activityFresh(void);
static void activityHide(void);
static void activityResume(void);
static void activityDestroy(void);

static MyRes activityBufferMalloc(void);
static void activityBufferFree(void);
static void checkQRCode(void);
static void checkCardTestResult(void);
static void showStatus(const char * str);
static void showThisTestResult(void);
static void showAllPianCha(void);
static void clearPageText(void);
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************正文********************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************
*FunctionName: createLunchActivity
*Description: 创建主界面
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月21日09:00:09
***************************************************************************************************/
MyRes createQualityActivity(Activity * thizActivity, Intent * pram)
{
	if(NULL == thizActivity)
		return My_Fail;
	
	if(My_Pass == activityBufferMalloc())
	{
		InitActivity(thizActivity, QualityActivityName, activityStart, activityInput, activityFresh, activityHide, activityResume, activityDestroy);
		
		return My_Pass;
	}
	
	return My_Fail;
}

/***************************************************************************************************
*FunctionName: activityStart
*Description: 显示主界面
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月21日09:00:32
***************************************************************************************************/
static void activityStart(void)
{
	pageBuffer->deviceQuality = getGB_DeviceQuality();
	pageBuffer->testIndex = 0;
	pageBuffer->canTestNow = false;
	pageBuffer->isTestting = false;
	pageBuffer->resultSum = 0.0;
	
	clearPageText();
	
	showStatus("请插入试剂卡\0");
	SelectPage(158);
}

/***************************************************************************************************
*FunctionName: activityInput
*Description: 界面输入
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月21日09:00:59
***************************************************************************************************/
static void activityInput(unsigned char *pbuf , unsigned short len)
{
	/*命令*/
	pageBuffer->lcdinput[0] = pbuf[4];
	pageBuffer->lcdinput[0] = (pageBuffer->lcdinput[0]<<8) + pbuf[5];
		
	//返回
	if(pageBuffer->lcdinput[0] == 0x3400)
	{
		deleteGB_DeviceQuality();
		backToActivity(SystemSetActivityName);
	}
	//提交
	else if(pageBuffer->lcdinput[0] == 0x3401)
	{
		memcpy(&(pageBuffer->deviceQuality->dateTime), &(getSystemRunTimeData()->systemDateTime), DateTimeStructSize);
		pageBuffer->deviceQuality->crc = CalModbusCRC16Fun(pageBuffer->deviceQuality, DeviceQualityStructCrcSize, NULL);
		
		if(My_Pass == writeDeviceQualityToFile(pageBuffer->deviceQuality))
		{
			deleteGB_DeviceQuality();
			backToActivity(SystemSetActivityName);
		}
		else
			SendKeyCode(2);
	}
	//设置标准值
	else if(pageBuffer->lcdinput[0] == 0x3410)
	{
		pageBuffer->deviceQuality->standardValue = strtod((char *)&pbuf[7], NULL);
		showAllPianCha();
	}
	//结果
	else if(pageBuffer->lcdinput[0] == 0x3302)
	{
		/*数据*/
		pageBuffer->lcdinput[1] = pbuf[7];
		pageBuffer->lcdinput[1] = (pageBuffer->lcdinput[1]<<8) + pbuf[8];
			
		/*自动获取ip*/
		if(pageBuffer->lcdinput[1] == 0x8000)
			pageBuffer->deviceQuality->isOk = true;
		/*使用设置的ip*/
		else if(pageBuffer->lcdinput[1] == 0x0000)
			pageBuffer->deviceQuality->isOk = false;
	}
	//维护说明
	else if(pageBuffer->lcdinput[0] == 0x3310)
	{
		snprintf(pageBuffer->deviceQuality->desc, GetBufLen(&pbuf[7], 2*pbuf[6])+1, "%s",&pbuf[7]);
	}
}

/***************************************************************************************************
*FunctionName: activityFresh
*Description: 界面刷新
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月21日09:01:16
***************************************************************************************************/
static void activityFresh(void)
{
	//如果没在测试，且卡槽无卡，则现在允许测试
	if((pageBuffer->isTestting == false) && (MaxLocation == getSystemRunTimeData()->motorData.location) && (CardPinIn == NoCard))
		pageBuffer->canTestNow = true;
	
	//如果现在允许测试，且测试次数未到最大次数，且有卡插入，则开始测试
	if((pageBuffer->canTestNow) && (pageBuffer->testIndex < DeviceQualityMaxTestCount) && (CardPinIn == CardIN))
	{
		pageBuffer->canTestNow = false;
		pageBuffer->isTestting = true;
		
		memset(&(pageBuffer->testData), 0, TestDataStructSize);
		
		//第一步扫描二维码
		StartScanQRCode(&(pageBuffer->testData.qrCode));
		showStatus("开始扫描二维码\0");
	}
	
	if(pageBuffer->isTestting)
	{
		checkQRCode();
		
		checkCardTestResult();
	}
}

/***************************************************************************************************
*FunctionName: activityHide
*Description: 隐藏界面时要做的事
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月21日09:01:40
***************************************************************************************************/
static void activityHide(void)
{

}

/***************************************************************************************************
*FunctionName: activityResume
*Description: 界面恢复显示时要做的事
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月21日09:01:58
***************************************************************************************************/
static void activityResume(void)
{
	SelectPage(158);
}

/***************************************************************************************************
*FunctionName: activityDestroy
*Description: 界面销毁
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月21日09:02:15
***************************************************************************************************/
static void activityDestroy(void)
{
	activityBufferFree();
}

/***************************************************************************************************
*FunctionName: activityBufferMalloc
*Description: 界面数据内存申请
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 
***************************************************************************************************/
static MyRes activityBufferMalloc(void)
{
	if(NULL == pageBuffer)
	{
		pageBuffer = MyMalloc(QualityPageBufferStructSize);
		if(pageBuffer)	
		{
			memset(pageBuffer, 0, QualityPageBufferStructSize);
	
			return My_Pass;
		}
		else
			return My_Fail;
	}
	else
		return My_Pass;
}

/***************************************************************************************************
*FunctionName: activityBufferFree
*Description: 界面内存释放
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月21日09:03:10
***************************************************************************************************/
static void activityBufferFree(void)
{
	MyFree(pageBuffer);
	pageBuffer = NULL;
}


static void checkQRCode(void)
{
	if(My_Pass == TakeScanQRCodeResult(&(pageBuffer->cardScanResult)))
	{
		//不支持的品种
		if(pageBuffer->cardScanResult == CardUnsupported)
		{
			showStatus("不支持此项目\0");
			MotorMoveTo(MaxLocation, 1);
			SendKeyCode(6);
			pageBuffer->isTestting = false;
		}
		//过期
		else if(pageBuffer->cardScanResult == CardCodeTimeOut)
		{
			showStatus("过期\0");
			MotorMoveTo(MaxLocation, 1);
			SendKeyCode(4);
			pageBuffer->isTestting = false;
		}
		//读取成功
		else if(pageBuffer->cardScanResult == CardCodeScanOK)
		{
			//读取校准参数
			memcpy(pageBuffer->testData.adjustData.ItemName, pageBuffer->testData.qrCode.ItemName, ItemNameLen);
			getAdjPram(getGBSystemSetData(), &(pageBuffer->testData.adjustData));
			
			//保存质控项目名称
			if(strlen(pageBuffer->deviceQuality->itemName) == 0)
			{
				showStatus("开始扫描试剂卡\0");
				memcpy(pageBuffer->deviceQuality->itemName, pageBuffer->testData.qrCode.ItemName, ItemNameLen);
				StartTest(&(pageBuffer->testData));
			}
			else if(CheckStrIsSame(pageBuffer->deviceQuality->itemName, pageBuffer->testData.qrCode.ItemName, ItemNameLen))
			{
				showStatus("开始扫描试剂卡\0");
				StartTest(&(pageBuffer->testData));
			}
			else
			{
				showStatus("项目错误，请更换\0");
				MotorMoveTo(MaxLocation, 1);
				pageBuffer->isTestting = false;
			}
		}
		/*其他错误：CardCodeScanFail, CardCodeCardOut, CardCodeScanTimeOut, CardCodeCRCError*/
		else
		{
			showStatus("二维码错误\0");
			MotorMoveTo(MaxLocation, 1);
			SendKeyCode(1);
			pageBuffer->isTestting = false;
		}
	}
}

static void checkCardTestResult(void)
{
	if(My_Pass == TakeTestResult(&(pageBuffer->testData.testResultDesc)))
	{
		if(pageBuffer->testData.testResultDesc == NoSample)
		{
			showStatus("未加样\0");
		}
		else if(pageBuffer->testData.testResultDesc == PeakError)
		{
			showStatus("错误\0");
		}
		else if(pageBuffer->testData.testResultDesc == ResultIsOK)
		{
			//保存测试结果
			pageBuffer->deviceQuality->testValue[pageBuffer->testIndex] = pageBuffer->testData.testSeries.AdjustResult;
			showThisTestResult();
			
			pageBuffer->testIndex++;
			
			showStatus("当前测试完成\0");
		}
		
		MotorMoveTo(MaxLocation, 1);
		pageBuffer->isTestting = false;
	}
}

static void showStatus(const char * str)
{
	snprintf(pageBuffer->tempBuf, 50, "%s", str);
	DisText(0x34a0, pageBuffer->tempBuf, strlen(pageBuffer->tempBuf)+1);
}

static void showThisTestResult(void)
{
	//显示当前测试结果
	snprintf(pageBuffer->tempBuf, 15, "%.*f", pageBuffer->testData.qrCode.itemConstData.pointNum, pageBuffer->testData.testSeries.AdjustResult);
	DisText(0x3430+(pageBuffer->testIndex*0x05), pageBuffer->tempBuf, strlen(pageBuffer->tempBuf)+1);
	
	pageBuffer->resultSum += pageBuffer->testData.testSeries.AdjustResult;
	
	if(pageBuffer->deviceQuality->standardValue != 0)
	{
		//显示当前测试偏差率	
		pageBuffer->tempValue1 = (pageBuffer->testData.testSeries.AdjustResult - pageBuffer->deviceQuality->standardValue) 
					/ pageBuffer->deviceQuality->standardValue;
		pageBuffer->tempValue1 *= 100;
			
		snprintf(pageBuffer->tempBuf, 15, "%.2f%%", pageBuffer->tempValue1);
		DisText(0x3465+(pageBuffer->testIndex*0x05), pageBuffer->tempBuf, strlen(pageBuffer->tempBuf)+1);
		
		//更新平均偏差
		pageBuffer->tempValue2 = pageBuffer->resultSum / (pageBuffer->testIndex+1);
		pageBuffer->tempValue2 = (pageBuffer->tempValue2 - pageBuffer->deviceQuality->standardValue)/pageBuffer->deviceQuality->standardValue;
			
		pageBuffer->tempValue1 = pageBuffer->tempValue2*100;
				
		snprintf(pageBuffer->tempBuf, 15, "%.2f%%", pageBuffer->tempValue1);
		DisText(0x349a, pageBuffer->tempBuf, strlen(pageBuffer->tempBuf)+1);
	}
}

static void showAllPianCha(void)
{
	unsigned char i=0;
	if(pageBuffer->deviceQuality->standardValue != 0)
	{
		for(i=0; i<pageBuffer->testIndex; i++)
		{
			pageBuffer->tempValue1 = 
				(pageBuffer->deviceQuality->testValue[i] - pageBuffer->deviceQuality->standardValue) 
				/ pageBuffer->deviceQuality->standardValue;
			
			pageBuffer->tempValue1 *= 100;
			
			snprintf(pageBuffer->tempBuf, 15, "%.2f%%", pageBuffer->tempValue1);
			DisText(0x3465+(i*0x05), pageBuffer->tempBuf, strlen(pageBuffer->tempBuf)+1);
		}
		
		//更新平均偏差率
		if(pageBuffer->testIndex > 0)
		{
			pageBuffer->tempValue2 = pageBuffer->resultSum / pageBuffer->testIndex;
			pageBuffer->tempValue2 = (pageBuffer->tempValue2 - pageBuffer->deviceQuality->standardValue)/pageBuffer->deviceQuality->standardValue;
			
			pageBuffer->tempValue1 = pageBuffer->tempValue2*100;
				
			snprintf(pageBuffer->tempBuf, 15, "%.2f%%", pageBuffer->tempValue1);
			DisText(0x349a, pageBuffer->tempBuf, strlen(pageBuffer->tempBuf)+1);
		}
	}
}

static void clearPageText(void)
{
	unsigned char i=0;
	
	for(i=0; i<DeviceQualityMaxTestCount; i++)
	{
		ClearText(0x3430+(i*0x05));
		ClearText(0x3465+(i*0x05));
	}
	
	ClearText(0x349a);
	ClearText(0x3410);
	
	ClearText(0x3420);
	
	pageBuffer->tempBuf[0] = 0x00;
	pageBuffer->tempBuf[1] = 0x00;
	WriteRadioData(0x3402, pageBuffer->tempBuf, 2);
}


/****************************************end of file************************************************/
