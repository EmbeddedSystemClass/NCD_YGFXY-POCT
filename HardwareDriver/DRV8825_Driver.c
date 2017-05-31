/***************************************************************************************************
*FileName��DRV8825_Driver
*Description���������
*Author��xsx
*Data��2016��4��23��11:03:11
***************************************************************************************************/

/***************************************************************************************************/
/******************************************ͷ�ļ�***************************************************/
/***************************************************************************************************/

#include	"DRV8825_Driver.h"
#include	"System_Data.h"
#include	"CardLimit_Driver.h"


/***************************************************************************************************/
/**************************************�ֲ���������*************************************************/
/***************************************************************************************************/

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
*FunctionName��DRV_Init
*Description�����IO��ʼ��
*Input��None
*Output��None
*Author��xsx
*Data��2016��4��23��11:30:52
***************************************************************************************************/
void DRV_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(DRV_Power_RCC | DRV_Dir_RCC | DRV_Sleep_RCC, ENABLE); /*ʹ��LED��ʹ�õ�GPIOʱ��*/
	
	GPIO_InitStructure.GPIO_Pin = DRV_Power_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(DRV_Power_Group, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = DRV_Sleep_Pin; 
	GPIO_Init(DRV_Sleep_Group, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = DRV_Dir_Pin; 
	GPIO_Init(DRV_Dir_Group, &GPIO_InitStructure);
	
	SetDRVSleepStatues(OnLine);
	
	SetDRVPowerStatues(NonamalPower);
	
	SetDRVDir(Reverse);

}

/***************************************************************************************************
*FunctionName��SetDRVSleepStatues
*Description�����õ�����߻�������
*Input��None
*Output��None
*Author��xsx
*Data��2016��4��27��08:47:37
***************************************************************************************************/
void SetDRVSleepStatues(DRVSleep statues)
{
	if(statues)
	{
		if(PreLimited)
			GPIO_ResetBits(DRV_Sleep_Group, DRV_Sleep_Pin);
		else
			GPIO_SetBits(DRV_Sleep_Group, DRV_Sleep_Pin);	
	}
	else
	{
		if(PreLimited)
			GPIO_SetBits(DRV_Sleep_Group, DRV_Sleep_Pin);
		else
			GPIO_ResetBits(DRV_Sleep_Group, DRV_Sleep_Pin);	
	}
}

/***************************************************************************************************
*FunctionName��SetDRVSleepStatues
*Description�����õ�����߻�������
*Input��None
*Output��None
*Author��xsx
*Data��2016��4��27��08:47:37
***************************************************************************************************/
void SetDRVPowerStatues(DRVPower statues)
{
	if(statues)
		GPIO_ResetBits(DRV_Power_Group, DRV_Power_Pin);
	else
		GPIO_SetBits(DRV_Power_Group, DRV_Power_Pin);
}

/***************************************************************************************************
*FunctionName��SetDRVDir
*Description�����õ������
*Input��None
*Output��None
*Author��xsx
*Data��2016��4��27��08:52:43
***************************************************************************************************/
void SetDRVDir(DRVDir value)
{
	if(value)
		GPIO_ResetBits(DRV_Dir_Group, DRV_Dir_Pin);
	else
		GPIO_SetBits(DRV_Dir_Group, DRV_Dir_Pin);
	
	setSystemMotorDir(value);
}

/***************************************************************************************************
*FunctionName: motorDisable
*Description: ʧ�ܵ��
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2017��3��10��10:39:34
***************************************************************************************************/
void motorDisable(void)
{
	GPIO_ResetBits(DRV_Sleep_Group, DRV_Sleep_Pin);
}



