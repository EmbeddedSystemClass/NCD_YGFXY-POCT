/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"UserMPage.h"
#include	"LCD_Driver.h"
#include	"MyMem.h"
#include	"CRC16.h"
#include	"SystemSetPage.h"
#include	"DeviceDao.h"
#include	"MyTools.h"
#include	"SleepPage.h"

#include 	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"

#include	<string.h>
#include	"stdio.h"

/******************************************************************************************/
/*****************************************局部变量声明*************************************/
static UserMPageBuffer * S_UserMPageBuffer = NULL;

/******************************************************************************************/
/*****************************************局部函数声明*************************************/
static void activityStart(void);
static void activityInput(unsigned char *pbuf , unsigned short len);
static void activityFresh(void);
static void activityHide(void);
static void activityResume(void);
static void activityDestroy(void);
static MyState_TypeDef activityBufferMalloc(void);
static void activityBufferFree(void);

static void ShowList(void);
static void ShowDetail(void);
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
MyState_TypeDef createUserManagerActivity(Activity * thizActivity, Intent * pram)
{
	if(NULL == thizActivity)
		return My_Fail;
	
	if(My_Pass == activityBufferMalloc())
	{
		InitActivity(thizActivity, "UserManagerActivity\0", activityStart, activityInput, activityFresh, activityHide, activityResume, activityDestroy);
		
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
	if(S_UserMPageBuffer)
	{
		/*读取所有操作人*/
		ReadDeviceFromFile(&(S_UserMPageBuffer->device));
		
		S_UserMPageBuffer->pageindex = 1;
		S_UserMPageBuffer->selectindex = 0;
		
		ShowList();
		ShowDetail();
	}
	SelectPage(106);
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
	if(S_UserMPageBuffer)
	{
		/*命令*/
		S_UserMPageBuffer->lcdinput[0] = pbuf[4];
		S_UserMPageBuffer->lcdinput[0] = (S_UserMPageBuffer->lcdinput[0]<<8) + pbuf[5];
		
		/*返回*/
		if(S_UserMPageBuffer->lcdinput[0] == 0x1d00)
		{
			backToFatherActivity();
		}
		
		/*上翻也*/
		else if(S_UserMPageBuffer->lcdinput[0] == 0x1d03)
		{
			if(S_UserMPageBuffer->pageindex > 1)
			{
				S_UserMPageBuffer->pageindex--;
					
				S_UserMPageBuffer->selectindex = 0;
					
				ShowList();
				ShowDetail();
			}
		}
		/*下翻页*/
		else if(S_UserMPageBuffer->lcdinput[0] == 0x1d04)
		{
			if(S_UserMPageBuffer->pageindex < (MaxOperatorSize / MaxPageShowOperatorSize))
			{
				S_UserMPageBuffer->tempuser = &S_UserMPageBuffer->device.operators[(S_UserMPageBuffer->pageindex)*MaxPageShowOperatorSize];
			
				if(S_UserMPageBuffer->tempuser->crc == CalModbusCRC16Fun1(S_UserMPageBuffer->tempuser, OneOperatorStructSizeWithOutCrc))
				{
					S_UserMPageBuffer->pageindex++;
					
					S_UserMPageBuffer->selectindex = 0;

					ShowList();
					ShowDetail();
				}
			}
		}
		/*选择操作人*/
		else if((S_UserMPageBuffer->lcdinput[0] >= 0x1d07)&&(S_UserMPageBuffer->lcdinput[0] <= 0x1d0B))
		{
			S_UserMPageBuffer->tempuser = &S_UserMPageBuffer->device.operators[(S_UserMPageBuffer->pageindex - 1)*MaxPageShowOperatorSize + S_UserMPageBuffer->lcdinput[0] - 0x1d07];
			
			if(S_UserMPageBuffer->tempuser->crc == CalModbusCRC16Fun1(S_UserMPageBuffer->tempuser, OneOperatorStructSizeWithOutCrc))
			{
				S_UserMPageBuffer->selectindex = S_UserMPageBuffer->lcdinput[0] - 0x1d07+1;
				ShowDetail();
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
	if(S_UserMPageBuffer)
	{

	}
	
	SelectPage(106);
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
	if(NULL == S_UserMPageBuffer)
	{
		S_UserMPageBuffer = MyMalloc(sizeof(UserMPageBuffer));
		
		if(S_UserMPageBuffer)
		{
			memset(S_UserMPageBuffer, 0, sizeof(UserMPageBuffer));
	
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
	MyFree(S_UserMPageBuffer);
	S_UserMPageBuffer = NULL;
}

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
static void ShowList(void)
{
	unsigned char i = 0;
	
	i = (S_UserMPageBuffer->pageindex-1)*MaxPageShowOperatorSize;
	
	S_UserMPageBuffer->tempuser = &(S_UserMPageBuffer->device.operators[i]);
	
	/*显示列表数据*/
	for(i=0; i<MaxPageShowOperatorSize; i++)
	{		
		if(S_UserMPageBuffer->tempuser->crc == CalModbusCRC16Fun1(S_UserMPageBuffer->tempuser, OneOperatorStructSizeWithOutCrc))
			sprintf(S_UserMPageBuffer->buf, "%s\0", S_UserMPageBuffer->tempuser->name);
		else
			sprintf(S_UserMPageBuffer->buf, "\0");
		
		DisText(0x1d10+8*i, S_UserMPageBuffer->buf, strlen(S_UserMPageBuffer->buf)+1);
		
		S_UserMPageBuffer->tempuser++;
	}
}

static void ShowDetail(void)
{
	unsigned char i = 0;
	
	if((S_UserMPageBuffer->selectindex > 0) && (S_UserMPageBuffer->selectindex <= MaxPageShowOperatorSize))
	{
		i = (S_UserMPageBuffer->pageindex-1)*MaxPageShowOperatorSize + S_UserMPageBuffer->selectindex-1;
	
		memcpy(&(S_UserMPageBuffer->tempnewuser), &(S_UserMPageBuffer->device.operators[i]), OneOperatorStructSize);
	}
	else
		memset(&(S_UserMPageBuffer->tempnewuser), 0, OneOperatorStructSize);
	
	if(S_UserMPageBuffer->tempnewuser.crc == CalModbusCRC16Fun1(&(S_UserMPageBuffer->tempnewuser), OneOperatorStructSizeWithOutCrc))
	{	
		BasicPic(0x1d40, 1, 137, 11, 10, 303, 79, 157, 135+(S_UserMPageBuffer->selectindex-1)*72);	
	}
	else
		BasicPic(0x1d40, 0, 137, 266, 215, 559, 284, 157, 135+(S_UserMPageBuffer->selectindex-1)*72);
	
	sprintf(S_UserMPageBuffer->buf, "%s\0", S_UserMPageBuffer->tempnewuser.name);
	DisText(0x1d50, S_UserMPageBuffer->buf, strlen(S_UserMPageBuffer->buf)+1);
		
	sprintf(S_UserMPageBuffer->buf, "%s\0", S_UserMPageBuffer->tempnewuser.age);
	DisText(0x1d60, S_UserMPageBuffer->buf, strlen(S_UserMPageBuffer->buf)+1);
		
	sprintf(S_UserMPageBuffer->buf, "%s\0", S_UserMPageBuffer->tempnewuser.sex);
	DisText(0x1d70, S_UserMPageBuffer->buf, strlen(S_UserMPageBuffer->buf)+1);
		
	sprintf(S_UserMPageBuffer->buf, "%s\0", S_UserMPageBuffer->tempnewuser.phone);
	DisText(0x1d80, S_UserMPageBuffer->buf, strlen(S_UserMPageBuffer->buf)+1);
		
	sprintf(S_UserMPageBuffer->buf, "%s\0", S_UserMPageBuffer->tempnewuser.job);
	DisText(0x1d90, S_UserMPageBuffer->buf, strlen(S_UserMPageBuffer->buf)+1);
		
	sprintf(S_UserMPageBuffer->buf, "%s\0", S_UserMPageBuffer->tempnewuser.department);
	DisText(0x1da0, S_UserMPageBuffer->buf, strlen(S_UserMPageBuffer->buf)+1);
}


