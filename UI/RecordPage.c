/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"RecordPage.h"

#include	"LCD_Driver.h"

#include	"Define.h"
#include	"MyMem.h"

#include	"SystemSetPage.h"
#include	"ShowResultPage.h"
#include	"CRC16.h"
#include	"Printf_Fun.h"
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
static RecordPageBuffer * S_RecordPageBuffer = NULL;
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
MyRes createRecordActivity(Activity * thizActivity, Intent * pram)
{
	if(NULL == thizActivity)
		return My_Fail;
	
	if(My_Pass == activityBufferMalloc())
	{
		InitActivity(thizActivity, "RecordActivity\0", activityStart, activityInput, activityFresh, activityHide, activityResume, activityDestroy);
		
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
	if(S_RecordPageBuffer)
	{
		S_RecordPageBuffer->selectindex = 0;
		S_RecordPageBuffer->pageindex = 1;
		ShowRecord(S_RecordPageBuffer->pageindex);
	}
	
	SelectPage(114);
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
	if(S_RecordPageBuffer)
	{
		/*命令*/
		S_RecordPageBuffer->lcdinput[0] = pbuf[4];
		S_RecordPageBuffer->lcdinput[0] = (S_RecordPageBuffer->lcdinput[0]<<8) + pbuf[5];
		
		/*返回*/
		if(S_RecordPageBuffer->lcdinput[0] == 0x2000)
		{
			backToFatherActivity();
		}
		//查看
		else if(S_RecordPageBuffer->lcdinput[0] == 0x2001)
		{
			//if((S_RecordPageBuffer->selectindex > 0) && (S_RecordPageBuffer->selectindex <= S_RecordPageBuffer->page.ElementsSize))
			//	startActivity(createShowResultActivity, createIntent(&S_RecordPageBuffer->page.testData[S_RecordPageBuffer->page.ElementsSize - S_RecordPageBuffer->selectindex], sizeof(TestData)));
		}
		/*上一页*/
		else if(S_RecordPageBuffer->lcdinput[0] == 0x2002)
		{
			if(S_RecordPageBuffer->pageindex > 1)
				S_RecordPageBuffer->pageindex -= 1;
			else
				S_RecordPageBuffer->pageindex = S_RecordPageBuffer->maxpagenum;
				
			ShowRecord(S_RecordPageBuffer->pageindex);
		}
		/*下一页*/
		else if(S_RecordPageBuffer->lcdinput[0] == 0x2003)
		{
			if(S_RecordPageBuffer->pageindex < S_RecordPageBuffer->maxpagenum)
				S_RecordPageBuffer->pageindex += 1;
			else
				S_RecordPageBuffer->pageindex = 1;
				
			ShowRecord(S_RecordPageBuffer->pageindex);
		}
		//选择数据
		else if((S_RecordPageBuffer->lcdinput[0] >= 0x2004)&&(S_RecordPageBuffer->lcdinput[0] <= 0x200b))
		{
			S_RecordPageBuffer->tempvalue1 = S_RecordPageBuffer->lcdinput[0] - 0x2004 + 1;
			
			if(S_RecordPageBuffer->tempvalue1 <= S_RecordPageBuffer->readTotalNum)
			{
				S_RecordPageBuffer->selectindex = S_RecordPageBuffer->tempvalue1;
				BasicPic(0x2020, 1, 137, 11, 142, 940, 179, 38, 149+(S_RecordPageBuffer->selectindex - 1)*40);
				startActivity(createShowResultActivity, createIntent(&(S_RecordPageBuffer->testData[S_RecordPageBuffer->readTotalNum - S_RecordPageBuffer->selectindex]), TestDataStructSize), NULL);
			}
		}
		//跳页
		else if(S_RecordPageBuffer->lcdinput[0] == 0x2010)
		{
			S_RecordPageBuffer->tempvalue1 = strtol((char *)(&pbuf[7]), NULL, 10);
			if( (S_RecordPageBuffer->tempvalue1 > 0) && (S_RecordPageBuffer->tempvalue1 <= S_RecordPageBuffer->maxpagenum))
			{
				S_RecordPageBuffer->pageindex = S_RecordPageBuffer->tempvalue1;
		
				S_RecordPageBuffer->selectindex = 0;

				ShowRecord(S_RecordPageBuffer->pageindex);
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
	if(S_RecordPageBuffer)
	{

	}
	
	SelectPage(114);
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
	if(NULL == S_RecordPageBuffer)
	{
		S_RecordPageBuffer = MyMalloc(sizeof(RecordPageBuffer));
		
		if(S_RecordPageBuffer)
		{
			memset(S_RecordPageBuffer, 0, sizeof(RecordPageBuffer));
	
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
	MyFree(S_RecordPageBuffer);
	S_RecordPageBuffer = NULL;
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
	S_RecordPageBuffer->tempvalue1 = pageindex-1;
	S_RecordPageBuffer->tempvalue1 *= TestDataRecordPageShowNum;
	S_RecordPageBuffer->pageRequest.startElementIndex = S_RecordPageBuffer->tempvalue1;
	S_RecordPageBuffer->pageRequest.pageSize = TestDataRecordPageShowNum;
	S_RecordPageBuffer->pageRequest.orderType = ASC;
	S_RecordPageBuffer->pageRequest.crc = CalModbusCRC16Fun(&S_RecordPageBuffer->pageRequest, PageRequestStructCrcSize, NULL);

	//读取数据
	memset(S_RecordPageBuffer->testData, 0, TestDataRecordPageShowNum * TestDataStructSize);
	readTestDataFromFile(&(S_RecordPageBuffer->pageRequest), &(S_RecordPageBuffer->deviceRecordHeader), S_RecordPageBuffer->testData, NULL);
	
	S_RecordPageBuffer->maxpagenum = ((S_RecordPageBuffer->deviceRecordHeader.itemSize % TestDataRecordPageShowNum) == 0)
		? (S_RecordPageBuffer->deviceRecordHeader.itemSize / TestDataRecordPageShowNum)
		: ((S_RecordPageBuffer->deviceRecordHeader.itemSize / TestDataRecordPageShowNum)+1);
		
	BasicPic(0x2020, 0, 100, 11, 142, 940, 179, 39, 140+(S_RecordPageBuffer->selectindex)*36);
	
	vTaskDelay(10 / portTICK_RATE_MS);

	S_RecordPageBuffer->readTotalNum = 0;
	for(i=0; i<TestDataRecordPageShowNum; i++)
	{
		S_RecordPageBuffer->tempdata = &(S_RecordPageBuffer->testData[TestDataRecordPageShowNum - i - 1]);
		
		if(S_RecordPageBuffer->tempdata->crc == CalModbusCRC16Fun(S_RecordPageBuffer->tempdata, TestDataStructCrcSize, NULL))
		{
			//显示索引
			snprintf(S_RecordPageBuffer->buf, 10, "%d", (pageindex-1)*TestDataRecordPageShowNum+S_RecordPageBuffer->readTotalNum+1);
			DisText(0x2030+(S_RecordPageBuffer->readTotalNum)*0x40, S_RecordPageBuffer->buf, strlen(S_RecordPageBuffer->buf)+1);
				
			//显示项目
			snprintf(S_RecordPageBuffer->buf, ItemNameLen, "%.11s", S_RecordPageBuffer->tempdata->qrCode.ItemName);
			DisText(0x2036+(S_RecordPageBuffer->readTotalNum)*0x40, S_RecordPageBuffer->buf, strlen(S_RecordPageBuffer->buf)+1);
				
			//显示样品编号
			snprintf(S_RecordPageBuffer->buf, 16, "%.15s", S_RecordPageBuffer->tempdata->sampleid);
			DisText(0x2040+(S_RecordPageBuffer->readTotalNum)*0x40, S_RecordPageBuffer->buf, strlen(S_RecordPageBuffer->buf)+1);
				
			//显示结果
			if(S_RecordPageBuffer->tempdata->testResultDesc != ResultIsOK)
			{
				snprintf(S_RecordPageBuffer->buf, 10, "Error");
			}
			else if(IsShowRealValue() == true)
				snprintf(S_RecordPageBuffer->buf, 20, "%.*f %s", S_RecordPageBuffer->tempdata->qrCode.itemConstData.pointNum, 
					S_RecordPageBuffer->tempdata->testSeries.AdjustResult, S_RecordPageBuffer->tempdata->qrCode.itemConstData.itemMeasure);
			else if(S_RecordPageBuffer->tempdata->testSeries.AdjustResult <= S_RecordPageBuffer->tempdata->qrCode.itemConstData.lowstResult)
				snprintf(S_RecordPageBuffer->buf, 20, "<%.*f %s", S_RecordPageBuffer->tempdata->qrCode.itemConstData.pointNum, 
					S_RecordPageBuffer->tempdata->qrCode.itemConstData.lowstResult, S_RecordPageBuffer->tempdata->qrCode.itemConstData.itemMeasure);
			else if(S_RecordPageBuffer->tempdata->testSeries.AdjustResult >= S_RecordPageBuffer->tempdata->qrCode.itemConstData.highestResult)
				snprintf(S_RecordPageBuffer->buf, 20, ">%.*f %s", S_RecordPageBuffer->tempdata->qrCode.itemConstData.pointNum, 
					S_RecordPageBuffer->tempdata->qrCode.itemConstData.highestResult, S_RecordPageBuffer->tempdata->qrCode.itemConstData.itemMeasure);
			else
				snprintf(S_RecordPageBuffer->buf, 20, "%.*f %s", S_RecordPageBuffer->tempdata->qrCode.itemConstData.pointNum, 
					S_RecordPageBuffer->tempdata->testSeries.AdjustResult, S_RecordPageBuffer->tempdata->qrCode.itemConstData.itemMeasure);
			DisText(0x204C+(S_RecordPageBuffer->readTotalNum)*0x40, S_RecordPageBuffer->buf, strlen(S_RecordPageBuffer->buf)+1);
				
			//显示时间
			snprintf(S_RecordPageBuffer->buf, 15, "%02d-%02d-%02d %02d:%02d", S_RecordPageBuffer->tempdata->testDateTime.year, 
				S_RecordPageBuffer->tempdata->testDateTime.month, S_RecordPageBuffer->tempdata->testDateTime.day,
				S_RecordPageBuffer->tempdata->testDateTime.hour, S_RecordPageBuffer->tempdata->testDateTime.min);
			DisText(0x2058+(S_RecordPageBuffer->readTotalNum)*0x40, S_RecordPageBuffer->buf, strlen(S_RecordPageBuffer->buf)+1);
				
			//显示操作人
			snprintf(S_RecordPageBuffer->buf, 15, "%s", S_RecordPageBuffer->tempdata->operator.name);
			DisText(0x2065+(S_RecordPageBuffer->readTotalNum)*0x40, S_RecordPageBuffer->buf, strlen(S_RecordPageBuffer->buf)+1);

			S_RecordPageBuffer->tempdata--;
			
			S_RecordPageBuffer->readTotalNum++;
		}
		else
		{
			ClearText(0x2030+(TestDataRecordPageShowNum - i - 1)*0x40);
			ClearText(0x2036+(TestDataRecordPageShowNum - i - 1)*0x40);
			ClearText(0x2040+(TestDataRecordPageShowNum - i - 1)*0x40);
			ClearText(0x204c+(TestDataRecordPageShowNum - i - 1)*0x40);
			ClearText(0x2058+(TestDataRecordPageShowNum - i - 1)*0x40);
			ClearText(0x2065+(TestDataRecordPageShowNum - i - 1)*0x40);
		}
		
		vTaskDelay(10 / portTICK_RATE_MS);
	}
	
	return My_Pass;
}

