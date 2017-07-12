/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"QualityRecordPage.h"

#include	"LCD_Driver.h"
#include	"DeviceQualityDao.h"
#include	"QualityDetailPage.h"

#include	"Define.h"
#include	"MyMem.h"
#include	"CRC16.h"

#include 	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"

#include	<string.h>
#include	"stdio.h"
#include 	"stdlib.h"

/******************************************************************************************/
/*****************************************局部变量声明*************************************/
static QualityRecordPageBuffer * pageBuffer = NULL;
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
MyRes createQualityRecordActivity(Activity * thizActivity, Intent * pram)
{
	if(NULL == thizActivity)
		return My_Fail;
	
	if(My_Pass == activityBufferMalloc())
	{
		InitActivity(thizActivity, QualityRecordActivityName, activityStart, activityInput, activityFresh, activityHide, activityResume, activityDestroy);
		
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
	pageBuffer->selectindex = 0;
	pageBuffer->pageindex = 1;
	ShowRecord(pageBuffer->pageindex);
	
	SelectPage(154);
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
		
	/*返回*/
	if(pageBuffer->lcdinput[0] == 0x2f00)
	{
		backToFatherActivity();
	}
	/*上一页*/
	else if(pageBuffer->lcdinput[0] == 0x2f02)
	{
		if(pageBuffer->pageindex > 1)
			pageBuffer->pageindex -= 1;
		else
			pageBuffer->pageindex = pageBuffer->maxpagenum;
				
		ShowRecord(pageBuffer->pageindex);
	}
	/*下一页*/
	else if(pageBuffer->lcdinput[0] == 0x2f03)
	{
		if(pageBuffer->pageindex < pageBuffer->maxpagenum)
			pageBuffer->pageindex += 1;
		else
			pageBuffer->pageindex = 1;
				
		ShowRecord(pageBuffer->pageindex);
	}
	//选择数据
	else if((pageBuffer->lcdinput[0] >= 0x2f04)&&(pageBuffer->lcdinput[0] <= 0x2f0b))
	{
		pageBuffer->tempvalue1 = pageBuffer->lcdinput[0] - 0x2f04 + 1;
		
		if(pageBuffer->tempvalue1 <= pageBuffer->deviceQualityReadPackge.readTotalNum)
		{
			pageBuffer->selectindex = pageBuffer->tempvalue1;
			BasicPic(0x2f20, 1, 137, 11, 355, 940, 392, 38, 149+(pageBuffer->selectindex - 1)*40);
			startActivity(createQualityDetailActivity, createIntent(&(pageBuffer->deviceQualityReadPackge.deviceQuality[pageBuffer->deviceQualityReadPackge.readTotalNum - pageBuffer->selectindex]), DeviceQualityStructSize), NULL);
		}
	}
	//跳页
	else if(pageBuffer->lcdinput[0] == 0x2f10)
	{
		pageBuffer->tempvalue1 = strtol((char *)(&pbuf[7]), NULL, 10);
		if( (pageBuffer->tempvalue1 > 0) && (pageBuffer->tempvalue1 <= pageBuffer->maxpagenum))
		{
			pageBuffer->pageindex = pageBuffer->tempvalue1;
		
			pageBuffer->selectindex = 0;

			ShowRecord(pageBuffer->pageindex);
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
	SelectPage(154);
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
		pageBuffer = MyMalloc(sizeof(QualityRecordPageBuffer));
		
		if(pageBuffer)
		{
			memset(pageBuffer, 0, sizeof(QualityRecordPageBuffer));
	
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

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

static MyRes ShowRecord(unsigned char pageindex)
{
	unsigned short i=0;
		
	//设置分页读取信息
	pageBuffer->tempvalue1 = pageindex-1;
	pageBuffer->tempvalue1 *= DeviceQualityRecordPageShowNum;
	pageBuffer->deviceQualityReadPackge.pageRequest.startElementIndex = pageBuffer->tempvalue1;
	pageBuffer->deviceQualityReadPackge.pageRequest.pageSize = DeviceQualityRecordPageShowNum;
	pageBuffer->deviceQualityReadPackge.pageRequest.orderType = ASC;
	pageBuffer->deviceQualityReadPackge.pageRequest.crc = CalModbusCRC16Fun(&pageBuffer->deviceQualityReadPackge.pageRequest, PageRequestStructCrcSize, NULL);
		
	//读取数据
	readDeviceQualityFromFile(&(pageBuffer->deviceQualityReadPackge));
	
	pageBuffer->maxpagenum = ((pageBuffer->deviceQualityReadPackge.deviceRecordHeader.itemSize % DeviceQualityRecordPageShowNum) == 0)
		? (pageBuffer->deviceQualityReadPackge.deviceRecordHeader.itemSize / DeviceQualityRecordPageShowNum)
		: ((pageBuffer->deviceQualityReadPackge.deviceRecordHeader.itemSize / DeviceQualityRecordPageShowNum)+1);
		
	BasicPic(0x2f20, 0, 100, 11, 90, 940, 127, 39, 140+(pageBuffer->selectindex)*36);
	
	vTaskDelay(10 / portTICK_RATE_MS);

	pageBuffer->tempDeviceQuality = &(pageBuffer->deviceQualityReadPackge.deviceQuality[pageBuffer->deviceQualityReadPackge.readTotalNum - 1]);
	for(i=0; i<pageBuffer->deviceQualityReadPackge.readTotalNum; i++)
	{
		//显示索引
		snprintf(pageBuffer->buf, 20, "%d", (pageindex-1)*DeviceQualityRecordPageShowNum+i+1);
		DisText(0x2f30+(i)*0x30, pageBuffer->buf, strlen(pageBuffer->buf)+1);
			
		//显示项目
		snprintf(pageBuffer->buf, 50, "%s", pageBuffer->tempDeviceQuality->itemName);
		DisText(0x2f35+(i)*0x30, pageBuffer->buf, strlen(pageBuffer->buf)+1);
		
		//显示时间
		snprintf(pageBuffer->buf, 30, "20%02d-%02d-%02d %02d:%02d:%02d", pageBuffer->tempDeviceQuality->dateTime.year, 
			pageBuffer->tempDeviceQuality->dateTime.month, pageBuffer->tempDeviceQuality->dateTime.day,
			pageBuffer->tempDeviceQuality->dateTime.hour, pageBuffer->tempDeviceQuality->dateTime.min,
			pageBuffer->tempDeviceQuality->dateTime.sec);
		DisText(0x2f40+(i)*0x30, pageBuffer->buf, strlen(pageBuffer->buf)+1);
		
		//显示结果
		if(pageBuffer->tempDeviceQuality->isOk)
			snprintf(pageBuffer->buf, 10, "合格");
		else
			snprintf(pageBuffer->buf, 10, "不合格");
		DisText(0x2f4a+(i)*0x30, pageBuffer->buf, strlen(pageBuffer->buf)+1);

		//显示操作人
		snprintf(pageBuffer->buf, 30, "%s", pageBuffer->tempDeviceQuality->operator.name);
		DisText(0x2f55+(i)*0x30, pageBuffer->buf, strlen(pageBuffer->buf)+1);

		pageBuffer->tempDeviceQuality--;
		vTaskDelay(10 / portTICK_RATE_MS);
	}

	for(i=pageBuffer->deviceQualityReadPackge.readTotalNum; i<DeviceQualityRecordPageShowNum; i++)
	{
		ClearText(0x2f30+(i)*0x30);
		ClearText(0x2f35+(i)*0x30);
		ClearText(0x2f40+(i)*0x30);
		ClearText(0x2f4a+(i)*0x30);
		ClearText(0x2f55+(i)*0x30);
		
		vTaskDelay(10 / portTICK_RATE_MS);
	}
	
	return My_Pass;
}

