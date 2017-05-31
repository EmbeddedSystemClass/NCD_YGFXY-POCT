/***************************************************************************************************
*FileName:TLV5617_Driver
*Description:����ģ�����������Ʒ������Ⱥͻ��߸߶�
*Author:xsx
*Data:2016��4��22��16:33:12
***************************************************************************************************/

/***************************************************************************************************/
/******************************************ͷ�ļ�***************************************************/
/***************************************************************************************************/
#include	"TLV5617_Driver.h"
#include	"SPI1_Driver.h"

#include	"Delay.h"
/***************************************************************************************************/
/**************************************�ֲ���������*************************************************/
/***************************************************************************************************/
static unsigned short GB_LedValue = 0;						//�������������
static unsigned short GB_CLineValue = 0;					//���߸߶�
/***************************************************************************************************/
/**************************************�ֲ���������*************************************************/
/***************************************************************************************************/
static void DA_Write(unsigned short data);
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************����********************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

void DA_IO_Init(void)
{
    GPIO_InitTypeDef    GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(DA_SCK_Rcc | DA_SDA_Rcc | DA_CS_Rcc, ENABLE);

	GPIO_InitStructure.GPIO_Pin = DA_SCK_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
    GPIO_Init(DA_SCK_Group, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = DA_SDA_Pin;
    GPIO_Init(DA_SDA_Group, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = DA_CS_Pin;
    GPIO_Init(DA_CS_Group, &GPIO_InitStructure);
	
}


static void DA_Write(unsigned short data)
{
	unsigned char i;
	
	DA_SCK_H();
	delay_us(5);
	
	for(i=0; i<16; i++)
    {
		if(data & ((unsigned short)0x8000))
			DA_SDA_H();
        else
			DA_SDA_L();
		delay_us(5);
	
		data <<= 1;
		
		DA_SCK_L();
		delay_us(5);
		
		DA_SCK_H();
		delay_us(5);
    }

}
/***************************************************************************************************
*FunctionName��GetGB_LedValue
*Description����ȡ��ǰ�������������
*Input��None
*Output��None
*Author��xsx
*Data��2016��4��22��18:19:48
***************************************************************************************************/
unsigned short GetGB_LedValue(void)
{
	return GB_LedValue;
}

/***************************************************************************************************
*FunctionName��SetGB_LedValue
*Description�����÷������������ 0 - 1023,ʵ�ʲ��������320
*Input��None
*Output��None
*Author��xsx
*Data��2016��4��22��18:19:29
***************************************************************************************************/
void SetGB_LedValue(unsigned short value)
{
	unsigned short temp = value;
	
	
	DA_CS_H();
	delay_us(10);
	
	DA_CS_L();
	delay_us(50);
	
	GB_LedValue = value;
	
	//��2λΪ0
	temp <<= 2;					
	
	/*����DAC Bֵ*/
	temp &= ~((unsigned short)1<<(15));
	temp &= ~((unsigned short)1<<(12));
	
	/*����ģʽ*/
	temp &= ~((unsigned short)1<<(14));
	
	/*����ģʽ*/
	temp &= ~((unsigned short)1<<(13));
	
	DA_Write(temp);
	
	delay_us(50);
	DA_CS_H();
}

/***************************************************************************************************
*FunctionName��GetGB_CLineValue
*Description����ȡ��ǰ���߸߶�ֵ 0 - 1023
*Input��None
*Output��None
*Author��xsx
*Data��2016��4��22��18:19:12
***************************************************************************************************/
unsigned short GetGB_CLineValue(void)
{
	return GB_CLineValue;
}

/***************************************************************************************************
*FunctionName��SetGB_CLineValue
*Description�����û��߸߶� 0-1023�� ���1023��ʹ�������5V
*Input��value -- ��12λ��Чλ
*Output��None
*Author��xsx
*Data��2016��4��22��18:18:55
***************************************************************************************************/
void SetGB_CLineValue(unsigned short value)
{
	unsigned short temp = value;
	
	DA_CS_H();
	delay_us(10);
	
	DA_CS_L();
	delay_us(50);
	
	GB_CLineValue = value;
	
	//��2λΪ0
	temp <<= 2;					
	
	/*����DAC Bֵ*/
	temp |= ((unsigned short)1<<(15));
	temp &= ~((unsigned short)1<<(12));
	
	/*����ģʽ*/
	temp &= ~((unsigned short)1<<(14));
	
	/*����ģʽ*/
	temp &= ~((unsigned short)1<<(13));

	DA_Write(temp);
	
	delay_us(50);
	DA_CS_H();
}
