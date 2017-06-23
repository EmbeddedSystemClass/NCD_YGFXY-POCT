/******************************************************************************************/
/*****************************************ͷ�ļ�*******************************************/

#include	"WelcomePage.h"
#include	"Define.h"
#include	"LCD_Driver.h"
#include	"UI_Data.h"
#include	"MyMem.h"
#include	"PlaySong_Task.h"

#include	"LunchPage.h"

#include	"Test_Task.h"
#include 	"netconf.h"
#include	"HttpClient_Task.h"
#include	"CodeScan_Task.h"
#include	"Paidui_Task.h"

#include	<string.h>
#include	"stdio.h"
/******************************************************************************************/
/*****************************************�ֲ���������*************************************/
static WelcomePageBuffer * S_WelcomePageBuffer = NULL;
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
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/

/***************************************************************************************************
*FunctionName: createWelcomeActivity
*Description: ������ӭ����
*Input: thizActivity -- ��ǰ����
*Output: 
*Return: 
*Author: xsx
*Date: 2016��12��20��16:21:51
***************************************************************************************************/
MyState_TypeDef createWelcomeActivity(Activity * thizActivity, Intent * pram)
{
	if(NULL == thizActivity)
		return My_Fail;
	
	if(My_Pass == activityBufferMalloc())
	{
		InitActivity(thizActivity, "WelcomeActivity\0", activityStart, activityInput, activityFresh, activityHide, activityResume, activityDestroy);
		
		return My_Pass;
	}
	
	return My_Fail;
}

/***************************************************************************************************
*FunctionName: activityStart
*Description: ��ʾ��ǰ����
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016��12��20��16:22:23
***************************************************************************************************/
static void activityStart(void)
{
	if(S_WelcomePageBuffer)
	{
		timer_set(&(S_WelcomePageBuffer->timer), 2);
		
		//AddNumOfSongToList(0, 0);
	}

	SelectPage(1);
}

/***************************************************************************************************
*FunctionName: activityInput
*Description: ��ǰ��������
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016��12��20��16:22:42
***************************************************************************************************/
static void activityInput(unsigned char *pbuf , unsigned short len)
{
	if(S_WelcomePageBuffer)
	{
		/*����*/
		S_WelcomePageBuffer->lcdinput[0] = pbuf[4];
		S_WelcomePageBuffer->lcdinput[0] = (S_WelcomePageBuffer->lcdinput[0]<<8) + pbuf[5];
		
		S_WelcomePageBuffer->lcdinput[1] = pbuf[6];
		S_WelcomePageBuffer->lcdinput[1] = (S_WelcomePageBuffer->lcdinput[1]<<8) + pbuf[7];
		
		if(0x81 == pbuf[3])
		{
			//ҳ��id
			if(0x03 == pbuf[4])
			{
				S_WelcomePageBuffer->currentPageId = S_WelcomePageBuffer->lcdinput[1];	
			}
		}
		else if(0x83 == pbuf[3])
		{
			if((S_WelcomePageBuffer->lcdinput[0] >= 0x1010) && (S_WelcomePageBuffer->lcdinput[0] <= 0x1014))
				while(1);
		}
	}
}

/***************************************************************************************************
*FunctionName: activityFresh
*Description: ��ǰ����ˢ��
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016��12��20��16:22:56
***************************************************************************************************/
static void activityFresh(void)
{
	if(S_WelcomePageBuffer)
	{
		if(My_Pass == readSelfTestStatus(&(S_WelcomePageBuffer->selfTestStatus)))
		{
			if(SystemData_OK == S_WelcomePageBuffer->selfTestStatus)
			{
				SetLEDLight(getGBSystemSetData()->ledLightIntensity);
			}
		}
		
		if(80 == S_WelcomePageBuffer->currentPageId)
		{
			//�Լ����
			if(SelfTest_OK == S_WelcomePageBuffer->selfTestStatus)
			{
				/*������������*/
				StartvTestTask();
					
				/*������������*/
				StartEthernet();

				/*�ϴ�����*/
				StartvHttpClientTask();
					
				/*��������ά������*/
				StartCodeScanTask();
					
				//��ʼ�Ŷ�����
				StartPaiduiTask();
					
				destroyTopActivity();
				startActivity(createLunchActivity, NULL, NULL);
				
				return;
			}
			//�������ݴ���˵��sd�쳣
			else if(SystemData_ERROR == S_WelcomePageBuffer->selfTestStatus)
			{
				SelectPage(81);
				
				vTaskDelay(1000 / portTICK_RATE_MS);
				
				SendKeyCode(5);
				
				AddNumOfSongToList(5, 0);
			}
			//led�쳣���澯����ģ�����
			else if(Light_Error == S_WelcomePageBuffer->selfTestStatus)
			{
				SelectPage(81);
				vTaskDelay(1000 / portTICK_RATE_MS);
				SendKeyCode(4);
				AddNumOfSongToList(4, 0);
			}
			//�ɼ��쳣���澯�ɼ�ģ�����
			else if(AD_ERROR == S_WelcomePageBuffer->selfTestStatus)
			{
				SelectPage(81);
				vTaskDelay(1000 / portTICK_RATE_MS);
				SendKeyCode(3);
				AddNumOfSongToList(3, 0);
			}
			//�����쳣���澯����ģ�����
			else if(Motol_ERROR == S_WelcomePageBuffer->selfTestStatus)
			{
				SelectPage(81);
				vTaskDelay(1000 / portTICK_RATE_MS);
				SendKeyCode(1);
				AddNumOfSongToList(1, 0);
			}
		}
		
		if((81 != S_WelcomePageBuffer->currentPageId) && (TimeOut == timer_expired(&(S_WelcomePageBuffer->timer))))
		{
			ReadCurrentPageId();

			timer_reset(&(S_WelcomePageBuffer->timer));
		}
	}
}


static void activityHide(void)
{

}

static void activityResume(void)
{
	
}

/***************************************************************************************************
*FunctionName: activityFinish
*Description: 
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 
***************************************************************************************************/
static void activityDestroy(void)
{
	activityBufferFree();
}


/***************************************************************************************************
*FunctionName��PageBufferMalloc
*Description����ǰ������ʱ��������
*Input��None
*Output��MyState_TypeDef -- ���سɹ����
*Author��xsx
*Data��2016��6��27��08:56:02
***************************************************************************************************/
static MyState_TypeDef activityBufferMalloc(void)
{
	if(NULL == S_WelcomePageBuffer)
	{
		S_WelcomePageBuffer = (WelcomePageBuffer *)MyMalloc(sizeof(WelcomePageBuffer));
			
		if(S_WelcomePageBuffer)
		{
			memset(S_WelcomePageBuffer, 0, sizeof(WelcomePageBuffer));
		
			return My_Pass;
			
		}
	}
	
	return My_Fail;
}

/***************************************************************************************************
*FunctionName��PageBufferFree
*Description����ǰ������ʱ�����ͷ�
*Input��None
*Output��None
*Author��xsx
*Data��2016��6��27��08:56:21
***************************************************************************************************/
static void activityBufferFree(void)
{
	MyFree(S_WelcomePageBuffer);
	S_WelcomePageBuffer = NULL;
}


