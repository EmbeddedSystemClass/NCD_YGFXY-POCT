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
static LunchPageBuffer * S_LunchPageBuffer = NULL;
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
MyState_TypeDef createLunchActivity(Activity * thizActivity, Intent * pram)
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
	if(S_LunchPageBuffer)
	{		
		timer_set(&(S_LunchPageBuffer->timer), getGBSystemSetData()->ledSleepTime);
	
		DspPageText();
	}
	
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
	if(S_LunchPageBuffer)
	{
		/*����*/
		S_LunchPageBuffer->lcdinput[0] = pbuf[4];
		S_LunchPageBuffer->lcdinput[0] = (S_LunchPageBuffer->lcdinput[0]<<8) + pbuf[5];
		
		//��������ʱ��
		timer_restart(&(S_LunchPageBuffer->timer));
		
		//����
		if(S_LunchPageBuffer->lcdinput[0] == 0x1103)
		{
			startActivity(createSystemSetActivity, NULL, NULL);
		}
		//�鿴����
		else if(S_LunchPageBuffer->lcdinput[0] == 0x1102)
		{	
			startActivity(createRecordActivity, NULL, NULL);
		}
		//�������
		else if(S_LunchPageBuffer->lcdinput[0] == 0x1100)
		{	
			S_LunchPageBuffer->error = CreateANewTest(NormalTestType);
			//�����ɹ�
			if(Error_OK == S_LunchPageBuffer->error)
			{
				S_LunchPageBuffer->tempOperator = &(GetCurrentTestItem()->testData.operator);
				startActivity(createSelectUserActivity, createIntent(&(S_LunchPageBuffer->tempOperator), 4), createSampleActivity);
			}
			//��ֹ�������
			else if(Error_StopNormalTest == S_LunchPageBuffer->error)
			{
				SendKeyCode(1);
				AddNumOfSongToList(8, 0);
			}
			//����ʧ��
			else if(Error_Mem == S_LunchPageBuffer->error)
			{
				SendKeyCode(2);
				AddNumOfSongToList(7, 0);
			}
		}
		//��������
		else if(S_LunchPageBuffer->lcdinput[0] == 0x1101)
		{
			//�п��Ŷӣ�������Ŷӽ���
			if(true == IsPaiDuiTestting())
			{
				startActivity(createPaiDuiActivity, NULL, NULL);
			}
			//�޿��Ŷ���ʼ����
			else
			{
				S_LunchPageBuffer->error = CreateANewTest(PaiDuiTestType);
				//�����ɹ�
				if(Error_OK == S_LunchPageBuffer->error)
				{
					S_LunchPageBuffer->tempOperator = &(GetCurrentTestItem()->testData.operator);
					startActivity(createSelectUserActivity, createIntent(&(S_LunchPageBuffer->tempOperator), 4), createSampleActivity);
				}
				//����ʧ��
				else if(Error_Mem == S_LunchPageBuffer->error)
				{
					SendKeyCode(2);
					AddNumOfSongToList(7, 0);
				}
				//�Ŷ�ģ��ʧ��
				else if(Error_PaiduiDisconnect == S_LunchPageBuffer->error)
				{
					SendKeyCode(3);
					AddNumOfSongToList(58, 0);
				}
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
	if(TimeOut == timer_expired(&(S_LunchPageBuffer->timer)))
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
	if(S_LunchPageBuffer)
	{
		timer_restart(&(S_LunchPageBuffer->timer));
	}
	
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
static MyState_TypeDef activityBufferMalloc(void)
{
	if(NULL == S_LunchPageBuffer)
	{
		S_LunchPageBuffer = MyMalloc(sizeof(LunchPageBuffer));
		if(S_LunchPageBuffer)	
		{
			memset(S_LunchPageBuffer, 0, sizeof(LunchPageBuffer));
	
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
	MyFree(S_LunchPageBuffer);
	S_LunchPageBuffer = NULL;
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
	if(S_LunchPageBuffer)
	{
		memset(S_LunchPageBuffer->buf, 0, 100);
		sprintf(S_LunchPageBuffer->buf, "V%d.%d.%02d", GB_SoftVersion/1000, GB_SoftVersion%1000/100, GB_SoftVersion%100);
		DisText(0x1110, S_LunchPageBuffer->buf, 30);
	}
}

