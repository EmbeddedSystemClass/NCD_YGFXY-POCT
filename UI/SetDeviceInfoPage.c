/******************************************************************************************/
/*****************************************ͷ�ļ�*******************************************/
#if 0

#include	"SetDeviceInfoPage.h"

#include	"LCD_Driver.h"
#include	"MyMem.h"
#include	"ShowDeviceInfoPage.h"
#include	"System_Data.h"
#include	"SystemSet_Dao.h"
#include	"SleepPage.h"

#include 	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"

#include	<string.h>
#include	"stdio.h"

/******************************************************************************************/
/*****************************************�ֲ���������*************************************/
static SetDeviceInfoPageBuffer * S_SetDeviceInfoPageBuffer = NULL;
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

static void showDeviceInfoText(void);
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
MyState_TypeDef createSetDeviceInfoActivity(Activity * thizActivity, Intent * pram)
{
	if(NULL == thizActivity)
		return My_Fail;
	
	if(My_Pass == activityBufferMalloc())
	{
		InitActivity(thizActivity, "SetDeviceInfoActivity\0", activityStart, activityInput, activityFresh, activityHide, activityResume, activityDestroy);
		
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
	if(S_SetDeviceInfoPageBuffer)
	{
		copyGBSystemSetData(&(S_SetDeviceInfoPageBuffer->systemSetData));
		
		memcpy(&(S_SetDeviceInfoPageBuffer->user), &(S_SetDeviceInfoPageBuffer->systemSetData.device.deviceuser), sizeof(User_Type));
		
		memcpy(S_SetDeviceInfoPageBuffer->deviceunit, S_SetDeviceInfoPageBuffer->systemSetData.device.deviceunit, MaxDeviceUnitLen);
		
		showDeviceInfoText();
	}
	
	SelectPage(102);
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
	if(S_SetDeviceInfoPageBuffer)
	{
		/*����*/
		S_SetDeviceInfoPageBuffer->lcdinput[0] = pbuf[4];
		S_SetDeviceInfoPageBuffer->lcdinput[0] = (S_SetDeviceInfoPageBuffer->lcdinput[0]<<8) + pbuf[5];
		
		/*����*/
		if(S_SetDeviceInfoPageBuffer->lcdinput[0] == 0x1B00)
		{
			backToFatherActivity();
		}
		/*ȷ��*/
		else if(S_SetDeviceInfoPageBuffer->lcdinput[0] == 0x1B01)
		{			
			if(S_SetDeviceInfoPageBuffer->ismodify == 1)
			{
				copyGBSystemSetData(&(S_SetDeviceInfoPageBuffer->systemSetData));
		
				memcpy(&(S_SetDeviceInfoPageBuffer->systemSetData.device.deviceuser), &(S_SetDeviceInfoPageBuffer->user), sizeof(User_Type));
		
				memcpy(S_SetDeviceInfoPageBuffer->systemSetData.device.deviceunit, S_SetDeviceInfoPageBuffer->deviceunit, MaxDeviceUnitLen);
				
				S_SetDeviceInfoPageBuffer->systemSetData.device.isnew = true;
				if(My_Pass == SaveSystemSetData(&(S_SetDeviceInfoPageBuffer->systemSetData)))
				{
					SendKeyCode(1);
					S_SetDeviceInfoPageBuffer->ismodify = 0;
				}
				else
					SendKeyCode(2);
			}
		}
		/*����*/
		else if(S_SetDeviceInfoPageBuffer->lcdinput[0] == 0x1B10)
		{
			memset(S_SetDeviceInfoPageBuffer->user.user_name, 0, MaxNameLen);
			memcpy(S_SetDeviceInfoPageBuffer->user.user_name, &pbuf[7], GetBufLen(&pbuf[7] , 2*pbuf[6]));
			S_SetDeviceInfoPageBuffer->ismodify = 1;
		}
		/*����*/
		else if(S_SetDeviceInfoPageBuffer->lcdinput[0] == 0x1B20)
		{
			memset(S_SetDeviceInfoPageBuffer->user.user_age, 0, MaxAgeLen);
			memcpy(S_SetDeviceInfoPageBuffer->user.user_age, &pbuf[7], GetBufLen(&pbuf[7] , 2*pbuf[6]));
			S_SetDeviceInfoPageBuffer->ismodify = 1;
		}
		/*�Ա�*/
		else if(S_SetDeviceInfoPageBuffer->lcdinput[0] == 0x1B30)
		{
			memset(S_SetDeviceInfoPageBuffer->user.user_sex, 0, MaxSexLen);
			memcpy(S_SetDeviceInfoPageBuffer->user.user_sex, &pbuf[7], GetBufLen(&pbuf[7] , 2*pbuf[6]));
			S_SetDeviceInfoPageBuffer->ismodify = 1;
		}
		/*��ϵ��ʽ*/
		else if(S_SetDeviceInfoPageBuffer->lcdinput[0] == 0x1B40)
		{
			memset(S_SetDeviceInfoPageBuffer->user.user_phone, 0, MaxPhoneLen);
			memcpy(S_SetDeviceInfoPageBuffer->user.user_phone, &pbuf[7], GetBufLen(&pbuf[7] , 2*pbuf[6]));
			S_SetDeviceInfoPageBuffer->ismodify = 1;
		}
		/*ְλ*/
		else if(S_SetDeviceInfoPageBuffer->lcdinput[0] == 0x1B50)
		{
			memset(S_SetDeviceInfoPageBuffer->user.user_job, 0, MaxJobLen);
			memcpy(S_SetDeviceInfoPageBuffer->user.user_job, &pbuf[7], GetBufLen(&pbuf[7] , 2*pbuf[6]));
			S_SetDeviceInfoPageBuffer->ismodify = 1;
		}
		/*��ע*/
		else if(S_SetDeviceInfoPageBuffer->lcdinput[0] == 0x1B60)
		{
			memset(S_SetDeviceInfoPageBuffer->user.user_desc, 0, MaxDescLen);
			memcpy(S_SetDeviceInfoPageBuffer->user.user_desc, &pbuf[7], GetBufLen(&pbuf[7] , 2*pbuf[6]));
			S_SetDeviceInfoPageBuffer->ismodify = 1;
		}
		/*�豸ʹ�õ�ַ*/
		else if(S_SetDeviceInfoPageBuffer->lcdinput[0] == 0x1B70)
		{
			memset(S_SetDeviceInfoPageBuffer->deviceunit, 0, MaxDeviceUnitLen);
			memcpy(S_SetDeviceInfoPageBuffer->deviceunit, &pbuf[7], GetBufLen(&pbuf[7] , 2*pbuf[6]));
			S_SetDeviceInfoPageBuffer->ismodify = 1;
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
	if(S_SetDeviceInfoPageBuffer)
	{

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
	SelectPage(102);
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
	if(NULL == S_SetDeviceInfoPageBuffer)
	{
		S_SetDeviceInfoPageBuffer = MyMalloc(sizeof(SetDeviceInfoPageBuffer));
		
		if(S_SetDeviceInfoPageBuffer)
		{
			memset(S_SetDeviceInfoPageBuffer, 0, sizeof(SetDeviceInfoPageBuffer));
	
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
	MyFree(S_SetDeviceInfoPageBuffer);
	S_SetDeviceInfoPageBuffer = NULL;
}

static void showDeviceInfoText(void)
{
	sprintf(S_SetDeviceInfoPageBuffer->tempBuf, "%s\0", S_SetDeviceInfoPageBuffer->user.user_name);
	DisText(0x1b10, S_SetDeviceInfoPageBuffer->tempBuf, strlen(S_SetDeviceInfoPageBuffer->tempBuf)+1);

	sprintf(S_SetDeviceInfoPageBuffer->tempBuf, "%s\0", S_SetDeviceInfoPageBuffer->user.user_age);
	DisText(0x1b20, S_SetDeviceInfoPageBuffer->tempBuf, strlen(S_SetDeviceInfoPageBuffer->tempBuf)+1);

	sprintf(S_SetDeviceInfoPageBuffer->tempBuf, "%s\0", S_SetDeviceInfoPageBuffer->user.user_sex);
	DisText(0x1b30, S_SetDeviceInfoPageBuffer->tempBuf, strlen(S_SetDeviceInfoPageBuffer->tempBuf)+1);

	sprintf(S_SetDeviceInfoPageBuffer->tempBuf, "%s\0", S_SetDeviceInfoPageBuffer->user.user_phone);
	DisText(0x1b40, S_SetDeviceInfoPageBuffer->tempBuf, strlen(S_SetDeviceInfoPageBuffer->tempBuf)+1);
	
	sprintf(S_SetDeviceInfoPageBuffer->tempBuf, "%s\0", S_SetDeviceInfoPageBuffer->user.user_job);
	DisText(0x1b50, S_SetDeviceInfoPageBuffer->tempBuf, strlen(S_SetDeviceInfoPageBuffer->tempBuf)+1);

	sprintf(S_SetDeviceInfoPageBuffer->tempBuf, "%s\0", S_SetDeviceInfoPageBuffer->user.user_desc);
	DisText(0x1b60, S_SetDeviceInfoPageBuffer->tempBuf, strlen(S_SetDeviceInfoPageBuffer->tempBuf)+1);
	
	sprintf(S_SetDeviceInfoPageBuffer->tempBuf, "%s\0", S_SetDeviceInfoPageBuffer->deviceunit);
	DisText(0x1b70, S_SetDeviceInfoPageBuffer->tempBuf, strlen(S_SetDeviceInfoPageBuffer->tempBuf)+1);
}

#endif
