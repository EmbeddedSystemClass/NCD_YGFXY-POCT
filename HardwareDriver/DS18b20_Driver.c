/***************************************************************************************************
*FileName:	DS18b20_Driver
*Description:	18b20��������
*Author: xsx_kair
*Data:	2017��1��18��11:04:09
***************************************************************************************************/

/***************************************************************************************************/
/******************************************Header List********************************************/
/***************************************************************************************************/

#include	"DS18b20_Driver.h"
#include	"Delay.h"

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
const static unsigned char CrcTable[256] =
{
	  0,  94, 188, 226,  97,  63, 221, 131, 194, 156, 126,  32, 163, 253,  31,  65,
	157, 195,  33, 127, 252, 162,  64,  30,  95,   1, 227, 189,  62,  96, 130, 220,
	 35, 125, 159, 193,  66,  28, 254, 160, 225, 191,  93,   3, 128, 222,  60,  98,
	190, 224,   2,  92, 223, 129,  99,  61, 124,  34, 192, 158,  29,  67, 161, 255,
	 70,  24, 250, 164,  39, 121, 155, 197, 132, 218,  56, 102, 229, 187,  89,   7,
	219, 133, 103,  57, 186, 228,   6,  88,  25,  71, 165, 251, 120,  38, 196, 154,
	101,  59, 217, 135,   4,  90, 184, 230, 167, 249,  27,  69, 198, 152, 122,  36,
	248, 166,  68,  26, 153, 199,  37, 123,  58, 100, 134, 216,  91,   5, 231, 185,
	140, 210,  48, 110, 237, 179,  81,  15,  78,  16, 242, 172,  47, 113, 147, 205,
	 17,  79, 173, 243, 112,  46, 204, 146, 211, 141, 111,  49, 178, 236,  14,  80,
	175, 241,  19,  77, 206, 144, 114,  44, 109,  51, 209, 143,  12,  82, 176, 238,
	 50, 108, 142, 208,  83,  13, 239, 177, 240, 174,  76,  18, 145, 207,  45, 115,
	202, 148, 118,  40, 171, 245,  23,  73,   8,  86, 180, 234, 105,  55, 213, 139,
	 87,   9, 235, 181,  54, 104, 138, 212, 149, 203,  41, 119, 244, 170,  72,  22,
	233, 183,  85,  11, 136, 214,  52, 106,  43, 117, 151, 201,  74,  20, 246, 168,
	116,  42, 200, 150,  21,  75, 169, 247, 182, 232,  10,  84, 215, 137, 107,  53,
};
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
static void DS18B20_GPIO_Config(void);
static void DS18B20_Config(void);
static void DS18B20_Mode_IPU(void);
static void DS18B20_Mode_Out_PP(void);
static bool DS18B20_Rst(void);
static void DS18B20_Write_Byte(const unsigned char dat);
static unsigned char DS18B20_Read_Byte(void);
static void DS18B20_Read_Bytes(unsigned char *buf, unsigned char len);
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/****************************************File Start*************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************
*FunctionName: DS18B20_Init
*Description: ��ʼ��18b20
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2017��1��18��11:13:23
***************************************************************************************************/
void DS18B20_Init(void)
{
	DS18B20_GPIO_Config();
	
	DS18B20_Config();
}

/***************************************************************************************************
*FunctionName: DS18B20_GPIO_Config
*Description: IO��ʼ��
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2017��1��18��11:04:39
***************************************************************************************************/
static void DS18B20_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(DS18B20_CLK, ENABLE); 
														   
  	GPIO_InitStructure.GPIO_Pin = DS18B20_PIN;	
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;   
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

	GPIO_Init(DS18B20_PORT, &GPIO_InitStructure); 
}

/***************************************************************************************************
*FunctionName: DS18B20_Mode_IPU
*Description: ���ö˿�Ϊ��������
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2017��1��18��11:08:19
***************************************************************************************************/
static void DS18B20_Mode_IPU(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = DS18B20_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN ; 
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(DS18B20_PORT, &GPIO_InitStructure);	 
}

