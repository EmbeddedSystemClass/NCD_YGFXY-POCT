/***************************************************************************************************
*FileName��MLX90614_Driver
*Description���ǽӴ�ʽ�����¶ȴ�����
*Author��xsx
*Data��2016��3��17��15:27:43
***************************************************************************************************/

/***************************************************************************************************/
/******************************************ͷ�ļ�***************************************************/
/***************************************************************************************************/
#include 	"MLX90614_Driver.h"
#include	"Delay.h"
/***************************************************************************************************/
/**************************************�ֲ���������*************************************************/
/***************************************************************************************************/

/***************************************************************************************************/
/**************************************�ֲ���������*************************************************/
/***************************************************************************************************/
static void SMBus_StartBit(void);
static void SMBus_StopBit(void);
static void SMBus_SendBit(unsigned char bit_out);
static unsigned char SMBus_SendByte(unsigned char Tx_buffer);
static unsigned char SMBus_ReceiveBit(void);
static unsigned char SMBus_ReceiveByte(unsigned char ack_nack);
static unsigned short SMBus_ReadMemory(unsigned char slaveAddress, unsigned char command);
static unsigned char PEC_Calculation(unsigned char pec[]);
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************����********************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************
*FunctionName��SMBus_Init
*Description��MLX90164�¶ȴ������˿ڳ�ʼ��
*Input��None
*Output��None
*Author��xsx
*Data��2016��3��17��15:31:10
***************************************************************************************************/
void SMBus_Init(void)
{
    GPIO_InitTypeDef    GPIO_InitStructure;

	/* Enable SMBUS_PORT clocks */
	RCC_AHB1PeriphClockCmd(MLX90614_Rcc, ENABLE);

    /*����SMBUS_SCK��SMBUS_SDAΪ���缫��©���*/
    GPIO_InitStructure.GPIO_Pin = MLX90614_SCK_Pin | MLX90614_SDA_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
    GPIO_Init(MLX90614_GpioGroup, &GPIO_InitStructure);

    SMBUS_SCK_H();
    SMBUS_SDA_H();
}

/***************************************************************************************************
*FunctionName��SMBus_StartBit
*Description��������ʼ�ź�
*Input��None
*Output��None
*Author��xsx
*Data��2016��3��17��15:32:30
***************************************************************************************************/
static void SMBus_StartBit(void)
{
    SMBUS_SDA_H();		// Set SDA line
    delay_us(5);	    // Wait a few microseconds
    SMBUS_SCK_H();		// Set SCL line
    delay_us(5);	    // Generate bus free time between Stop
	
    SMBUS_SDA_L();		// Clear SDA line
    delay_us(10);	    // Hold time after (Repeated) Start
    // Condition. After this period, the first clock is generated.
    //(Thd:sta=4.0us min)
    SMBUS_SCK_L();	    // Clear SCL line
    delay_us(5);	    // Wait a few microseconds
}

/***************************************************************************************************
*FunctionName��SMBus_StopBit
*Description�����������ź�
*Input��None
*Output��None
*Author��xsx
*Data��2016��3��17��15:32:45
***************************************************************************************************/
static void SMBus_StopBit(void)
{
    SMBUS_SCK_L();		// Clear SCL line
    delay_us(5);	    // Wait a few microseconds
	
    SMBUS_SDA_L();		// Clear SDA line
    delay_us(5);	    // Wait a few microseconds
	
    SMBUS_SCK_H();		// Set SCL line
    delay_us(10);	    // Stop condition setup time(Tsu:sto=4.0us min)
    SMBUS_SDA_H();		// Set SDA line
}

/***************************************************************************************************
*FunctionName��SMBus_SendByte
*Description���񴫸�������һ���ֽ�
*Input��Tx_buffer -- ����������
*Output��None
*Author��xsx
*Data��2016��3��17��15:33:14
***************************************************************************************************/
static unsigned char SMBus_SendByte(unsigned char Tx_buffer)
{
    unsigned char	Bit_counter;
    unsigned char	Ack_bit;
    unsigned char	bit_out;

    for(Bit_counter=8; Bit_counter; Bit_counter--)
    {
        if (Tx_buffer&0x80)
        {
            bit_out=1;   // If the current bit of Tx_buffer is 1 set bit_out
        }
        else
        {
            bit_out=0;  // else clear bit_out
        }
        SMBus_SendBit(bit_out);		// Send the current bit on SDA
        Tx_buffer<<=1;				// Get next bit for checking
    }

    Ack_bit=SMBus_ReceiveBit();		// Get acknowledgment bit
    return	Ack_bit;
}

