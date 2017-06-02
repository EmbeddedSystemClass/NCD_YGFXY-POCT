/******************************************************************************************/
/*****************************************ͷ�ļ�*******************************************/

#include	"WifiSetPage.h"

#include	"LCD_Driver.h"
#include	"MyMem.h"
#include	"WifiFunction.h"
#include	"NetPreSetPage.h"
#include	"WifiDao.h"
#include	"SleepPage.h"

#include 	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"

#include	<string.h>
#include	"stdio.h"

/******************************************************************************************/
/*****************************************�ֲ���������*************************************/
static WifiPageBuffer * S_WifiPageBuffer = NULL;
/******************************************************************************************/
/*****************************************�ֲ���������*************************************/
static void RefreshWifi(void);
static void DisListText(void);
static void CheckIsNeedKey(void);
static MyState_TypeDef connectWifiFun(void);

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
*FunctionName: createSelectUserActivity
*Description: ����ѡ������˽���
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016��12��21��09:00:09
***************************************************************************************************/
MyState_TypeDef createWifiSetActivity(Activity * thizActivity, Intent * pram)
{
	if(NULL == thizActivity)
		return My_Fail;
	
	if(My_Pass == activityBufferMalloc())
	{
		InitActivity(thizActivity, "WifiSetActivity\0", activityStart, activityInput, activityFresh, activityHide, activityResume, activityDestroy);
		
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
	if(S_WifiPageBuffer)
	{
		SelectPage(112);
		
		RefreshWifi();
		
		DisListText();
	}
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
	if(S_WifiPageBuffer)
	{
		/*����*/
		S_WifiPageBuffer->lcdinput[0] = pbuf[4];
		S_WifiPageBuffer->lcdinput[0] = (S_WifiPageBuffer->lcdinput[0]<<8) + pbuf[5];

		/*�����������wifi*/
		if(S_WifiPageBuffer->lcdinput[0] == 0x1E70)
		{
			SendKeyCode(4);

			memcpy(S_WifiPageBuffer->wifip->key, &pbuf[7], GetBufLen(&pbuf[7] , 2*pbuf[6]));
			if(My_Fail == ConnectWifi(S_WifiPageBuffer->wifip))
			{
				SendKeyCode(16);
				vTaskDelay(100/portTICK_RATE_MS);
				SendKeyCode(2);
			}
			else
			{
				SendKeyCode(16);
				vTaskDelay(100/portTICK_RATE_MS);
				SendKeyCode(1);
				SaveWifiData(S_WifiPageBuffer->wifip);
			}
		}
		/*��һҳ*/
		else if(S_WifiPageBuffer->lcdinput[0] == 0x1E52)
		{
			if(S_WifiPageBuffer->pageindex > 0)
			{
				S_WifiPageBuffer->pageindex--;
				DisListText();
			}
		}
		/*��һҳ*/
		else if(S_WifiPageBuffer->lcdinput[0] == 0x1E53)
		{
			if(S_WifiPageBuffer->pageindex < (MaxWifiListNum/PageWifiNum-1))
			{
				if(strlen(S_WifiPageBuffer->wifilist[(S_WifiPageBuffer->pageindex+1)*PageWifiNum].ssid) > 0)
				{
					S_WifiPageBuffer->pageindex++;
					DisListText();
				}
			}
		}
		/*����*/
		else if(S_WifiPageBuffer->lcdinput[0] == 0x1E50)
		{
			RestartWifi();
			
			backToFatherActivity();
		}
		/*ˢ��*/
		else if(S_WifiPageBuffer->lcdinput[0] == 0x1E51)
		{
			RefreshWifi();
			DisListText();
		}
		/*ѡ��wifi*/
		else if((S_WifiPageBuffer->lcdinput[0] >= 0x1E58)&&(S_WifiPageBuffer->lcdinput[0] <= 0x1E5F))
		{
			/*�ж�ѡ���wifi�Ƿ񳬳�����Ч�б�*/
			if(strlen(S_WifiPageBuffer->wifilist[(S_WifiPageBuffer->pageindex)*PageWifiNum+(S_WifiPageBuffer->lcdinput[0] - 0x1E58)].ssid) > 0)
			{
				S_WifiPageBuffer->selectindex = (S_WifiPageBuffer->lcdinput[0] - 0x1E58)+1;
				CheckIsNeedKey();
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
	if(S_WifiPageBuffer)
	{

	}
	
	SelectPage(112);
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
	
	//����wifi�������ϴ�����ģʽ
	giveWifixMutex();
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
	if(NULL == S_WifiPageBuffer)
	{
		S_WifiPageBuffer = MyMalloc(sizeof(WifiPageBuffer));
		
		if(S_WifiPageBuffer)
		{
			memset(S_WifiPageBuffer, 0, sizeof(WifiPageBuffer));
	
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
	MyFree(S_WifiPageBuffer);
	S_WifiPageBuffer = NULL;
}

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
static void RefreshWifi(void)
{	
	vTaskDelay(200 / portTICK_RATE_MS);
	SendKeyCode(5);
	
	S_WifiPageBuffer->pageindex = 0;
	S_WifiPageBuffer->selectindex = 0;
	
	if(S_WifiPageBuffer->isGetWifiControl == false)
	{
		if(My_Fail == takeWifiMutex(10000 / portTICK_RATE_MS))
		{
			SendKeyCode(6);
			return;
		}
		else
			S_WifiPageBuffer->isGetWifiControl = true;
	}
	
	/*�������atģʽ�������atģʽ*/
	if(My_Fail == SetWifiWorkInAT(AT_Mode))
	{
		SendKeyCode(17);
		return;
	}
	
	memset(S_WifiPageBuffer->wifilist, 0, sizeof(WIFI_Def)*MaxWifiListNum);
		
	ScanApList(S_WifiPageBuffer->wifilist);
		
	SendKeyCode(17);

}

/*ˢ���б�����*/
static void DisListText(void)
{
	unsigned char i=0;
	
	S_WifiPageBuffer->wifinum = 0;
	S_WifiPageBuffer->wifip = &(S_WifiPageBuffer->wifilist[S_WifiPageBuffer->pageindex*PageWifiNum]);
	
	for(i=0; i<PageWifiNum; i++)
	{
		if(strlen(S_WifiPageBuffer->wifip->ssid) > 0)
		{
			sprintf(S_WifiPageBuffer->buf, "%s\0", S_WifiPageBuffer->wifip->ssid);
			
			S_WifiPageBuffer->myico[i].X = 600;
			S_WifiPageBuffer->myico[i].Y = 142+i*40;
				
			if(S_WifiPageBuffer->wifip->indicator <= 10)
				S_WifiPageBuffer->myico[i].ICO_ID = 25;
			else if(S_WifiPageBuffer->wifip->indicator < 40)
				S_WifiPageBuffer->myico[i].ICO_ID = 26;
			else if(S_WifiPageBuffer->wifip->indicator < 70)
				S_WifiPageBuffer->myico[i].ICO_ID = 27;
			else
				S_WifiPageBuffer->myico[i].ICO_ID = 28;
			
			S_WifiPageBuffer->wifinum = i+1;
			S_WifiPageBuffer->wifip++;
		}
		else
			sprintf(S_WifiPageBuffer->buf, "\0");
		
		DisText(0x1E80+i*0x10, S_WifiPageBuffer->buf, strlen(S_WifiPageBuffer->buf)+1);
	}
	
	BasicUI(0x1F00 ,0x1807 , S_WifiPageBuffer->wifinum, S_WifiPageBuffer->myico , sizeof(Basic_ICO)*S_WifiPageBuffer->wifinum);
}


static void CheckIsNeedKey(void)
{
	static char *security = NULL;
	
	if(S_WifiPageBuffer)
	{
		if((S_WifiPageBuffer->selectindex <= 0)||(S_WifiPageBuffer->selectindex > PageWifiNum))
			return;
		
		S_WifiPageBuffer->wifip = &(S_WifiPageBuffer->wifilist[(S_WifiPageBuffer->pageindex)*PageWifiNum+S_WifiPageBuffer->selectindex-1]);
		
		security = strstr(S_WifiPageBuffer->wifip->auth, "OPEN");
		/*����Ҫ����*/
		if(security)
		{
			SendKeyCode(4);

			if(My_Fail == ConnectWifi(S_WifiPageBuffer->wifip))
			{
				SendKeyCode(16);
				vTaskDelay(100 / portTICK_RATE_MS);
				SendKeyCode(2);
			}
			else
			{
				SendKeyCode(16);
				vTaskDelay(100 / portTICK_RATE_MS);
				SendKeyCode(1);	
			}
		}
		/*�ѱ�������*/
		else if(My_Pass == ReadWifiData(S_WifiPageBuffer->wifip))
		{
			SendKeyCode(4);

			if(My_Fail == ConnectWifi(S_WifiPageBuffer->wifip))
			{
				SendKeyCode(16);
				vTaskDelay(100 / portTICK_RATE_MS);
				SendKeyCode(3);
				deleteWifi(S_WifiPageBuffer->wifip);
			}
			else
			{
				SendKeyCode(16);
				vTaskDelay(100 / portTICK_RATE_MS);
				SendKeyCode(1);	
			}
		}
		/*��������*/
		else
			SendKeyCode(3);
	}
}

static MyState_TypeDef connectWifiFun(void)
{
	SendKeyCode(4);

	if(My_Fail == ConnectWifi(S_WifiPageBuffer->wifip))
	{
		SendKeyCode(16);
		vTaskDelay(100 / portTICK_RATE_MS);
		SendKeyCode(2);
	}
	else
	{
		SendKeyCode(16);

		SendKeyCode(1);
		
	}
}
