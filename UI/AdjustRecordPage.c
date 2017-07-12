/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"AdjustRecordPage.h"

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
static AdjustRecordPageBuffer * adjustRecordPageBuffer = NULL;
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
MyRes createAdjustRecordActivity(Activity * thizActivity, Intent * pram)
{
	if(NULL == thizActivity)
		return My_Fail;
	
	if(My_Pass == activityBufferMalloc())
	{
		InitActivity(thizActivity, "AdjustRecordActivity\0", activityStart, activityInput, activityFresh, activityHide, activityResume, activityDestroy);
		
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
	adjustRecordPageBuffer->selectindex = 0;
	adjustRecordPageBuffer->pageindex = 1;
	ShowRecord(adjustRecordPageBuffer->pageindex);
	
	SelectPage(150);
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
	adjustRecordPageBuffer->lcdinput[0] = pbuf[4];
	adjustRecordPageBuffer->lcdinput[0] = (adjustRecordPageBuffer->lcdinput[0]<<8) + pbuf[5];
		
	/*返回*/
	if(adjustRecordPageBuffer->lcdinput[0] == 0x2b00)
	{
		backToFatherActivity();
	}
	/*上一页*/
	else if(adjustRecordPageBuffer->lcdinput[0] == 0x2b02)
	{
		if(adjustRecordPageBuffer->pageindex > 1)
			adjustRecordPageBuffer->pageindex -= 1;
		else
			adjustRecordPageBuffer->pageindex = adjustRecordPageBuffer->maxpagenum;
				
		ShowRecord(adjustRecordPageBuffer->pageindex);
	}
	/*下一页*/
	else if(adjustRecordPageBuffer->lcdinput[0] == 0x2b03)
	{
		if(adjustRecordPageBuffer->pageindex < adjustRecordPageBuffer->maxpagenum)
			adjustRecordPageBuffer->pageindex += 1;
		else
			adjustRecordPageBuffer->pageindex = 1;
				
		ShowRecord(adjustRecordPageBuffer->pageindex);
	}
	//选择数据
	else if((adjustRecordPageBuffer->lcdinput[0] >= 0x2b04)&&(adjustRecordPageBuffer->lcdinput[0] <= 0x2b0b))
	{
		adjustRecordPageBuffer->tempvalue1 = adjustRecordPageBuffer->lcdinput[0] - 0x2b04 + 1;
		
		if(adjustRecordPageBuffer->tempvalue1 <= adjustRecordPageBuffer->deviceAdjustReadPackge.readTotalNum)
		{
			adjustRecordPageBuffer->selectindex = adjustRecordPageBuffer->tempvalue1;
			BasicPic(0x2b20, 1, 137, 11, 191, 940, 228, 38, 149+(adjustRecordPageBuffer->selectindex - 1)*40);
		}
	}
	//跳页
	else if(adjustRecordPageBuffer->lcdinput[0] == 0x2b10)
	{
		adjustRecordPageBuffer->tempvalue1 = strtol((char *)(&pbuf[7]), NULL, 10);
		if( (adjustRecordPageBuffer->tempvalue1 > 0) && (adjustRecordPageBuffer->tempvalue1 <= adjustRecordPageBuffer->maxpagenum))
		{
			adjustRecordPageBuffer->pageindex = adjustRecordPageBuffer->tempvalue1;
		
			adjustRecordPageBuffer->selectindex = 0;

			ShowRecord(adjustRecordPageBuffer->pageindex);
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
	SelectPage(150);
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
	if(NULL == adjustRecordPageBuffer)
	{
		adjustRecordPageBuffer = MyMalloc(sizeof(AdjustRecordPageBuffer));
		
		if(adjustRecordPageBuffer)
		{
			memset(adjustRecordPageBuffer, 0, sizeof(AdjustRecordPageBuffer));
	
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
	MyFree(adjustRecordPageBuffer);
	adjustRecordPageBuffer = NULL;
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
	adjustRecordPageBuffer->tempvalue1 = pageindex-1;
	adjustRecordPageBuffer->tempvalue1 *= DeviceAdjustRecordPageShowNum;
	adjustRecordPageBuffer->deviceAdjustReadPackge.pageRequest.startElementIndex = adjustRecordPageBuffer->tempvalue1;
	adjustRecordPageBuffer->deviceAdjustReadPackge.pageRequest.pageSize = DeviceAdjustRecordPageShowNum;
	adjustRecordPageBuffer->deviceAdjustReadPackge.pageRequest.orderType = ASC;
	adjustRecordPageBuffer->deviceAdjustReadPackge.pageRequest.crc = CalModbusCRC16Fun(&adjustRecordPageBuffer->deviceAdjustReadPackge.pageRequest, PageRequestStructCrcSize, NULL);
		
	//读取数据
	readDeviceAdjustFromFile(&(adjustRecordPageBuffer->deviceAdjustReadPackge));
	
	adjustRecordPageBuffer->maxpagenum = ((adjustRecordPageBuffer->deviceAdjustReadPackge.deviceRecordHeader.itemSize % DeviceAdjustRecordPageShowNum) == 0)
		? (adjustRecordPageBuffer->deviceAdjustReadPackge.deviceRecordHeader.itemSize / DeviceAdjustRecordPageShowNum)
		: ((adjustRecordPageBuffer->deviceAdjustReadPackge.deviceRecordHeader.itemSize / DeviceAdjustRecordPageShowNum)+1);
		
	BasicPic(0x2b20, 0, 100, 39, 522, 968, 556, 39, 140+(adjustRecordPageBuffer->selectindex)*36);
	
	vTaskDelay(10 / portTICK_RATE_MS);

	adjustRecordPageBuffer->tempDeviceAdjust = &(adjustRecordPageBuffer->deviceAdjustReadPackge.deviceAdjust[adjustRecordPageBuffer->deviceAdjustReadPackge.readTotalNum - 1]);
	for(i=0; i<adjustRecordPageBuffer->deviceAdjustReadPackge.readTotalNum; i++)
	{
		//显示索引
		snprintf(adjustRecordPageBuffer->buf, 20, "%d", (pageindex-1)*DeviceAdjustRecordPageShowNum+i+1);
		DisText(0x2b30+(i)*0x20, adjustRecordPageBuffer->buf, strlen(adjustRecordPageBuffer->buf)+1);
			
		//显示测试值
		snprintf(adjustRecordPageBuffer->buf, 10, "%.2f", adjustRecordPageBuffer->tempDeviceAdjust->measurev);
		DisText(0x2b35+(i)*0x20, adjustRecordPageBuffer->buf, strlen(adjustRecordPageBuffer->buf)+1);
		
		//显示标准值
		snprintf(adjustRecordPageBuffer->buf, 10, "%.2f", adjustRecordPageBuffer->tempDeviceAdjust->normalv);
		DisText(0x2b3a+(i)*0x20, adjustRecordPageBuffer->buf, strlen(adjustRecordPageBuffer->buf)+1);
			
		//显示异常时间
		snprintf(adjustRecordPageBuffer->buf, 30, "%02d-%02d-%02d %02d:%02d", adjustRecordPageBuffer->tempDeviceAdjust->dateTime.year, 
			adjustRecordPageBuffer->tempDeviceAdjust->dateTime.month, adjustRecordPageBuffer->tempDeviceAdjust->dateTime.day,
			adjustRecordPageBuffer->tempDeviceAdjust->dateTime.hour, adjustRecordPageBuffer->tempDeviceAdjust->dateTime.min);
		DisText(0x2b40+(i)*0x20, adjustRecordPageBuffer->buf, strlen(adjustRecordPageBuffer->buf)+1);

		//显示操作人
		snprintf(adjustRecordPageBuffer->buf, 30, "%s", adjustRecordPageBuffer->tempDeviceAdjust->operator.name);
		DisText(0x2b4a+(i)*0x20, adjustRecordPageBuffer->buf, strlen(adjustRecordPageBuffer->buf)+1);

		adjustRecordPageBuffer->tempDeviceAdjust--;
		vTaskDelay(10 / portTICK_RATE_MS);
	}

	for(i=adjustRecordPageBuffer->deviceAdjustReadPackge.readTotalNum; i<DeviceAdjustRecordPageShowNum; i++)
	{
		ClearText(0x2b30+(i)*0x20);
		ClearText(0x2b35+(i)*0x20);
		ClearText(0x2b3a+(i)*0x20);
		ClearText(0x2b40+(i)*0x20);
		ClearText(0x2b4a+(i)*0x20);
		
		vTaskDelay(10 / portTICK_RATE_MS);
	}
	
	return My_Pass;
}

