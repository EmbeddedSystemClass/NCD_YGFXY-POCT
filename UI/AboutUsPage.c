/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"AboutUsPage.h"
#include	"LCD_Driver.h"
#include	"UI_Data.h"
#include	"MyMem.h"
#include	"RemoteSoft_Data.h"
#include	"SystemSet_Data.h"

#include	<string.h>
#include	"stdio.h"

/******************************************************************************************/
/*****************************************局部变量声明*************************************/
static AboutUsPageBuffer * page = NULL;
/******************************************************************************************/
/*****************************************局部函数声明*************************************/
static void activityStart(void);
static void activityInput(unsigned char *pbuf , unsigned short len);
static void activityFresh(void);
static void activityHide(void);
static void activityResume(void);
static void activityDestroy(void);
static MyRes activityBufferMalloc(void);
static void activityBufferFree(void);

static void dspPageText(void);
static void freshRemoteFirmwareVersion(void);
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/

/***************************************************************************************************
*FunctionName: createSelectUserActivity
*Description: 创建选择操作人界面
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月21日09:00:09
***************************************************************************************************/
MyRes createAboutUsActivity(Activity * thizActivity, Intent * pram)
{
	if(NULL == thizActivity)
		return My_Fail;
	
	if(My_Pass == activityBufferMalloc())
	{
		InitActivity(thizActivity, "AboutUsActivity\0", activityStart, activityInput, activityFresh, activityHide, activityResume, activityDestroy);
		
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
	timer_set(&(page->timer), 10);
	
	dspPageText();
		
	freshRemoteFirmwareVersion();
		
	SelectPage(116);
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
	page->lcdinput[0] = pbuf[4];
	page->lcdinput[0] = (page->lcdinput[0]<<8) + pbuf[5];
		
	//返回
	if(page->lcdinput[0] == 0x2900)
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
	if(TimeOut == timer_expired(&(page->timer)))
	{
		freshRemoteFirmwareVersion();
		timer_restart(&(page->timer));
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
	dspPageText();
	
	freshRemoteFirmwareVersion();
	
	timer_restart(&(page->timer));
	
	SelectPage(116);
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
	if(NULL == page)
	{
		page = MyMalloc(sizeof(AboutUsPageBuffer));
		
		if(page)
		{
			memset(page, 0, sizeof(AboutUsPageBuffer));
	
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
	MyFree(page);
	page = NULL;
}

/***************************************************************************************************
*FunctionName:  dspPageText
*Description:  显示界面内容
*Input:  
*Output:  
*Return:  
*Author:  xsx
*Date: 2017年7月6日 13:56:23
***************************************************************************************************/
static void dspPageText(void)
{
	snprintf(page->buf, 20, "%s", GB_SoftVersion_Build);
	DisText(0x2920, page->buf, strlen(page->buf)+1);
	
	//设置二维码x,y,像素点大小
	page->buf[0] = 0;
	page->buf[1] = 180;
	page->buf[2] = 0x01;
	page->buf[3] = 0x42;
	page->buf[4] = 0;
	page->buf[5] = 0x02;
	writeDataToLcd(0x2941, page->buf, 6);

	//显示二维码
	snprintf(page->buf, 50, "http://www.whnewcando.com/?%s", getGBSystemSetData()->deviceId);
	writeDataToLcd(0x2950, page->buf, strlen(page->buf)+1);
}

/***************************************************************************************************
*FunctionName:  freshRemoteFirmwareVersion
*Description:  刷新远程软件版本
*Input:  
*Output:  
*Return:  
*Author:  xsx
*Date: 2017年7月6日 14:01:36
***************************************************************************************************/
static void freshRemoteFirmwareVersion(void)
{
	if((getIsSuccessDownloadFirmware() == true) && (getGbRemoteFirmwareVersion() > GB_SoftVersion))
	{
		page->tempV = getGbRemoteFirmwareVersion();
		snprintf(page->buf, 50, "V%d.%d.%02d (新版本V%d.%d.%02d)", GB_SoftVersion/1000, GB_SoftVersion%1000/100, GB_SoftVersion%100,
			page->tempV/1000, page->tempV%1000/100, page->tempV%100);
	}
	else
		snprintf(page->buf, 50, "V%d.%d.%02d", GB_SoftVersion/1000, GB_SoftVersion%1000/100, GB_SoftVersion%100);
	DisText(0x2910, page->buf, strlen(page->buf)+1);
}
