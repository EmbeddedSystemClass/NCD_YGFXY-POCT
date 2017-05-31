/***************************************************************************************************
*FileName��Wifi_Driver
*Description��wifiģ��Ķ˿ڳ�ʼ��
*Author��xsx
*Data��2016��5��9��15:00:15
***************************************************************************************************/

/***************************************************************************************************/
/******************************************ͷ�ļ�***************************************************/
/***************************************************************************************************/

#include	"Wifi_Driver.h"

#include	"Delay.h"

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
*FunctionName��Wifi_GPIOInit
*Description����ʼ��ϵͳָʾ�Ƶ�IO
*Input��None
*Output��None
*Author��xsx
*Data��2015��8��26��14:34:45
***************************************************************************************************/
void Wifi_GPIOInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(WPS_Rcc | Reload_Rcc | Reset_Rcc, ENABLE);

  	GPIO_InitStructure.GPIO_Pin = WPS_Pin; 
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(WPS_GpioGroup, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = Reload_Pin; 
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(Reload_GpioGroup, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = Reset_Pin; 
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(Reset_GpioGroup, &GPIO_InitStructure);

	GPIO_WriteBit(Reload_GpioGroup, Reload_Pin, Bit_SET);
	
	WifiReset();
}


void WifiReset(void)
{
	GPIO_WriteBit(Reset_GpioGroup, Reset_Pin, Bit_RESET);
	
	delay_ms(50);
	
	GPIO_WriteBit(Reset_GpioGroup, Reset_Pin, Bit_SET);
}


	




