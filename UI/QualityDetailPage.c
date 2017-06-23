/***************************************************************************************************
*FileName: MaintenancePage
*Description: 设备维护
*Author:xsx
*Data: 2017年6月14日 15:51:52
***************************************************************************************************/

/***************************************************************************************************/
/******************************************头文件***************************************************/
/***************************************************************************************************/
#include	"QualityDetailPage.h"

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
static QualityDetailPageBuffer * pageBuffer = NULL;
/***************************************************************************************************/
/**************************************局部函数声明*************************************************/
/***************************************************************************************************/
static void activityStart(void);
static void activityInput(unsigned char *pbuf , unsigned short len);
static void activityFresh(void);
static void activityHide(void);
static void activityResume(void);
static void activityDestroy(void);

static MyState_TypeDef activityBufferMalloc(void);
static void activityBufferFree(void);
static void showPageValue(void);
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
MyState_TypeDef createQualityDetailActivity(Activity * thizActivity, Intent * pram)
{
	if(NULL == thizActivity)
		return My_Fail;
	
	if(My_Pass == activityBufferMalloc())
	{
		InitActivity(thizActivity, QualityDetailRecordActivityName, activityStart, activityInput, activityFresh, activityHide, activityResume, activityDestroy);
		
		if(pram != NULL)
			readIntent(pram, &(pageBuffer->deviceQuality), DeviceQualityStructSize);
		
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
	showPageValue();
	
	SelectPage(159);
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
		backToFatherActivity();
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
	SelectPage(159);
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
static MyState_TypeDef activityBufferMalloc(void)
{
	if(NULL == pageBuffer)
	{
		pageBuffer = MyMalloc(QualityDetailPageBufferStructSize);
		if(pageBuffer)	
		{
			memset(pageBuffer, 0, QualityDetailPageBufferStructSize);
	
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

static void showPageValue(void)
{
	unsigned char i=0;
	pageBuffer->resultSum = 0.0;
	pageBuffer->tempValue3 = 0;
	
	//显示标准值
	snprintf(pageBuffer->tempBuf, 15, "%.2f", pageBuffer->deviceQuality.standardValue);
	DisText(0x3410, pageBuffer->tempBuf, strlen(pageBuffer->tempBuf)+1);
	
	//显示每个测试值和对应偏差
	for(i=0; i<DeviceQualityMaxTestCount; i++)
	{
		//有数据则显示
		if(pageBuffer->deviceQuality.testValue[i] != 0)
		{
			snprintf(pageBuffer->tempBuf, 15, "%.2f", pageBuffer->deviceQuality.testValue[i]);
			DisText(0x3430+(i*0x05), pageBuffer->tempBuf, strlen(pageBuffer->tempBuf)+1);
			
			pageBuffer->tempValue1 = (pageBuffer->deviceQuality.testValue[i] - pageBuffer->deviceQuality.standardValue) 
				/ pageBuffer->deviceQuality.standardValue;
				
			pageBuffer->tempValue1 *= 100;
				
			snprintf(pageBuffer->tempBuf, 15, "%.2f%%", pageBuffer->tempValue1);
			DisText(0x3465+(i*0x05), pageBuffer->tempBuf, strlen(pageBuffer->tempBuf)+1);
			
			pageBuffer->resultSum += pageBuffer->deviceQuality.testValue[i];
			pageBuffer->tempValue3++;
		}
		//无数据则清除
		else
		{
			ClearText(0x3430+(i*0x05));
			ClearText(0x3465+(i*0x05));
		}
	}
		
	//更新平均偏差率
	if(i > 0)
	{
		pageBuffer->tempValue2 = pageBuffer->resultSum / pageBuffer->tempValue3;
		pageBuffer->tempValue2 = (pageBuffer->tempValue2 - pageBuffer->deviceQuality.standardValue)/pageBuffer->deviceQuality.standardValue;
			
		pageBuffer->tempValue1 = pageBuffer->tempValue2*100;
				
		snprintf(pageBuffer->tempBuf, 15, "%.2f%%", pageBuffer->tempValue1);
		DisText(0x349a, pageBuffer->tempBuf, strlen(pageBuffer->tempBuf)+1);
	}
	
	//显示结果
	pageBuffer->tempBuf[0] = 0x00;
	pageBuffer->tempBuf[1] = 0x00;
	if(pageBuffer->deviceQuality.isOk)
	{
		pageBuffer->tempBuf[1] = 0x01;
	}
	WriteRadioData(0x3402, pageBuffer->tempBuf, 2);
	
	snprintf(pageBuffer->tempBuf, 40, "%s", pageBuffer->deviceQuality.desc);
	DisText(0x3420, pageBuffer->tempBuf, strlen(pageBuffer->tempBuf)+1);
}



/****************************************end of file************************************************/