/***************************************************************************************************
*FunctionName: DS18B20_Mode_Out_PP
*Description: ����Ϊ�������
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2017��1��18��11:08:49
***************************************************************************************************/
static void DS18B20_Mode_Out_PP(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = DS18B20_PIN;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;   
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

	GPIO_Init(DS18B20_PORT, &GPIO_InitStructure);	 	 
}

/***************************************************************************************************
*FunctionName: DS18B20_Rst
*Description: ��λ18b20
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2017��1��18��11:10:32
***************************************************************************************************/
static bool DS18B20_Rst(void)
{
	uint8_t pulse_time = 0;
	
	/* ��������Ϊ������� */
	DS18B20_Mode_Out_PP();
	
	DS18B20_DATA_OUT(DS18B20_LOW);
	/* �������ٲ���480us�ĵ͵�ƽ��λ�ź� */
	delay_us(600);
	/* �����ڲ�����λ�źź��轫�������� */
	DS18B20_DATA_OUT(DS18B20_HIGH);
	/* ��������Ϊ�������� */
	DS18B20_Mode_IPU();
	/*�ӻ����յ������ĸ�λ�źź󣬻���15~60us���������һ����������*/
	delay_us(120);
	
	pulse_time = DS18B20_DATA_IN();
	

	delay_us(300);
	
	if(pulse_time == 0)
		return true;
	else
		return false;
	/* �ȴ���������ĵ�������������Ϊһ��60~240us�ĵ͵�ƽ�ź� 
	 * �����������û����������ʱ�����ӻ����յ������ĸ�λ�źź󣬻���15~60us���������һ����������
	 */
/*	while( DS18B20_DATA_IN() && pulse_time < 200 )
	{
		pulse_time++;
		delay_us(1);
	}
	*/
	/* ����100us�󣬴������嶼��û�е���*/
/*	if( pulse_time >= 200 )
		return false;
	else
	{
		delay_us(240);
		return true;
	}*/
}

/***************************************************************************************************
*FunctionName: DS18B20_Write_Byte
*Description: дһ���ֽڵ�DS18B20����λ����
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2017��1��18��11:12:39
***************************************************************************************************/
static void DS18B20_Write_Byte(const unsigned char dat)
{
	unsigned char i;
	DS18B20_Mode_Out_PP();
	
	for( i=0; i<8; i++ )
	{
		DS18B20_DATA_OUT(DS18B20_LOW);
		delay_us(8);
		
		if(dat & (1<<i))
		{
			DS18B20_DATA_OUT(DS18B20_HIGH);
			delay_us(58);
		}
		else
		{
			delay_us(60);
		
			DS18B20_DATA_OUT(DS18B20_HIGH);
			delay_us(2);
		}
		
	}
}

/***************************************************************************************************
*FunctionName: DS18B20_Read_Byte
*Description: ��DS18B20��һ���ֽڣ���λ����
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2017��1��18��11:12:04
***************************************************************************************************/
static unsigned char DS18B20_Read_Byte(void)
{
	unsigned char i, dat = 0;	

	for(i=0; i<8; i++) 
	{
		DS18B20_Mode_Out_PP();		
		DS18B20_DATA_OUT(DS18B20_HIGH);
		delay_us(2);
				
		DS18B20_DATA_OUT(DS18B20_LOW);
		delay_us(5);
		
		/* ���ó����룬�ͷ����ߣ����ⲿ�������轫�������� */
		DS18B20_DATA_OUT(DS18B20_HIGH);
		DS18B20_Mode_IPU();
		delay_us(5);
		
		if( DS18B20_DATA_IN() == SET )
			dat |= (1<<i);
		delay_us(55);
	}
	
	return dat;
}

/***************************************************************************************************
*FunctionName:  DS18B20_Read_Bytes
*Description:  ��ȡ����ֽ�
*Input:  
*Output:  
*Return:  
*Author:  xsx
*Date: 2017��3��23�� 16:24:55
***************************************************************************************************/
static void DS18B20_Read_Bytes(unsigned char *buf, unsigned char len)
{
	unsigned char i = 0;
	
	for(i=0; i<len; i++)
	{
		*buf = DS18B20_Read_Byte();
		buf++;
	}
}

