/***************************************************************************************************
*FileName��PT8211_Driver
*Description����ƵDA����
*Author��xsx
*Data��2015��10��6��10:57:01
***************************************************************************************************/

/***************************************************************************************************/
/******************************************ͷ�ļ�***************************************************/
/***************************************************************************************************/
#include 	"stm32f4xx.h"
#include	"PT8211_Driver.h"


#include	"MyMem.h"

/***************************************************************************************************/
/**************************************�ֲ���������*************************************************/
/***************************************************************************************************/

/***************************************************************************************************/
/**************************************�ֲ���������*************************************************/
/***************************************************************************************************/
void (*i2s_tx_callback)(void);
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************����********************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************
*FunctionName��PT8211_GPIOInit
*Description����ʼ��PT8211��I2S�ӿ�
*Input��None
*Output��None
*Author��xsx
*Data��2015��8��26��14:34:45
***************************************************************************************************/
void PT8211_GPIOInit(void)
{	
	GPIO_InitTypeDef  GPIO_InitStructure;

  	RCC_AHB1PeriphClockCmd(I2S_CK_Rcc | I2S_SD_Rcc | I2S_WS_Rcc | Audio_Mute_Rcc, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);//ʹ��SPI1ʱ��

  	GPIO_InitStructure.GPIO_Pin = I2S_CK_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//���� 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  	GPIO_Init(I2S_CK_Group, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = I2S_SD_Pin;
	GPIO_Init(I2S_SD_Group, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = I2S_WS_Pin;
	GPIO_Init(I2S_WS_Group, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = Audio_Mute_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//���ù���
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//���� 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  	GPIO_Init(Audio_Mute_Group, &GPIO_InitStructure);
	
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_SPI3); //PB3����Ϊ SPI1
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI3); //PB4����Ϊ SPI1
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource15, GPIO_AF_SPI3); //PB5����Ϊ SPI1
	
	SetAudioMode(Mute_Mode);
}

void SetAudioMode(AudioMode mode)
{
	if(Audio_Mode == mode)
		GPIO_SetBits(Audio_Mute_Group, Audio_Mute_Pin);
	else
		GPIO_ResetBits(Audio_Mute_Group, Audio_Mute_Pin);
}

/***************************************************************************************************
*FunctionName��ConfigI2S
*Description�����ݶ�ȡ����Ƶ��Ϣ����I2S
*Input��audiofreq -- ����Ƶ��
*Output��
*Author��
*Data��
***************************************************************************************************/
void ConfigI2S(unsigned short audiofreq)
{
  	I2S_InitTypeDef   I2S_InitStructure;

 	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
	
	PT8211_GPIOInit();
	
	SPI_I2S_DeInit(SPI3);  

	I2S_InitStructure.I2S_Mode = I2S_Mode_MasterTx;
	I2S_InitStructure.I2S_Standard = I2S_Standard_MSB;
	I2S_InitStructure.I2S_DataFormat = I2S_DataFormat_16b;
	I2S_InitStructure.I2S_MCLKOutput = I2S_MCLKOutput_Disable;
	I2S_InitStructure.I2S_AudioFreq = audiofreq;
	I2S_InitStructure.I2S_CPOL = I2S_CPOL_Low;
	I2S_Init(SPI3, &I2S_InitStructure);
	
	I2S_Cmd(SPI3, ENABLE);
}

void I2S_DMA_Init(unsigned char *buf0, unsigned char *buf1, unsigned short num)
{
	DMA_InitTypeDef DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE); //??DMA??

	DMA_DeInit(DMA1_Stream5);
		while (DMA_GetCmdStatus(DMA1_Stream5) != DISABLE){}//�ȴ�DMA1_Stream1������ 	 
	
	DMA_InitStructure.DMA_Channel = DMA_Channel_0;  //ͨ��0 SPI2_TXͨ�� 
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&SPI3->DR;//�����ַΪ:(u32)&SPI2->DR
	DMA_InitStructure.DMA_Memory0BaseAddr = (u32)buf0;//DMA �洢��0��ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;//�洢��������ģʽ
	DMA_InitStructure.DMA_BufferSize = num;//���ݴ����� 
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//���������ģʽ
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//�洢������ģʽ
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//�������ݳ���:16λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//�洢�����ݳ��ȣ�16λ 
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;// ʹ��ѭ��ģʽ 
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;//�����ȼ�
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable; //��ʹ��FIFOģʽ        
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//����ͻ�����δ���
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//�洢��ͻ�����δ���
	DMA_Init(DMA1_Stream5, &DMA_InitStructure);//��ʼ��DMA Stream
	DMA_Cmd(DMA1_Stream5,DISABLE);//�ر�DMA,��������	
			
	DMA_DoubleBufferModeConfig(DMA1_Stream5,(u32)buf1,DMA_Memory_0);//˫����ģʽ����
	 
	DMA_DoubleBufferModeCmd(DMA1_Stream5,ENABLE);//˫����ģʽ����
	 
	DMA_ITConfig(DMA1_Stream5,DMA_IT_TC,ENABLE);//������������ж�
	SPI_I2S_DMACmd(SPI3,SPI_I2S_DMAReq_Tx,ENABLE);//SPI2 TX DMA????.
	
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream5_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x4;//��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;//�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure);//����
}

void DMA1_Stream5_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_Stream5,DMA_IT_TCIF5)==SET)////DMA1_Stream4,������ɱ�־
	{
		DMA_ClearITPendingBit(DMA1_Stream5,DMA_IT_TCIF5);
      	i2s_tx_callback();	//ִ�лص�����,��ȡ���ݵȲ����������洦��  
	}
}

void StartPlay(void)
{
	SetAudioMode(Audio_Mode);
	DMA_Cmd(DMA1_Stream5,ENABLE);//����DMA TX����,��ʼ���� 
	
}
void StopPlay(void)
{
	SetAudioMode(Mute_Mode);
	DMA_Cmd(DMA1_Stream5,DISABLE);//�ر�DMA,��������	
	DMA_ITConfig(DMA1_Stream5,DMA_IT_TC,DISABLE);//������������ж�
	SPI_I2S_DMACmd(SPI3,SPI_I2S_DMAReq_Tx,DISABLE);//SPI2 TX DMA????.
}

