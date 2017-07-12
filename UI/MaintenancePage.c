/***************************************************************************************************
*FileName: MaintenancePage
*Description: 设备维护
*Author:xsx
*Data: 2017年6月14日 15:51:52
***************************************************************************************************/

/***************************************************************************************************/
/******************************************头文件***************************************************/
/***************************************************************************************************/
#include	"MaintenancePage.h"

#include	"LCD_Driver.h"
#include	"Define.h"
#include	"MyMem.h"

#include	"Maintenance_Data.h"
#include	"SystemSetPage.h"
#include	"SelectUserPage.h"
#include	"System_Data.h"
#include	"DeviceMaintenanceDao.h"

#include	"CRC16.h"

#include	<string.h>
#include	"stdio.h"

/***************************************************************************************************/
/**************************************变量声明*************************************************/
/***************************************************************************************************/
static MaintenancePageBuffer * pageBuffer = NULL;
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
MyRes createMaintenanceActivity(Activity * thizActivity, Intent * pram)
{
	if(NULL == thizActivity)
		return My_Fail;
	
	if(My_Pass == activityBufferMalloc())
	{
		InitActivity(thizActivity, MaintenanceActivityName, activityStart, activityInput, activityFresh, activityHide, activityResume, activityDestroy);
		
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
	pageBuffer->deviceMaintenance = getGB_DeviceMaintenance();
	
	clearPageText();
	
	SelectPage(156);
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
	if(pageBuffer->lcdinput[0] == 0x3300)
	{
		deleteGB_DeviceMaintenance();
		backToActivity(SystemSetActivityName);
	}
	//提交
	else if(pageBuffer->lcdinput[0] == 0x3301)
	{
		memcpy(&(pageBuffer->deviceMaintenance->dateTime), &(getSystemRunTimeData()->systemDateTime), DateTimeStructSize);
		pageBuffer->deviceMaintenance->crc = CalModbusCRC16Fun(pageBuffer->deviceMaintenance, DeviceMaintenanceStructCrcSize, NULL);

		if(My_Pass == writeDeviceMaintenanceToFile(pageBuffer->deviceMaintenance))
		{
			deleteGB_DeviceMaintenance();
			backToActivity(SystemSetActivityName);
		}
		else
			SendKeyCode(2);
	}
	//结果
	else if(pageBuffer->lcdinput[0] == 0x3302)
	{
		/*数据*/
		pageBuffer->lcdinput[1] = pbuf[7];
		pageBuffer->lcdinput[1] = (pageBuffer->lcdinput[1]<<8) + pbuf[8];
			
		/*自动获取ip*/
		if(pageBuffer->lcdinput[1] == 0x8000)
			pageBuffer->deviceMaintenance->isOk = true;
		/*使用设置的ip*/
		else if(pageBuffer->lcdinput[1] == 0x0000)
			pageBuffer->deviceMaintenance->isOk = false;
	}
	//维护说明
	else if(pageBuffer->lcdinput[0] == 0x3310)
	{
		snprintf(pageBuffer->deviceMaintenance->desc, GetBufLen(&pbuf[7], 2*pbuf[6])+1, "%s", &pbuf[7]);
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
	SelectPage(82);
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
		pageBuffer = MyMalloc(MaintenancePageBufferStructSize);
		if(pageBuffer)	
		{
			memset(pageBuffer, 0, MaintenancePageBufferStructSize);
	
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

static void clearPageText(void)
{
	ClearText(0x3310);
	
	pageBuffer->tempBuf[0] = 0x00;
	pageBuffer->tempBuf[1] = 0x00;
	WriteRadioData(0x3302, pageBuffer->tempBuf, 2);
}

/****************************************end of file************************************************/
