
/******************************************************************************************/
/*****************************************ͷ�ļ�*******************************************/

#include	"SleepPage.h"

#include	"LCD_Driver.h"
#include	"UI_Data.h"
#include	"System_Data.h"

#include	"MyMem.h"

#include 	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"

#include	<string.h>
#include	"stdio.h"

/******************************************************************************************/
/*****************************************�ֲ���������*************************************/
static SleepPageBuffer *S_SleepPageBuffer = NULL;
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
MyState_TypeDef createSleepActivity(Activity * thizActivity, Intent * pram)
{
	if(NULL == thizActivity)
		return My_Fail;
	
	if(My_Pass == activityBufferMalloc())
	{
		InitActivity(thizActivity, "SleepActivity\0", activityStart, activityInput, activityFresh, activityHide, activityResume, activityDestroy);
		
		return My_Pass;
	}
	
	return My_Fail;
}

static void activityStart(void)
{
	if(S_SleepPageBuffer)
	{
		
	}	
	
	SetLEDLight(10);
	
	SelectPage(142);
}
static void activityInput(unsigned char *pbuf , unsigned short len)
{
	if(S_SleepPageBuffer)
	{
		/*����*/
		S_SleepPageBuffer->lcdinput[0] = pbuf[4];
		S_SleepPageBuffer->lcdinput[0] = (S_SleepPageBuffer->lcdinput[0]<<8) + pbuf[5];
		
		/*����*/
		if(S_SleepPageBuffer->lcdinput[0] == 0x1D70)
		{
			backToFatherActivity();
		}
	}
}
static void activityFresh(void)
{

}
static void activityHide(void)
{

}
static void activityResume(void)
{

}
static void activityDestroy(void)
{
	SetLEDLight(getGBSystemSetData()->ledLightIntensity);
	
	activityBufferFree();
}

static MyState_TypeDef activityBufferMalloc(void)
{
	if(NULL == S_SleepPageBuffer)
	{
		S_SleepPageBuffer = MyMalloc(sizeof(SleepPageBuffer));
		
		if(S_SleepPageBuffer)
		{
			memset(S_SleepPageBuffer, 0, sizeof(SleepPageBuffer));
			
			return My_Pass;
		}
	}

	return My_Fail;
}

static void activityBufferFree(void)
{
	MyFree(S_SleepPageBuffer);
	S_SleepPageBuffer = NULL;
}

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

