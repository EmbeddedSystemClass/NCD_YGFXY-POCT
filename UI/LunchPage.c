/******************************************************************************************/
/*****************************************ͷ�ļ�*******************************************/

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
/*****************************************�ֲ���������*************************************/
static LunchPageBuffer * page = NULL;
/******************************************************************************************/
/*****************************************�ֲ���������*************************************/
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
*Description: ����������
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016��12��21��09:00:09
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
*Description: ��ʾ������
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016��12��21��09:00:32
***************************************************************************************************/
static void activityStart(void)
{
	timer_set(&(page->timer), getGBSystemSetData()->ledSleepTime);
	
	DspPageText();
	
	SelectPage(82);

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
	/*����*/
	page->lcdinput[0] = pbuf[4];
	page->lcdinput[0] = (page->lcdinput[0]<<8) + pbuf[5];
	
	//��������ʱ��
	timer_restart(&(page->timer));
		
	//����
	if(page->lcdinput[0] == 0x1103)
	{
		startActivity(createSystemSetActivity, NULL, NULL);
	}
	//�鿴����
	else if(page->lcdinput[0] == 0x1102)
	{	
		startActivity(createRecordActivity, NULL, NULL);
	}
	//�������
	else if(page->lcdinput[0] == 0x1100)
	{	
		page->error = CreateANewTest(NormalTestType);
		//�����ɹ�
		if(Error_OK == page->error)
		{
			page->tempOperator = &(GetCurrentTestItem()->testData.operator);
			startActivity(createSelectUserActivity, createIntent(&(page->tempOperator), 4), createSampleActivity);
		}
		//��ֹ�������
		else if(Error_StopNormalTest == page->error)
		{
			SendKeyCode(1);
			AddNumOfSongToList(8, 0);
		}
		//����ʧ��
		else if(Error_Mem == page->error)
		{
			SendKeyCode(2);
			AddNumOfSongToList(7, 0);
		}
	}
	//��������
	else if(page->lcdinput[0] == 0x1101)
	{
		//�п��Ŷӣ�������Ŷӽ���
		if(true == IsPaiDuiTestting())
		{
			startActivity(createPaiDuiActivity, NULL, NULL);
		}
		//�޿��Ŷ���ʼ����
		else
		{
			page->error = CreateANewTest(PaiDuiTestType);
			//�����ɹ�
			if(Error_OK == page->error)
			{
				page->tempOperator = &(GetCurrentTestItem()->testData.operator);
				startActivity(createSelectUserActivity, createIntent(&(page->tempOperator), 4), createSampleActivity);
			}
			//����ʧ��
			else if(Error_Mem == page->error)
			{
				SendKeyCode(2);
				AddNumOfSongToList(7, 0);
			}
			//�Ŷ�ģ��ʧ��
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
*Description: ����ˢ��
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016��12��21��09:01:16
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
	timer_restart(&(page->timer));
	
	SelectPage(82);
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
*Description: �����ڴ��ͷ�
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016��12��21��09:03:10
***************************************************************************************************/
static void activityBufferFree(void)
{
	MyFree(page);
	page = NULL;
}

/***************************************************************************************************
*FunctionName: DspPageText
*Description: ��ǰ����������ʾ
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016��12��21��09:03:42
***************************************************************************************************/
static void DspPageText(void)
{
	memset(page->buf, 0, 100);
	sprintf(page->buf, "V%d.%d.%02d", GB_SoftVersion/1000, GB_SoftVersion%1000/100, GB_SoftVersion%100);
	DisText(0x1110, page->buf, 30);
}

