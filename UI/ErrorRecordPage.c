/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"ErrorRecordPage.h"

#include	"LCD_Driver.h"

#include	"Define.h"
#include	"MyMem.h"

#include	"CRC16.h"
#include	"System_Data.h"
#include	"SleepPage.h"

#include 	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"

#include	<string.h>
#include	"stdio.h"
#include 	"stdlib.h"

/******************************************************************************************/
/*****************************************局部变量声明*************************************/
static ErrorRecordPageBuffer * S_ErrorRecordPageBuffer = NULL;
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

static MyRes ShowRecord(unsigned char pageindex);
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
MyRes createErrorRecordActivity(Activity * thizActivity, Intent * pram)
{
	if(NULL == thizActivity)
		return My_Fail;
	
	if(My_Pass == activityBufferMalloc())
	{
		InitActivity(thizActivity, "ErrorRecordActivity\0", activityStart, activityInput, activityFresh, activityHide, activityResume, activityDestroy);
		
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
	S_ErrorRecordPageBuffer->selectindex = 0;
	S_ErrorRecordPageBuffer->pageindex = 1;
	ShowRecord(S_ErrorRecordPageBuffer->pageindex);
	
	SelectPage(152);
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
	S_ErrorRecordPageBuffer->lcdinput[0] = pbuf[4];
	S_ErrorRecordPageBuffer->lcdinput[0] = (S_ErrorRecordPageBuffer->lcdinput[0]<<8) + pbuf[5];
		
	/*返回*/
	if(S_ErrorRecordPageBuffer->lcdinput[0] == 0x2d00)
	{
		backToFatherActivity();
	}
	/*上一页*/
	else if(S_ErrorRecordPageBuffer->lcdinput[0] == 0x2d02)
	{
		if(S_ErrorRecordPageBuffer->pageindex > 1)
			S_ErrorRecordPageBuffer->pageindex -= 1;
		else
			S_ErrorRecordPageBuffer->pageindex = S_ErrorRecordPageBuffer->maxpagenum;
				
		ShowRecord(S_ErrorRecordPageBuffer->pageindex);
	}
	/*下一页*/
	else if(S_ErrorRecordPageBuffer->lcdinput[0] == 0x2d03)
	{
		if(S_ErrorRecordPageBuffer->pageindex < S_ErrorRecordPageBuffer->maxpagenum)
			S_ErrorRecordPageBuffer->pageindex += 1;
		else
			S_ErrorRecordPageBuffer->pageindex = 1;
				
		ShowRecord(S_ErrorRecordPageBuffer->pageindex);
	}
	//选择数据
	else if((S_ErrorRecordPageBuffer->lcdinput[0] >= 0x2d04)&&(S_ErrorRecordPageBuffer->lcdinput[0] <= 0x2d0b))
	{
		S_ErrorRecordPageBuffer->tempvalue1 = S_ErrorRecordPageBuffer->lcdinput[0] - 0x2d04 + 1;
		
		if(S_ErrorRecordPageBuffer->tempvalue1 <= S_ErrorRecordPageBuffer->deviceErrorReadPackge.readTotalNum)
		{
			S_ErrorRecordPageBuffer->selectindex = S_ErrorRecordPageBuffer->tempvalue1;
			BasicPic(0x2d20, 1, 137, 11, 90, 940, 127, 38, 149+(S_ErrorRecordPageBuffer->selectindex - 1)*40);
		}
	}
	//跳页
	else if(S_ErrorRecordPageBuffer->lcdinput[0] == 0x2d10)
	{
		S_ErrorRecordPageBuffer->tempvalue1 = strtol((char *)(&pbuf[7]), NULL, 10);
		if( (S_ErrorRecordPageBuffer->tempvalue1 > 0) && (S_ErrorRecordPageBuffer->tempvalue1 <= S_ErrorRecordPageBuffer->maxpagenum))
		{
			S_ErrorRecordPageBuffer->pageindex = S_ErrorRecordPageBuffer->tempvalue1;
		
			S_ErrorRecordPageBuffer->selectindex = 0;

			ShowRecord(S_ErrorRecordPageBuffer->pageindex);
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
	SelectPage(152);
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
	if(NULL == S_ErrorRecordPageBuffer)
	{
		S_ErrorRecordPageBuffer = MyMalloc(sizeof(ErrorRecordPageBuffer));
		
		if(S_ErrorRecordPageBuffer)
		{
			memset(S_ErrorRecordPageBuffer, 0, sizeof(ErrorRecordPageBuffer));
	
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
	MyFree(S_ErrorRecordPageBuffer);
	S_ErrorRecordPageBuffer = NULL;
}

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

static MyRes ShowRecord(unsigned char pageindex)
{
	unsigned short i=0;
		
	//设置分页读取信息
	S_ErrorRecordPageBuffer->tempvalue1 = pageindex-1;
	S_ErrorRecordPageBuffer->tempvalue1 *= DeviceErrorRecordPageShowNum;
	S_ErrorRecordPageBuffer->deviceErrorReadPackge.pageRequest.startElementIndex = S_ErrorRecordPageBuffer->tempvalue1;
	S_ErrorRecordPageBuffer->deviceErrorReadPackge.pageRequest.pageSize = DeviceErrorRecordPageShowNum;
	S_ErrorRecordPageBuffer->deviceErrorReadPackge.pageRequest.orderType = ASC;
	S_ErrorRecordPageBuffer->deviceErrorReadPackge.pageRequest.crc = CalModbusCRC16Fun(&S_ErrorRecordPageBuffer->deviceErrorReadPackge.pageRequest, PageRequestStructCrcSize, NULL);
		
	//读取数据
	readDeviceErrorFromFile(&(S_ErrorRecordPageBuffer->deviceErrorReadPackge));
	
	S_ErrorRecordPageBuffer->maxpagenum = ((S_ErrorRecordPageBuffer->deviceErrorReadPackge.deviceRecordHeader.itemSize % DeviceErrorRecordPageShowNum) == 0)
		? (S_ErrorRecordPageBuffer->deviceErrorReadPackge.deviceRecordHeader.itemSize / DeviceErrorRecordPageShowNum)
		: ((S_ErrorRecordPageBuffer->deviceErrorReadPackge.deviceRecordHeader.itemSize / DeviceErrorRecordPageShowNum)+1);
		
	BasicPic(0x2d20, 0, 100, 11, 90, 940, 127, 39, 140+(S_ErrorRecordPageBuffer->selectindex)*36);
	
	vTaskDelay(10 / portTICK_RATE_MS);

	S_ErrorRecordPageBuffer->tempDeviceError = &(S_ErrorRecordPageBuffer->deviceErrorReadPackge.deviceError[S_ErrorRecordPageBuffer->deviceErrorReadPackge.readTotalNum - 1]);
	for(i=0; i<S_ErrorRecordPageBuffer->deviceErrorReadPackge.readTotalNum; i++)
	{
		//显示索引
		snprintf(S_ErrorRecordPageBuffer->buf, 20, "%d", (pageindex-1)*DeviceErrorRecordPageShowNum+i+1);
		DisText(0x2d30+(i)*0x30, S_ErrorRecordPageBuffer->buf, strlen(S_ErrorRecordPageBuffer->buf)+1);
			
		//显示异常代码
		snprintf(S_ErrorRecordPageBuffer->buf, 10, "%04d", S_ErrorRecordPageBuffer->tempDeviceError->errorCode);
		DisText(0x2d35+(i)*0x30, S_ErrorRecordPageBuffer->buf, strlen(S_ErrorRecordPageBuffer->buf)+1);
			
		//显示异常时间
		snprintf(S_ErrorRecordPageBuffer->buf, 30, "%02d-%02d-%02d %02d:%02d", S_ErrorRecordPageBuffer->tempDeviceError->dateTime.year, 
			S_ErrorRecordPageBuffer->tempDeviceError->dateTime.month, S_ErrorRecordPageBuffer->tempDeviceError->dateTime.day,
			S_ErrorRecordPageBuffer->tempDeviceError->dateTime.hour, S_ErrorRecordPageBuffer->tempDeviceError->dateTime.min);
		DisText(0x2d3a+(i)*0x30, S_ErrorRecordPageBuffer->buf, strlen(S_ErrorRecordPageBuffer->buf)+1);
			
		//显示异常说明
		snprintf(S_ErrorRecordPageBuffer->buf, 40, "%s", S_ErrorRecordPageBuffer->tempDeviceError->result);
		DisText(0x2d45+(i)*0x30, S_ErrorRecordPageBuffer->buf, strlen(S_ErrorRecordPageBuffer->buf)+1);

		//显示操作人
		snprintf(S_ErrorRecordPageBuffer->buf, 30, "%s", S_ErrorRecordPageBuffer->tempDeviceError->operator.name);
		DisText(0x2d55+(i)*0x30, S_ErrorRecordPageBuffer->buf, strlen(S_ErrorRecordPageBuffer->buf)+1);

		S_ErrorRecordPageBuffer->tempDeviceError--;
		vTaskDelay(10 / portTICK_RATE_MS);
	}

	for(i=S_ErrorRecordPageBuffer->deviceErrorReadPackge.readTotalNum; i<DeviceErrorRecordPageShowNum; i++)
	{
		ClearText(0x2d30+(i)*0x30);
		ClearText(0x2d35+(i)*0x30);
		ClearText(0x2d3a+(i)*0x30);
		ClearText(0x2d45+(i)*0x30);
		ClearText(0x2d55+(i)*0x30);
		
		vTaskDelay(10 / portTICK_RATE_MS);
	}
	
	return My_Pass;
}