/***************************************************************************************************
*FunctionName��SMBus_SendBit
*Description������һ��λ
*Input��None
*Output��None
*Author��xsx
*Data��2016��3��17��15:35:30
***************************************************************************************************/
static void SMBus_SendBit(unsigned char bit_out)
{
    if(bit_out==0)
		SMBUS_SDA_L();
    else
        SMBUS_SDA_H();

    delay_us(2);					// Tsu:dat = 250ns minimum
	
    SMBUS_SCK_H();					// Set SCL line
    delay_us(6);					// High Level of Clock Pulse
    SMBUS_SCK_L();					// Clear SCL line
    delay_us(3);					// Low Level of Clock Pulse
}

/***************************************************************************************************
*FunctionName��SMBus_ReceiveBit
*Description������һ��λ
*Input��None
*Output��None
*Author��xsx
*Data��2016��3��17��15:37:49
***************************************************************************************************/
static unsigned char SMBus_ReceiveBit(void)
{
    unsigned char Ack_bit;

    SMBUS_SDA_H();          //���ſ��ⲿ������������������
	delay_us(2);			// High Level of Clock Pulse
    SMBUS_SCK_H();			// Set SCL line
    delay_us(5);			// High Level of Clock Pulse
	
    if (SMBUS_SDA_PIN())
        Ack_bit=1;
    else
		Ack_bit=0;


    SMBUS_SCK_L();			// Clear SCL line
    delay_us(3);			// Low Level of Clock Pulse

    return	Ack_bit;
}

/***************************************************************************************************
*FunctionName��SMBus_ReceiveByte
*Description������һ���ֽڣ�������Ӧ��
*Input��ack_nack -- Ӧ������
*Output��None
*Author��xsx
*Data��2016��3��17��15:38:22
***************************************************************************************************/
static unsigned char SMBus_ReceiveByte(unsigned char ack_nack)
{
    unsigned char 	RX_buffer;
    unsigned char	Bit_Counter;

    for(Bit_Counter=8; Bit_Counter; Bit_Counter--)
    {
        if(SMBus_ReceiveBit())			// Get a bit from the SDA line
        {
            RX_buffer <<= 1;			// If the bit is HIGH save 1  in RX_buffer
            RX_buffer |=0x01;
        }
        else
        {
            RX_buffer <<= 1;			// If the bit is LOW save 0 in RX_buffer
            RX_buffer &=0xfe;
        }
    }
    SMBus_SendBit(ack_nack);			// Sends acknowledgment bit
	
    return RX_buffer;
}

