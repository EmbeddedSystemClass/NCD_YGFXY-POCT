/***************************************************************************************************
*FileName��OutModel_Fun
*Description���Ŷ�ģ�鹦��
*Author��xsx
*Data��2016��3��17��17:11:23
***************************************************************************************************/

/***************************************************************************************************/
/******************************************ͷ�ļ�***************************************************/
/***************************************************************************************************/

#include	"OutModel_Fun.h"
#include	"System_Data.h"
#include	"MyMem.h"


#include 	"task.h"
#include 	"queue.h"

#include	"stdio.h"
#include	"string.h"
/***************************************************************************************************/
/**************************************�ֲ���������*************************************************/
/***************************************************************************************************/
static ModelData_DefType GB_S_ModelData = 
	{
		.OneModel_Data = 
			{
				{
					.LED_Statues = R_OFF_G_ON,
					.Time = 0
				},
				{
					.LED_Statues = R_OFF_G_ON,
					.Time = 0
				},
				{
					.LED_Statues = R_OFF_G_ON,
					.Time = 0
				},
				{
					.LED_Statues = R_OFF_G_ON,
					.Time = 0
				},
				{
					.LED_Statues = R_OFF_G_ON,
					.Time = 0
				},
				{
					.LED_Statues = R_OFF_G_ON,
					.Time = 0
				},
				{
					.LED_Statues = R_OFF_G_ON,
					.Time = 0
				},
				{
					.LED_Statues = R_OFF_G_ON,
					.Time = 0
				},
				{
					.LED_Statues = R_OFF_G_ON,
					.Time = 0
				}
			},
		.T_Count = 1
	};

/***************************************************************************************************/
/**************************************�ֲ���������*************************************************/
/***************************************************************************************************/

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************����********************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************
*FunctionName��UpOneModelData
*Description������һ���Ŷ�λ��״̬
*Input��modelindex -- �Ŷ�λ���
		ledstatues -- ��״̬
		time -- ��˸���ʱ��
*Output��None
*Author��xsx
*Data��2016��3��17��17:31:54
***************************************************************************************************/
void UpOneModelData(unsigned char modelindex, TM1623_LED_State ledstatues, TM1623_LED_State ledOffStatues, unsigned char time)
{
	GB_S_ModelData.OneModel_Data[modelindex].LED_Statues = ledstatues;
	GB_S_ModelData.OneModel_Data[modelindex].LEN_OffStatus = ledOffStatues;
	GB_S_ModelData.OneModel_Data[modelindex].Time = time;
}

/***************************************************************************************************
*FunctionName��GetGBModelData
*Description����ȡ��ǰ�Ŷ�ģ��״̬
*Input��None
*Output��None
*Author��xsx
*Data��2016��3��17��17:38:20
***************************************************************************************************/
ModelData_DefType *GetGBModelData(void)
{
	return &GB_S_ModelData;
}


TM1623_KEY_State GetKeyStatues(unsigned char index)
{
	return GB_S_ModelData.OneModel_Data[index].Key_Statues;
}
/***************************************************************************************************
*FunctionName��OutModel_Init
*Description����ʼ���Ŷ�ģ��
*Input��None
*Output��None
*Author��xsx
*Data��2016��3��17��17:39:33
***************************************************************************************************/
void OutModel_Init(void)
{
	unsigned char i=0;
	for(i=0; i<9; i++)
	{
		UpOneModelData(i, R_OFF_G_ON, R_OFF_G_OFF, 0);
	}
	GB_S_ModelData.T_Count = 1;
}

/***************************************************************************************************
*FunctionName��ToggleLedStatues
*Description������led״̬
*Input��None
*Output��None
*Author��xsx
*Data��2016��3��17��18:03:39
***************************************************************************************************/
void ToggleLedStatues(unsigned char ledindex)
{
	if(GB_S_ModelData.OneModel_Data[ledindex].LED_Statues == GetLedStatues(ledindex))
		ChangeTM1623LedStatues(ledindex, GB_S_ModelData.OneModel_Data[ledindex].LEN_OffStatus);		//��
	else
		ChangeTM1623LedStatues(ledindex, GB_S_ModelData.OneModel_Data[ledindex].LED_Statues);		//��
}

/***************************************************************************************************
*FunctionName��
*Description��
*Input��None
*Output��None
*Author��xsx
*Data��
***************************************************************************************************/
void ChangeOutModelStatues(void)
{
	static unsigned char i=0;
	
	/*��ȡ����״̬*/
	TM1623_ReadKey();
	
	if(0xff == getTM1623KeyData(4))
		setSystemPaiduiModuleStatus(Connect_Error);
	else
		setSystemPaiduiModuleStatus(Connect_Ok);
	
	/*����led״̬*/
	for(i=0; i<8; i++)
	{
		if(GB_S_ModelData.OneModel_Data[i].Time == 0)
			ChangeTM1623LedStatues(i, GB_S_ModelData.OneModel_Data[i].LED_Statues);
		else if((GB_S_ModelData.T_Count % GB_S_ModelData.OneModel_Data[i].Time) == 0)
		{
			if((GB_S_ModelData.T_Count % 2) == 0)
				ChangeTM1623LedStatues(i, GB_S_ModelData.OneModel_Data[i].LEN_OffStatus);
			else
				ChangeTM1623LedStatues(i, GB_S_ModelData.OneModel_Data[i].LED_Statues);
		}
		
		GB_S_ModelData.OneModel_Data[i].Key_Statues = GetTheKeyStatues(i);
	}

	GB_S_ModelData.T_Count++;
}