/***************************************************************************************************
*FunctionName:  DS18B20_CRC
*Description:  ��18b20��ȡ������У��
*Input:  
*Output:  
*Return:  
*Author:  xsx
*Date: 2017��3��23�� 16:29:39
***************************************************************************************************/
static unsigned char DS18B20_CRC(const unsigned char *buf, unsigned char len)
{
	unsigned char crc_data = 0, i = 0;
	for(i=0; i<len; i++)						//���У��
		crc_data = CrcTable[crc_data^buf[i]];
	
	return crc_data;
}

/***************************************************************************************************
*FunctionName:  DS18B20_Config
*Description:  ����18B20
*Input:  
*Output:  
*Return:  
*Author:  xsx
*Date: 2017��3��23�� 16:32:05
***************************************************************************************************/
static void DS18B20_Config(void)
{
	DS18B20_Rst();	
	DS18B20_Write_Byte(0xcc);
	DS18B20_Write_Byte(0x4e);
	DS18B20_Write_Byte(0x19);
	DS18B20_Write_Byte(0x1a);
	DS18B20_Write_Byte(0x1f);		//9bitģʽ��0.5���϶�
	
	DS18B20_Rst();	
	DS18B20_Write_Byte(0xcc);
	DS18B20_Write_Byte(0x48);
	
	DS18B20_Rst();	
	DS18B20_Write_Byte(0xcc);
	DS18B20_Write_Byte(0xb8);
}

void startDS18B20(void)
{
	DS18B20_Rst();
	DS18B20_Write_Byte(0XCC);				/* ���� ROM */
	DS18B20_Write_Byte(0X44);				/* ��ʼת�� */
}
/*
 * �洢���¶���16 λ�Ĵ�������չ�Ķ����Ʋ�����ʽ
 * ��������12λ�ֱ���ʱ������5������λ��7������λ��4��С��λ
 *
 *         |---------����----------|-----С�� �ֱ��� 1/(2^4)=0.0625----|
 * ���ֽ�  | 2^3 | 2^2 | 2^1 | 2^0 | 2^(-1) | 2^(-2) | 2^(-3) | 2^(-4) |
 *
 *
 *         |-----����λ��0->��  1->��-------|-----------����-----------|
 * ���ֽ�  |  s  |  s  |  s  |  s  |    s   |   2^6  |   2^5  |   2^4  |
 *
 * 
 * �¶� = ����λ + ���� + С��*0.0625
 */
float readDS18B20Temp(void)
{
	uint8_t value[9];
	
	short s_tem;
	float f_tem = 300;
	
	DS18B20_Rst();
	DS18B20_Write_Byte(0XCC);				/* ���� ROM */
	DS18B20_Write_Byte(0XBE);				/* ���¶�ֵ */
	
	DS18B20_Read_Bytes(value, 9);
	
	if(0 == DS18B20_CRC(value, 9))
	{
		s_tem = value[1]<<8;
		s_tem = s_tem | value[0];
		
		if( s_tem < 0 )		/* ���¶� */
			f_tem = (~s_tem+1) * 0.0625;	
		else
			f_tem = s_tem * 0.0625;
	}
	
	return f_tem; 	
}

 /**
  * @brief  ��ƥ�� ROM ����»�ȡ DS18B20 �¶�ֵ 
  * @param  ds18b20_id�����ڴ�� DS18B20 ���кŵ�������׵�ַ
  * @retval ��
  */
void DS18B20_ReadId ( uint8_t * ds18b20_id )
{
	uint8_t uc;
	
	
	DS18B20_Write_Byte(0x33);       //��ȡ���к�
	
	for ( uc = 0; uc < 8; uc ++ )
	  ds18b20_id [ uc ] = DS18B20_Read_Byte();
	
}

/****************************************end of file************************************************/