/***************************************************************************************************
*FunctionName��SMBus_ReadMemory
*Description����ȡ����
*Input��slaveAddress -- ��������ַ
*		command -- ��ȡ�����ݵ�ַ
*Output��None
*Author��xsx
*Data��2016��3��17��15:39:58
***************************************************************************************************/
static unsigned short SMBus_ReadMemory(unsigned char slaveAddress, unsigned char command)
{
    unsigned short data;			// Data storage (DataH:DataL)
    unsigned char Pec;				// PEC byte storage
    unsigned char DataL=0;			// Low data byte storage
    unsigned char DataH=0;			// High data byte storage
    unsigned char arr[6];			// Buffer for the sent bytes
    unsigned char PecReg;			// Calculated PEC byte storage
    unsigned char ErrorCounter;	// Defines the number of the attempts for communication with MLX90614

    ErrorCounter = 10;				// Initialising of ErrorCounter
	slaveAddress <<= 1;	//2-7λ��ʾ�ӻ���ַ
	
    do
    {
repeat:
		SMBus_StopBit();			    //If slave send NACK stop comunication
        --ErrorCounter;				    //Pre-decrement ErrorCounter
        if(!ErrorCounter) 			    //ErrorCounter=0?
        {
            break;					    //Yes,go out from do-while{}
        }
		
        SMBus_StartBit();				//Start condition
        if(SMBus_SendByte(slaveAddress))//Send SlaveAddress ���λWr=0��ʾ������д����
        {
            goto	repeat;			    //Repeat comunication again
        }
        if(SMBus_SendByte(command))	    //Send command
        {
            goto	repeat;		    	//Repeat comunication again
        }

        SMBus_StartBit();					//Repeated Start condition
        if(SMBus_SendByte(slaveAddress+1))	//Send SlaveAddress ���λRd=1��ʾ������������
        {
            goto	repeat;             	//Repeat comunication again
        }

        DataL = SMBus_ReceiveByte(ACK);	//Read low data,master must send ACK
        DataH = SMBus_ReceiveByte(ACK); //Read high data,master must send ACK
        Pec = SMBus_ReceiveByte(NACK);	//Read PEC byte, master must send NACK
        SMBus_StopBit();				//Stop condition

        arr[5] = slaveAddress;		//
        arr[4] = command;			//
        arr[3] = slaveAddress+1;	//Load array arr
        arr[2] = DataL;				//
        arr[1] = DataH;				//
        arr[0] = 0;					//
        PecReg=PEC_Calculation(arr);//Calculate CRC
    }
    while(PecReg != Pec);		//If received and calculated CRC are equal go out from do-while{}

	data = (DataH<<8) | DataL;	//data=DataH:DataL
    return data;
}

/*******************************************************************************
* Function Name  : PEC_calculation
* Description    : Calculates the PEC of received bytes
* Input          : pec[]
* Output         : None
* Return         : pec[0]-this byte contains calculated crc value
*******************************************************************************/
static unsigned char PEC_Calculation(unsigned char pec[])
{
    unsigned char 	crc[6];
    unsigned char	BitPosition=47;
    unsigned char	shift;
    unsigned char	i;
    unsigned char	j;
    unsigned char	temp;

    do
    {
        /*Load pattern value 0x000000000107*/
        crc[5]=0;
        crc[4]=0;
        crc[3]=0;
        crc[2]=0;
        crc[1]=0x01;
        crc[0]=0x07;

        /*Set maximum bit position at 47 ( six bytes byte5...byte0,MSbit=47)*/
        BitPosition=47;

        /*Set shift position at 0*/
        shift=0;

        /*Find first "1" in the transmited message beginning from the MSByte byte5*/
        i=5;
        j=0;
        while((pec[i]&(0x80>>j))==0 && i>0)
        {
            BitPosition--;
            if(j<7)
            {
                j++;
            }
            else
            {
                j=0x00;
                i--;
            }
        }/*End of while */

        /*Get shift value for pattern value*/
        shift=BitPosition-8;

        /*Shift pattern value */
        while(shift)
        {
            for(i=5; i<0xFF; i--)
            {
                if((crc[i-1]&0x80) && (i>0))
                {
                    temp=1;
                }
                else
                {
                    temp=0;
                }
                crc[i]<<=1;
                crc[i]+=temp;
            }/*End of for*/
            shift--;
        }/*End of while*/

        /*Exclusive OR between pec and crc*/
        for(i=0; i<=5; i++)
        {
            pec[i] ^=crc[i];
        }/*End of for*/
    }
    while(BitPosition>8); /*End of do-while*/

    return pec[0];
}

/***************************************************************************************************
*FunctionName��SMBus_ReadTemperature
*Description����ȡ�¶ȣ����浽ָ��λ��
*Input��pbuf -- �¶ȱ���λ��
*Output��None
*Author��xsx
*Data��2016��3��17��15:46:52
***************************************************************************************************/
float GetCardTemperature(void)
{
	float temp = 0.0;
	
	/*��ȡ���������¶�*/
	temp = SMBus_ReadMemory(SA, RAM_ACCESS|0x07);
	temp *= 0.02;
	temp -= 273.15;
	
	return temp;
}

/*********************************END OF FILE*********************************/
