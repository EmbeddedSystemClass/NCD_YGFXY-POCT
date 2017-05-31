/***************************************************************************************************
*FileName:Usart2_Driver
*Description:����2����
*Author:xsx
*Data:2016��4��29��11:57:28
***************************************************************************************************/


/***************************************************************************************************/
/******************************************ͷ�ļ�***************************************************/
/***************************************************************************************************/
#include "stm32f4xx.h"
#include "Usart2_Driver.h"

/***************************************************************************************************/
/**************************************�ֲ���������*************************************************/
/***************************************************************************************************/
static xQueueHandle xRxQueue;									//���ն���
static xQueueHandle xTxQueue;									//���Ͷ���
static xSemaphoreHandle xRXMutex;								//������
static xSemaphoreHandle xTXMutex;								//������




/***************************************************************************************************/
/**************************************�ֲ���������*************************************************/
/***************************************************************************************************/
static void Usart2_Os_Init(void);
static void ConfigUsart2(void);
static portBASE_TYPE prvUsart2_ISR_NonNakedBehaviour( void );






/***************************************************************************************************/
/***************************************************************************************************/
/***************************************����********************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************
*FunctionName��Usart2_Os_Init
*Description������2���л���������
*Input��None
*Output��None
*Author��xsx
*Data��2016��4��29��11:58:16
***************************************************************************************************/
static void Usart2_Os_Init(void)
{
	xRxQueue = xQueueCreate( xRxQueue2_Len, ( unsigned portBASE_TYPE ) sizeof( signed portCHAR ) );
	xTxQueue = xQueueCreate( xTxQueue2_Len, ( unsigned portBASE_TYPE ) sizeof( signed portCHAR ) );
	
	vSemaphoreCreateBinary(xRXMutex);
	vSemaphoreCreateBinary(xTXMutex);
}


/***************************************************************************************************
*FunctionName��ConfigUsart2
*Description������2�Ķ˿ڳ�ʼ��
*Input��None
*Output��None
*Author��xsx
*Data��2016��4��29��11:58:48
***************************************************************************************************/
static void ConfigUsart2(void)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* ����GPIO_D��ʱ�� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD , ENABLE);
	/* ��������3��ʱ�� */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 , ENABLE);

	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_USART2);


	USART_InitStructure.USART_BaudRate   = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits   = USART_StopBits_1;
	USART_InitStructure.USART_Parity     = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode                = USART_Mode_Tx | USART_Mode_Rx;

	USART_Init(USART2, &USART_InitStructure);

	/* ʹ�ܴ���2 */
	USART_Cmd(USART2, ENABLE);
	//ʹ�ܽ����ж�
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	
	/* NVIC configuration */
	/* Configure the Priority Group to 2 bits */


	/* Enable the USARTx Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/***************************************************************************************************
*FunctionName��Usart2_Init
*Description������2��ʼ�����ⲿ����
*Input��None
*Output��None
*Author��xsx
*Data��2016��4��29��11:59:09
***************************************************************************************************/
void Usart2_Init(void)
{
	Usart2_Os_Init();
	ConfigUsart2();
}


/***************************************************************************************************
*FunctionName��USART2_IRQHandler
*Description������2�жϺ���
*Input��None
*Output��None
*Author��xsx
*Data��2016��4��29��11:59:24
***************************************************************************************************/
void USART2_IRQHandler(void)
{			
	prvUsart2_ISR_NonNakedBehaviour();
}


/***************************************************************************************************
*FunctionName��prvUsart2_ISR_NonNakedBehaviour
*Description������2�жϷ�����
*Input��None
*Output��None
*Author��xsx
*Data��2016��4��29��11:59:44
***************************************************************************************************/
__attribute__((__noinline__))
static portBASE_TYPE prvUsart2_ISR_NonNakedBehaviour( void )
{
		/* Now we can declare the local variables. */
	signed portCHAR     cChar;
	portBASE_TYPE     xHigherPriorityTaskWoken = pdFALSE;

	portBASE_TYPE retstatus;

	if(USART_GetITStatus(USART2 , USART_IT_TXE) == SET)
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
			USART_SendData(USART2, cChar);
		}
		else
		{
			/* Queue empty, nothing to send so turn off the Tx interrupt. */
			USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
		}
	}

	if(USART_GetITStatus(USART2, USART_IT_RXNE) == SET)
	{
		/* The interrupt was caused by the receiver getting data. */
		cChar = USART_ReceiveData(USART2);

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
*FunctionName��EnableUsart2TXInterrupt
*Description������һ�η����жϣ����Ͷ��������ݣ�
*Input��None
*Output��None
*Author��xsx
*Data��2016��4��29��11:18:28
***************************************************************************************************/
void EnableUsart2TXInterrupt(void)
{
	USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
}

/***************************************************************************************************
*FunctionName��GetUsart2RXQueue, GetUsart2TXQueue,GetUsart2Mutex
*Description����ȡ����2�ķ��ͽ��ն���,�Ͷ��л�����
*Input��None
*Output��None
*Author��xsx
*Data��2016��4��29��11:22:06
***************************************************************************************************/
xQueueHandle GetUsart2RXQueue(void)
{
	return xRxQueue;
}

xQueueHandle GetUsart2TXQueue(void)
{
	return xTxQueue;
}

xSemaphoreHandle GetUsart2RXMutex(void)
{
	return xRXMutex;
}

xSemaphoreHandle GetUsart2TXMutex(void)
{
	return xTXMutex;
}



