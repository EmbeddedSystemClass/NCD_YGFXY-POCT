/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"ServerSetPage.h"
#include	"LCD_Driver.h"
#include	"SystemSet_Dao.h"
#include	"NetPreSetPage.h"
#include	"MyMem.h"
#include	"CRC16.h"
#include	"SleepPage.h"

#include 	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"

#include	<string.h>
#include	"stdio.h"
#include 	"stdlib.h"

/******************************************************************************************/
/*****************************************局部变量声明*************************************/
static ServerSetPageBuffer * S_ServerSetPageBuffer = NULL;
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

static void SetServerIP(unsigned char *buf, unsigned char len);
static void SetServerPort(unsigned char *buf, unsigned char len);
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
MyState_TypeDef createServerSetActivity(Activity * thizActivity, Intent * pram)
{
	if(NULL == thizActivity)
		return My_Fail;
	
	if(My_Pass == activityBufferMalloc())
	{
		InitActivity(thizActivity, "ServerSetActivity\0", activityStart, activityInput, activityFresh, activityHide, activityResume, activityDestroy);
		
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
	if(S_ServerSetPageBuffer)
	{
		memcpy(&(S_ServerSetPageBuffer->serverSet), &(getGBSystemSetData()->serverSet), ServerSetStructSize);
		
		/*更新ip*/
		snprintf((S_ServerSetPageBuffer->buf), 16, "%d.%d.%d.%d", S_ServerSetPageBuffer->serverSet.serverIP.ip_1, S_ServerSetPageBuffer->serverSet.serverIP.ip_2, 
			S_ServerSetPageBuffer->serverSet.serverIP.ip_3, S_ServerSetPageBuffer->serverSet.serverIP.ip_4);
		DisText(0x1fb0, S_ServerSetPageBuffer->buf, strlen(S_ServerSetPageBuffer->buf)+1);
		
		/*更新端口号*/
		snprintf((S_ServerSetPageBuffer->buf), 5, "%d", S_ServerSetPageBuffer->serverSet.serverPort);
		DisText(0x1fc0, S_ServerSetPageBuffer->buf, strlen(S_ServerSetPageBuffer->buf)+1);
	}
	
	SelectPage(148);
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
	S_ServerSetPageBuffer->lcdinput[0] = pbuf[4];
	S_ServerSetPageBuffer->lcdinput[0] = (S_ServerSetPageBuffer->lcdinput[0]<<8) + pbuf[5];

	/*设置IP*/
	if(S_ServerSetPageBuffer->lcdinput[0] == 0x1FB0)
	{
		SetServerIP(&pbuf[7], GetBufLen(&pbuf[7] , 2*pbuf[6]));
		S_ServerSetPageBuffer->ischanged = 1;
	}
	/*设置端口号*/
	else if(S_ServerSetPageBuffer->lcdinput[0] == 0x1FC0)
	{
		SetServerPort(&pbuf[7], GetBufLen(&pbuf[7] , 2*pbuf[6]));
		S_ServerSetPageBuffer->ischanged = 1;
	}
	/*确认修改*/
	else if(S_ServerSetPageBuffer->lcdinput[0] == 0x1FA2)
	{
		if(1 == S_ServerSetPageBuffer->ischanged)
		{
			memcpy(&(S_ServerSetPageBuffer->systemSetData), getGBSystemSetData(), SystemSetDataStructSize);
		
			memcpy(&(S_ServerSetPageBuffer->systemSetData.serverSet), &(S_ServerSetPageBuffer->serverSet), ServerSetStructSize);
				
			if(My_Pass == SaveSystemSetData(&(S_ServerSetPageBuffer->systemSetData)))
			{
				SendKeyCode(1);

				S_ServerSetPageBuffer->ischanged = 0;
			}
			else
				SendKeyCode(2);
		}
	}
	/*返回*/
	else if(S_ServerSetPageBuffer->lcdinput[0] == 0x1FA1)
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

	SelectPage(148);
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
	if(NULL == S_ServerSetPageBuffer)
	{
		S_ServerSetPageBuffer = MyMalloc(sizeof(ServerSetPageBuffer));
		
		if(S_ServerSetPageBuffer)
		{
			memset(S_ServerSetPageBuffer, 0, sizeof(ServerSetPageBuffer));
	
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
	MyFree(S_ServerSetPageBuffer);
	S_ServerSetPageBuffer = NULL;
}

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

static void SetServerIP(unsigned char *buf, unsigned char len)
{
	S_ServerSetPageBuffer->tempBuf = strtok((char *)buf, ".");
	if(S_ServerSetPageBuffer->tempBuf)
	{
		S_ServerSetPageBuffer->tempValue = strtol(S_ServerSetPageBuffer->tempBuf, NULL, 10);
		if(S_ServerSetPageBuffer->tempValue > 255)
		{
			SendKeyCode(3);
			goto END;
		}
		
		S_ServerSetPageBuffer->serverSet.serverIP.ip_1 = S_ServerSetPageBuffer->tempValue;
	}
	else
	{
		SendKeyCode(3);
		goto END;
	}
	
	S_ServerSetPageBuffer->tempBuf = strtok(NULL, ".");
	if(S_ServerSetPageBuffer->tempBuf)
	{
		S_ServerSetPageBuffer->tempValue = strtol(S_ServerSetPageBuffer->tempBuf, NULL, 10);
		if(S_ServerSetPageBuffer->tempValue > 255)
		{
			SendKeyCode(3);
			goto END;
		}
		
		S_ServerSetPageBuffer->serverSet.serverIP.ip_2 = S_ServerSetPageBuffer->tempValue;
	}
	else
	{
		SendKeyCode(3);
		goto END;
	}
	
	S_ServerSetPageBuffer->tempBuf = strtok(NULL, ".");
	if(S_ServerSetPageBuffer->tempBuf)
	{
		S_ServerSetPageBuffer->tempValue = strtol(S_ServerSetPageBuffer->tempBuf, NULL, 10);
		if(S_ServerSetPageBuffer->tempValue > 255)
		{
			SendKeyCode(3);
			goto END;
		}
		
		S_ServerSetPageBuffer->serverSet.serverIP.ip_3 = S_ServerSetPageBuffer->tempValue;
	}
	else
	{
		SendKeyCode(3);
		goto END;
	}
	
	S_ServerSetPageBuffer->tempBuf = strtok(NULL, ".");
	if(S_ServerSetPageBuffer->tempBuf)
	{
		S_ServerSetPageBuffer->tempValue = strtol(S_ServerSetPageBuffer->tempBuf, NULL, 10);
		if(S_ServerSetPageBuffer->tempValue > 255)
		{
			SendKeyCode(3);
			goto END;
		}
		
		S_ServerSetPageBuffer->serverSet.serverIP.ip_4 = S_ServerSetPageBuffer->tempValue;
	}
	else
	{
		SendKeyCode(3);
		goto END;
	}
	
	END:
		snprintf((S_ServerSetPageBuffer->buf), 16, "%d.%d.%d.%d", S_ServerSetPageBuffer->serverSet.serverIP.ip_1, S_ServerSetPageBuffer->serverSet.serverIP.ip_2, 
			S_ServerSetPageBuffer->serverSet.serverIP.ip_3, S_ServerSetPageBuffer->serverSet.serverIP.ip_4);
		DisText(0x1fb0, S_ServerSetPageBuffer->buf, strlen(S_ServerSetPageBuffer->buf)+1);
}

static void SetServerPort(unsigned char *buf, unsigned char len)
{
	S_ServerSetPageBuffer->tempValue = strtol((char *)buf, NULL, 10);
	if(S_ServerSetPageBuffer->tempValue > 65535)
	{
		SendKeyCode(3);
	}
	else
		S_ServerSetPageBuffer->serverSet.serverPort = S_ServerSetPageBuffer->tempValue;
	
	snprintf((S_ServerSetPageBuffer->buf), 5, "%d", S_ServerSetPageBuffer->serverSet.serverPort);
	DisText(0x1fc0, S_ServerSetPageBuffer->buf, strlen(S_ServerSetPageBuffer->buf)+1);
}
