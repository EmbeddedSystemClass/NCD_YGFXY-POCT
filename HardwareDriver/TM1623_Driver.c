
/***************************************************************************************************
*FileName��TM1623_Driver
*Description���Ŷӹ���ģ������
*Author��xsx
*Data��2016��3��12��14:31:46
***************************************************************************************************/

/***************************************************************************************************/
/******************************************ͷ�ļ�***************************************************/
/***************************************************************************************************/
#include	"TM1623_Driver.h"
#include 	"delay.h"

/***************************************************************************************************/
/**************************************�ֲ���������*************************************************/
/***************************************************************************************************/
/*
	��ʾLED�Ƶ�״̬��ÿ��LED �ֺ�ɫ����ɫ��ռ2λ
	00  ��Ϩ�� 
	01	��������̵���
	10	�̵����������
	11	����
	3�ֽڹ�24λ���ɱ�ʾ12��˫ɫLED��
	0��1λ��ʾLED1���Դ�����
*/
static unsigned int GB_S_TM1623LEDState = 0;

static unsigned char GB_S_TM1623KEYState[5] = {0, 0, 0, 0, 0};
/***************************************************************************************************/
/**************************************�ֲ���������*************************************************/
/***************************************************************************************************/
static void TM1623_SDA_IN(void);
static void TM1623_SDA_OUT(void);
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************����********************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************
*FunctionName��TM1623_Init
*Description���Ŷӹ���ģ��GPIO��ʼ��
*Input��None
*Output��None
*Author��xsx
*Data��2016��3��12��14:32:15
***************************************************************************************************/
void TM1623_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(TM1623_SCK_Rcc | TM1623_STB_Rcc | TM1623_SDA_Rcc, ENABLE);//ʹ��GPIOBʱ��

	//GPIOB8,B9��ʼ������
	GPIO_InitStructure.GPIO_Pin = TM1623_SCK_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_Init(TM1623_SCK_Group, &GPIO_InitStructure);//��ʼ��
	
	GPIO_InitStructure.GPIO_Pin = TM1623_SDA_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_Init(TM1623_SDA_Group, &GPIO_InitStructure);//��ʼ��
	
	GPIO_InitStructure.GPIO_Pin = TM1623_STB_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_Init(TM1623_STB_Group, &GPIO_InitStructure);//��ʼ��
	
}
/***************************************************************************************************
*FunctionName��TM1623_SDA_IN
*Description���Ŷӹ���ģ��ͨ��SDA������
*Input��None
*Output��None
*Author��xsx
*Data��2016��3��12��14:33:11
***************************************************************************************************/
static void TM1623_SDA_IN(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	//GPIOB8,B9��ʼ������
	GPIO_InitStructure.GPIO_Pin = TM1623_SDA_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_Init(TM1623_SDA_Group, &GPIO_InitStructure);//��ʼ��
}
/***************************************************************************************************
*FunctionName��TM1623_SDA_OUT
*Description���Ŷӹ���ģ��ͨ��SDA�����
*Input��None
*Output��None
*Author��xsx
*Data��2016��3��12��14:33:23
***************************************************************************************************/
static void TM1623_SDA_OUT(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	//GPIOB8,B9��ʼ������
	GPIO_InitStructure.GPIO_Pin = TM1623_SDA_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_Init(TM1623_SDA_Group, &GPIO_InitStructure);//��ʼ��
}

/***************************************************************************************************
*FunctionName��WriteByte
*Description���Ŷӹ���ģ�飬дһ���ֽ�
*Input��data -- д�������
*Output��None
*Author��xsx
*Data��2016��3��12��14:34:15
***************************************************************************************************/
void TM1623_WriteByte(unsigned char data)
{
	unsigned char i=0;
	
	for(i=0; i<8; i++)
	{
		if(data & 0x01)
			GPIO_SetBits(TM1623_SDA_Group, TM1623_SDA_Pin);
		else
			GPIO_ResetBits(TM1623_SDA_Group, TM1623_SDA_Pin);
			
		delay_us(5);
		GPIO_SetBits(TM1623_SCK_Group, TM1623_SCK_Pin);
		delay_us(5);

		GPIO_ResetBits(TM1623_SCK_Group, TM1623_SCK_Pin);
		delay_us(5);

		data >>= 1;
	}
}

