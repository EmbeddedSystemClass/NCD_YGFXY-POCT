/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"LunchPage.h"

#include	"LCD_Driver.h"
#include	"Define.h"
#include	"MyMem.h"

#include	"RecordPage.h"
#include	"SystemSetPage.h"
#include	"SelectUserPage.h"
#include	"PaiDuiPage.h"
#include	"SampleIDPage.h"
#include	"SleepPage.h"
#include	"PlaySong_Task.h"

#include 	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"

#include	<string.h>
#include	"stdio.h"

/******************************************************************************************/
/*****************************************局部变量声明*************************************/
static LunchPageBuffer * page = NULL;
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

static void DspPageText(void);
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/

/***************************************************************************************************
*FunctionName: createLunchActivity
*Description: 创建主界面
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月21日09:00:09
***************************************************************************************************/
MyRes createLunchActivity(Activity * thizActivity, Intent * pram)
{
	if(NULL == thizActivity)
		return My_Fail;
	
	if(My_Pass == activityBufferMalloc())
	{
		InitActivity(thizActivity, lunchActivityName, activityStart, activityInput, activityFresh, activityHide, activityResume, activityDestroy);
		
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
	timer_set(&(page->timer), getGBSystemSetData()->ledSleepTime);
	
	DspPageText();
	
	SelectPage(82);

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
	
	//重置休眠时间
	timer_restart(&(page->timer));
		
	//设置
	if(page->lcdinput[0] == 0x1103)
	{
		startActivity(createSystemSetActivity, NULL, NULL);
	}
	//查看数据
	else if(page->lcdinput[0] == 0x1102)
	{	
		startActivity(createRecordActivity, NULL, NULL);
	}
	//常规测试
	else if(page->lcdinput[0] == 0x1100)
	{	
		page->error = CreateANewTest(NormalTestType);
		//创建成功
		if(Error_OK == page->error)
		{
			page->tempOperator = &(GetCurrentTestItem()->testData.operator);
			startActivity(createSelectUserActivity, createIntent(&(page->tempOperator), 4), createSampleActivity);
		}
		//禁止常规测试
		else if(Error_StopNormalTest == page->error)
		{
			SendKeyCode(1);
			AddNumOfSongToList(8, 0);
		}
		//创建失败
		else if(Error_Mem == page->error)
		{
			SendKeyCode(2);
			AddNumOfSongToList(7, 0);
		}
	}
	//批量测试
	else if(page->lcdinput[0] == 0x1101)
	{
		//有卡排队，则进入排队界面
		if(true == IsPaiDuiTestting())
		{
			startActivity(createPaiDuiActivity, NULL, NULL);
		}
		//无卡排队则开始创建
		else
		{
			page->error = CreateANewTest(PaiDuiTestType);
			//创建成功
			if(Error_OK == page->error)
			{
				page->tempOperator = &(GetCurrentTestItem()->testData.operator);
				startActivity(createSelectUserActivity, createIntent(&(page->tempOperator), 4), createSampleActivity);
			}
			//创建失败
			else if(Error_Mem == page->error)
			{
				SendKeyCode(2);
				AddNumOfSongToList(7, 0);
			}
			//排队模块失联
			else if(Error_PaiduiDisconnect == page->error)
			{
				SendKeyCode(3);
				AddNumOfSongToList(58, 0);
			}
		}
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
		startActivity(createSleepActivity, NULL, NULL);
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
	timer_restart(&(page->timer));
	
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
	if(NULL == page)
	{
		page = MyMalloc(sizeof(LunchPageBuffer));
		if(page)	
		{
			memset(page, 0, sizeof(LunchPageBuffer));
	
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
*FunctionName: DspPageText
*Description: 当前界面内容显示
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月21日09:03:42
***************************************************************************************************/
static void DspPageText(void)
{
	memset(page->buf, 0, 100);
	sprintf(page->buf, "V%d.%d.%02d", GB_SoftVersion/1000, GB_SoftVersion%1000/100, GB_SoftVersion%100);
	DisText(0x1110, page->buf, 30);
}

