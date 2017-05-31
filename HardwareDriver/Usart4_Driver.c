/***************************************************************************************************
*FileName:Usart4_Driver
*Description:����4������Ŀǰ����wifi
*Author:xsx
*Data:2016��5��9��13:54:09
***************************************************************************************************/

/***************************************************************************************************/
/******************************************ͷ�ļ�***************************************************/
/***************************************************************************************************/
#include 	"stm32f4xx.h"
#include 	"Usart4_Driver.h"

/***************************************************************************************************/
/**************************************�ֲ���������*************************************************/
/***************************************************************************************************/
static xQueueHandle xRxQueue = NULL;									//���ն���
static xQueueHandle xTxQueue = NULL;									//���Ͷ���

/***************************************************************************************************/
/**************************************�ֲ���������*************************************************/
/***************************************************************************************************/
static void Usart_Os_Init(void);
static void ConfigUsart(void);
static portBASE_TYPE prvUsart_ISR_NonNakedBehaviour( void );
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************����********************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************
*FunctionName��Usart6_Os_Init
*Description����������6�Ķ��л�����
*Input��None
*Output��None
*Author��xsx
*Data��2016��4��29��11:28:04
***************************************************************************************************/
static void Usart_Os_Init(void)
{
	xRxQueue = xQueueCreate( xRxQueue4_Len, ( unsigned portBASE_TYPE ) sizeof( signed portCHAR ) );
	xTxQueue = xQueueCreate( xTxQueue4_Len, ( unsigned portBASE_TYPE ) sizeof( signed portCHAR ) );
}

/***************************************************************************************************
*FunctionName��ConfigUsart6
*Description������6�Ķ˿ڳ�ʼ��������
*Input��None
*Output��None
*Author��xsx
*Data��2016��4��29��11:28:25
***************************************************************************************************/
static void ConfigUsart(void)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* ����GPIO_D��ʱ�� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	/* ��������3��ʱ�� */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);

	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_11;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_UART4);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_UART4);

	USART_InitStructure.USART_BaudRate   = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits   = USART_StopBits_1;
	USART_InitStructure.USART_Parity     = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode                = USART_Mode_Tx | USART_Mode_Rx;

	USART_Init(UART4, &USART_InitStructure);
  
	/* ʹ�ܴ���3 */
	USART_Cmd(UART4, ENABLE);
	//ʹ�ܽ����ж�
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
	
	
	/* NVIC configuration */
	/* Configure the Priority Group to 2 bits */


	/* Enable the USARTx Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/***************************************************************************************************
*FunctionName��Usart6_Init
*Description������6�ⲿ���ó�ʼ������
*Input��None
*Output��None
*Author��xsx
*Data��2016��4��29��11:28:56
***************************************************************************************************/
void Usart4_Init(void)
{
	Usart_Os_Init();
	ConfigUsart();
}

/***************************************************************************************************
*FunctionName��USART6_IRQHandler
*Description������6���жϺ���
*Input��None
*Output��None
*Author��xsx
*Data��2016��4��29��11:29:13
***************************************************************************************************/
void UART4_IRQHandler(void)
{			
	prvUsart_ISR_NonNakedBehaviour();
}

/***************************************************************************************************
*FunctionName��prvUsart6_ISR_NonNakedBehaviour
*Description������6���жϷ�����
*Input��None
*Output��None
*Author��xsx
*Data��2016��4��29��11:29:32
***************************************************************************************************/
__attribute__((__noinline__))
static portBASE_TYPE prvUsart_ISR_NonNakedBehaviour( void )
{
	signed portCHAR     cChar;
	portBASE_TYPE     xHigherPriorityTaskWoken = pdFALSE;

	portBASE_TYPE retstatus;

	if(USART_GetITStatus(UART4 , USART_IT_TXE) == SET)
	{
		/* The interrupt was caused by the THR becoming empty.  Are there any
		more characters to transmit?
		Because FreeRTOS is not supposed to run with nested interrupts, put all OS
		calls in a critical section . */
		portENTER_CRITICAL();
			retstatus = xQueueReceiveFromISR( xTxQueue, &cChar, &xHigherPriorityTaskWoken );
		portEXIT_CRITICAL();

		if (retstatus == pdTRUE)
		{
			/* A character was retrieved from the queue so can be sent to the THR now. */
			USART_SendData(UART4, cChar);
		}
		else
		{
			/* Queue empty, nothing to send so turn off the Tx interrupt. */
			USART_ITConfig(UART4, USART_IT_TXE, DISABLE);
		}
	}

	if(USART_GetITStatus(UART4, USART_IT_RXNE) == SET)
	{
		/* The interrupt was caused by the receiver getting data. */
		cChar = USART_ReceiveData(UART4);

		/* Because FreeRTOS is not supposed to run with nested interrupts, put all OS
		calls in a critical section . */
		portENTER_CRITICAL();
			xQueueSendFromISR(xRxQueue, &cChar, &xHigherPriorityTaskWoken);
		portEXIT_CRITICAL();
	}

	/* The return value will be used by portEXIT_SWITCHING_ISR() to know if it
	should perform a vTaskSwitchContext(). */
	return ( xHigherPriorityTaskWoken );
}

/***************************************************************************************************
*FunctionName��EnableUsart6TXInterrupt
*Description������һ�η����жϣ����Ͷ��������ݣ�
*Input��None
*Output��None
*Author��xsx
*Data��2016��4��29��11:18:28
***************************************************************************************************/
void EnableUsart4TXInterrupt(void)
{
	USART_ITConfig(UART4, USART_IT_TXE, ENABLE);
}

/***************************************************************************************************
*FunctionName��GetUsart6RXQueue, GetUsart6TXQueue,GetUsart6Mutex
*Description����ȡ����6�ķ��ͽ��ն���,�Ͷ��л�����
*Input��None
*Output��None
*Author��xsx
*Data��2016��4��29��11:22:06
***************************************************************************************************/
xQueueHandle GetUsart4RXQueue(void)
{
	return xRxQueue;
}

xQueueHandle GetUsart4TXQueue(void)
{
	return xTxQueue;
}