/***************************************************************************************************
*FunctionName��TM1623_Send_CMD
*Description������������Ŷ�ģ��
*Input��cmd -- ���͵�����
*Output��None
*Author��xsx
*Data��2016��3��12��14:38:39
***************************************************************************************************/
void TM1623_Send_CMD(unsigned char cmd)
{
	GPIO_SetBits(TM1623_STB_Group, TM1623_STB_Pin);
	delay_us(5);
	
	GPIO_ResetBits(TM1623_STB_Group, TM1623_STB_Pin);
	delay_us(5);
	
	TM1623_WriteByte(cmd);
}

/***************************************************************************************************
*FunctionName��TM1623_ReadKey
*Description����ȡ������Ϣ
*Input��None
*Output��None
*Author��xsx
*Data��2016��3��12��14:44:16
***************************************************************************************************/
void TM1623_ReadKey(void)
{
	unsigned char i=0,j;
	unsigned char data = 0;	
	
	
	TM1623_Send_CMD(0x03); 

	TM1623_Send_CMD(0x42);
	delay_us(5);
	TM1623_SDA_IN();
	
	for(j=0; j<5; j++)
	{
		data = 0;
		
		for(i=0; i<8; i++)
		{
			data = data >> 1;
			GPIO_ResetBits(TM1623_SCK_Group, TM1623_SCK_Pin);
			delay_us(5);
			GPIO_SetBits(TM1623_SCK_Group, TM1623_SCK_Pin);
			delay_us(5);
			
			if(GPIO_ReadInputDataBit(TM1623_SDA_Group, TM1623_SDA_Pin))
			{
				data = data|0x80;
			}
		}
		GB_S_TM1623KEYState[j] = data;
	}
	TM1623_SDA_OUT();
}

/***************************************************************************************************
*FunctionName��ChangeTM1623LedStatues
*Description������ָ��״̬д���Ŷ�ģ��
*Input��ledindex -- �Ʊ��
*		statues -- ��Ӧ��ŵ�״̬
*Output��None
*Author��xsx
*Data��2016��3��12��16:40:34
***************************************************************************************************/
void ChangeTM1623LedStatues(unsigned char ledindex, TM1623_LED_State statues)
{
	unsigned int temp = statues;
	
	if(ledindex >   12)
		return;
	
	TM1623_Send_CMD(0x03);
	TM1623_Send_CMD(0x44);
	
	TM1623_Send_CMD(0xc0+(ledindex/2)*2);
	
	if((statues & 0x01) == 0)
		GB_S_TM1623LEDState &= ~(((unsigned int)1)<<(ledindex*2));
	else
		GB_S_TM1623LEDState |= (((unsigned int)1)<<(ledindex*2));
	
	if(((statues & 0x02)>>1) == 0)
		GB_S_TM1623LEDState &= ~(((unsigned int)1)<<(ledindex*2+1));
	else
		GB_S_TM1623LEDState |= (((unsigned int)1)<<(ledindex*2+1));
	
	temp = (unsigned char)((GB_S_TM1623LEDState>>(ledindex/2*4))&0x0f);
	temp <<= 4;

	TM1623_WriteByte(temp);
	
	TM1623_Send_CMD(0x8f);
}
/***************************************************************************************************
*FunctionName��GetLedStatues
*Description����ȡһ��LED��״̬
*Input��None
*Output��None
*Author��xsx
*Data��2016��3��17��17:54:58
***************************************************************************************************/
TM1623_LED_State GetLedStatues(unsigned char ledindex)
{
	unsigned char temp = 0;
	
	temp = (unsigned char)((GB_S_TM1623LEDState>>(ledindex*2))&0x03);

	return (TM1623_LED_State)temp;
}


/***************************************************************************************************
*FunctionName��
*Description��
*Input��None
*Output��None
*Author��xsx
*Data��
***************************************************************************************************/
TM1623_KEY_State GetTheKeyStatues(unsigned char keyindex)
{
	/*9������ֻ��ʹ��ǰ2���ֽ�*/
	unsigned char tempkeydata = GB_S_TM1623KEYState[keyindex/4];
	
	if(keyindex > 3)
		keyindex -= 4;
	
	tempkeydata >>= ((keyindex/2)+ keyindex);
	
	tempkeydata = tempkeydata&0x01;
	
	return (TM1623_KEY_State)tempkeydata;
}

/***************************************************************************************************
*FunctionName:  getTM1623KeyData
*Description:  ��ȡ��1623��ȡ������
*Input:  
*Output:  
*Return:  
*Author:  xsx
*Date: 2017��3��27�� 15:00:19
***************************************************************************************************/
unsigned char getTM1623KeyData(unsigned char index)
{
	return GB_S_TM1623KEYState[index];
}



