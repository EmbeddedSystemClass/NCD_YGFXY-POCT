/******************************************************************************************/
/*****************************************ͷ�ļ�*******************************************/

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
/*****************************************�ֲ���������*************************************/
static RecordPageBuffer * S_RecordPageBuffer = NULL;
/******************************************************************************************/
/*****************************************�ֲ���������*************************************/
static void activityStart(void);
static void activityInput(unsigned char *pbuf , unsigned short len);
static void activityFresh(void);
static void activityHide(void);
static void activityResume(void);
static void activityDestroy(void);
static MyState_TypeDef activityBufferMalloc(void);
static void activityBufferFree(void);

static MyState_TypeDef ShowRecord(unsigned char pageindex);
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/

/***************************************************************************************************
*FunctionName: createSelectUserActivity
*Description: ����ѡ������˽���
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016��12��21��09:00:09
***************************************************************************************************/
MyState_TypeDef createRecordActivity(Activity * thizActivity, Intent * pram)
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
*Description: ��ʾ������
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016��12��21��09:00:32
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
*Description: ��������
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016��12��21��09:00:59
***************************************************************************************************/
static void activityInput(unsigned char *pbuf , unsigned short len)
{
	if(S_RecordPageBuffer)
	{
		/*����*/
		S_RecordPageBuffer->lcdinput[0] = pbuf[4];
		S_RecordPageBuffer->lcdinput[0] = (S_RecordPageBuffer->lcdinput[0]<<8) + pbuf[5];
		
		/*����*/
		if(S_RecordPageBuffer->lcdinput[0] == 0x2000)
		{
			backToFatherActivity();
		}
		//�鿴
		else if(S_RecordPageBuffer->lcdinput[0] == 0x2001)
		{
			//if((S_RecordPageBuffer->selectindex > 0) && (S_RecordPageBuffer->selectindex <= S_RecordPageBuffer->page.ElementsSize))
			//	startActivity(createShowResultActivity, createIntent(&S_RecordPageBuffer->page.testData[S_RecordPageBuffer->page.ElementsSize - S_RecordPageBuffer->selectindex], sizeof(TestData)));
		}
		/*��һҳ*/
		else if(S_RecordPageBuffer->lcdinput[0] == 0x2002)
		{
			if(S_RecordPageBuffer->pageindex > 1)
				S_RecordPageBuffer->pageindex -= 1;
			else
				S_RecordPageBuffer->pageindex = S_RecordPageBuffer->maxpagenum;
				
			ShowRecord(S_RecordPageBuffer->pageindex);
		}
		/*��һҳ*/
		else if(S_RecordPageBuffer->lcdinput[0] == 0x2003)
		{
			if(S_RecordPageBuffer->pageindex < S_RecordPageBuffer->maxpagenum)
				S_RecordPageBuffer->pageindex += 1;
			else
				S_RecordPageBuffer->pageindex = 1;
				
			ShowRecord(S_RecordPageBuffer->pageindex);
		}
		//ѡ������
		else if((S_RecordPageBuffer->lcdinput[0] >= 0x2004)&&(S_RecordPageBuffer->lcdinput[0] <= 0x200b))
		{
			S_RecordPageBuffer->tempvalue1 = S_RecordPageBuffer->lcdinput[0] - 0x2004 + 1;
			
			if(S_RecordPageBuffer->tempvalue1 <= S_RecordPageBuffer->testDataRecordReadPackage.readTotalNum)
			{
				S_RecordPageBuffer->selectindex = S_RecordPageBuffer->tempvalue1;
				BasicPic(0x2020, 1, 137, 11, 142, 940, 179, 38, 149+(S_RecordPageBuffer->selectindex - 1)*40);
				startActivity(createShowResultActivity, createIntent(&(S_RecordPageBuffer->testDataRecordReadPackage.testData[S_RecordPageBuffer->testDataRecordReadPackage.readTotalNum - S_RecordPageBuffer->selectindex]), TestDataStructSize));
			}
		}
		//��ҳ
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
*Description: ����ˢ��
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016��12��21��09:01:16
***************************************************************************************************/
static void activityFresh(void)
{

}

/***************************************************************************************************
*FunctionName: activityHide
*Description: ���ؽ���ʱҪ������
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016��12��21��09:01:40
***************************************************************************************************/
static void activityHide(void)
{

}

/***************************************************************************************************
*FunctionName: activityResume
*Description: ����ָ���ʾʱҪ������
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016��12��21��09:01:58
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
*Description: ��������
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016��12��21��09:02:15
***************************************************************************************************/
static void activityDestroy(void)
{
	activityBufferFree();
}

/***************************************************************************************************
*FunctionName: activityBufferMalloc
*Description: ���������ڴ�����
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 
***************************************************************************************************/
static MyState_TypeDef activityBufferMalloc(void)
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
*Description: �����ڴ��ͷ�
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016��12��21��09:03:10
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

static MyState_TypeDef ShowRecord(unsigned char pageindex)
{
	unsigned short i=0;
		
	//���÷�ҳ��ȡ��Ϣ
	S_RecordPageBuffer->tempvalue1 = pageindex-1;
	S_RecordPageBuffer->tempvalue1 *= TestDataRecordPageShowNum;
	S_RecordPageBuffer->testDataRecordReadPackage.pageRequest.startElementIndex = S_RecordPageBuffer->tempvalue1;
	S_RecordPageBuffer->testDataRecordReadPackage.pageRequest.pageSize = TestDataRecordPageShowNum;
	S_RecordPageBuffer->testDataRecordReadPackage.pageRequest.orderType = ASC;
	S_RecordPageBuffer->testDataRecordReadPackage.pageRequest.crc = CalModbusCRC16Fun1(&S_RecordPageBuffer->testDataRecordReadPackage.pageRequest, PageRequestStructCrcSize);
		
	//��ȡ����
	readTestDataFromFile(&(S_RecordPageBuffer->testDataRecordReadPackage));
	
	S_RecordPageBuffer->maxpagenum = ((S_RecordPageBuffer->testDataRecordReadPackage.deviceRecordHeader.itemSize % TestDataRecordPageShowNum) == 0)
		? (S_RecordPageBuffer->testDataRecordReadPackage.deviceRecordHeader.itemSize / TestDataRecordPageShowNum)
		: ((S_RecordPageBuffer->testDataRecordReadPackage.deviceRecordHeader.itemSize / TestDataRecordPageShowNum)+1);
		
	BasicPic(0x2020, 0, 100, 11, 142, 940, 179, 39, 140+(S_RecordPageBuffer->selectindex)*36);
	
	vTaskDelay(10 / portTICK_RATE_MS);

	S_RecordPageBuffer->tempdata = &(S_RecordPageBuffer->testDataRecordReadPackage.testData[S_RecordPageBuffer->testDataRecordReadPackage.readTotalNum - 1]);
	for(i=0; i<S_RecordPageBuffer->testDataRecordReadPackage.readTotalNum; i++)
	{
		//��ʾ����
		sprintf(S_RecordPageBuffer->buf, "%d\0", (pageindex-1)*TestDataRecordPageShowNum+i+1);
		DisText(0x2030+(i)*0x40, S_RecordPageBuffer->buf, strlen(S_RecordPageBuffer->buf)+1);
			
		//��ʾ��Ŀ
		sprintf(S_RecordPageBuffer->buf, "%.11s\0", S_RecordPageBuffer->tempdata->qrCode.ItemName);
		DisText(0x2036+(i)*0x40, S_RecordPageBuffer->buf, strlen(S_RecordPageBuffer->buf)+1);
			
		//��ʾ��Ʒ���
		sprintf(S_RecordPageBuffer->buf, "%.15s\0", S_RecordPageBuffer->tempdata->sampleid);
		DisText(0x2040+(i)*0x40, S_RecordPageBuffer->buf, strlen(S_RecordPageBuffer->buf)+1);
			
		//��ʾ���
		if(S_RecordPageBuffer->tempdata->testResultDesc != ResultIsOK)
		{
			sprintf(S_RecordPageBuffer->buf, "Error\0");
		}
		else if(IsShowRealValue() == true)
			sprintf(S_RecordPageBuffer->buf, "%.*f %s\0", S_RecordPageBuffer->tempdata->qrCode.itemConstData.pointNum, 
				S_RecordPageBuffer->tempdata->testSeries.AdjustResult, S_RecordPageBuffer->tempdata->qrCode.itemConstData.itemMeasure);
		else if(S_RecordPageBuffer->tempdata->testSeries.AdjustResult <= S_RecordPageBuffer->tempdata->qrCode.itemConstData.lowstResult)
			sprintf(S_RecordPageBuffer->buf, "<%.*f %s\0", S_RecordPageBuffer->tempdata->qrCode.itemConstData.pointNum, 
				S_RecordPageBuffer->tempdata->qrCode.itemConstData.lowstResult, S_RecordPageBuffer->tempdata->qrCode.itemConstData.itemMeasure);
		else if(S_RecordPageBuffer->tempdata->testSeries.AdjustResult >= S_RecordPageBuffer->tempdata->qrCode.itemConstData.highestResult)
			sprintf(S_RecordPageBuffer->buf, ">%.*f %s\0", S_RecordPageBuffer->tempdata->qrCode.itemConstData.pointNum, 
				S_RecordPageBuffer->tempdata->qrCode.itemConstData.highestResult, S_RecordPageBuffer->tempdata->qrCode.itemConstData.itemMeasure);
		else
			sprintf(S_RecordPageBuffer->buf, "%.*f %s\0", S_RecordPageBuffer->tempdata->qrCode.itemConstData.pointNum, 
				S_RecordPageBuffer->tempdata->testSeries.AdjustResult, S_RecordPageBuffer->tempdata->qrCode.itemConstData.itemMeasure);
		DisText(0x204C+(i)*0x40, S_RecordPageBuffer->buf, strlen(S_RecordPageBuffer->buf)+1);
			
		//��ʾʱ��
		sprintf(S_RecordPageBuffer->buf, "%02d-%02d-%02d %02d:%02d\0", S_RecordPageBuffer->tempdata->testDateTime.year, 
			S_RecordPageBuffer->tempdata->testDateTime.month, S_RecordPageBuffer->tempdata->testDateTime.day,
			S_RecordPageBuffer->tempdata->testDateTime.hour, S_RecordPageBuffer->tempdata->testDateTime.min);
		DisText(0x2058+(i)*0x40, S_RecordPageBuffer->buf, strlen(S_RecordPageBuffer->buf)+1);
			
		//��ʾ������
		sprintf(S_RecordPageBuffer->buf, "%s\0", S_RecordPageBuffer->tempdata->operator.name);
		DisText(0x2065+(i)*0x40, S_RecordPageBuffer->buf, strlen(S_RecordPageBuffer->buf)+1);

		S_RecordPageBuffer->tempdata--;
		vTaskDelay(10 / portTICK_RATE_MS);
	}

	for(i=S_RecordPageBuffer->testDataRecordReadPackage.readTotalNum; i<TestDataRecordPageShowNum; i++)
	{
		ClearText(0x2030+(i)*0x40);
		ClearText(0x2036+(i)*0x40);
		ClearText(0x2040+(i)*0x40);
		ClearText(0x204c+(i)*0x40);
		ClearText(0x2058+(i)*0x40);
		ClearText(0x2065+(i)*0x40);
		
		vTaskDelay(10 / portTICK_RATE_MS);
	}
	
	return My_Pass;
}

