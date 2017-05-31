/***************************************************************************************************
*FileName:Usart3_Driver
*Description:����3����
*Author:xsx
*Data:2016��4��29��11:50:01
***************************************************************************************************/






/***************************************************************************************************/
/******************************************ͷ�ļ�***************************************************/
/***************************************************************************************************/
#include 	"stm32f4xx.h"
#include 	"Usart3_Driver.h"






/***************************************************************************************************/
/**************************************�ֲ���������*************************************************/
/***************************************************************************************************/
static xQueueHandle xRxQueue;									//���ն���
static xQueueHandle xTxQueue;									//���Ͷ���
static xSemaphoreHandle xMutex;									//������






/***************************************************************************************************/
/**************************************�ֲ���������*************************************************/
/***************************************************************************************************/
static void Usart3_Os_Init(void);
static void ConfigUsart3(void);
static portBASE_TYPE prvUsart3_ISR_NonNakedBehaviour( void );






/***************************************************************************************************/
/***************************************************************************************************/
/***************************************����********************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************
*FunctionName��Usart3_Os_Init
*Description������3�Ķ��л���������
*Input��None
*Output��None
*Author��xsx
*Data��2016��4��29��11:50:51
***************************************************************************************************/
static void Usart3_Os_Init(void)
{
	xRxQueue = xQueueCreate( xRxQueue3_Len, ( unsigned portBASE_TYPE ) sizeof( signed portCHAR ) );
	xTxQueue = xQueueCreate( xTxQueue3_Len, ( unsigned portBASE_TYPE ) sizeof( signed portCHAR ) );
	
	vSemaphoreCreateBinary(xMutex);
}

/***************************************************************************************************
*FunctionName��ConfigUsart3
*Description������3�Ķ˿ڳ�ʼ��������
*Input��None
*Output��None
*Author��xsx
*Data��2016��4��29��11:52:08
***************************************************************************************************/
static void ConfigUsart3(void)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* ����GPIO_D��ʱ�� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	/* ��������3��ʱ�� */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_USART3);

	USART_InitStructure.USART_BaudRate   = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits   = USART_StopBits_1;
	USART_InitStructure.USART_Parity     = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode                = USART_Mode_Tx | USART_Mode_Rx;

	USART_Init(USART3, &USART_InitStructure);

	/* ʹ�ܴ���2 */
	USART_Cmd(USART3, ENABLE);
	//ʹ�ܽ����ж�
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	
	/* NVIC configuration */
	/* Configure the Priority Group to 2 bits */


	/* Enable the USARTx Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/***************************************************************************************************
*FunctionName��Usart3_Init
*Description������3�ⲿ���ó�ʼ������
*Input��None
*Output��None
*Author��xsx
*Data��2016��4��29��11:28:56
***************************************************************************************************/
void Usart3_Init(void)
{
	Usart3_Os_Init();
	ConfigUsart3();
}

/***************************************************************************************************
*FunctionName��USART3_IRQHandler
*Description������3���жϺ���
*Input��None
*Output��None
*Author��xsx
*Data��2016��4��29��11:29:13
***************************************************************************************************/
void USART3_IRQHandler(void)
{			
	prvUsart3_ISR_NonNakedBehaviour();
}

/***************************************************************************************************
*FunctionName��prvUsart3_ISR_NonNakedBehaviour
*Description������3���жϷ�����
*Input��None
*Output��None
*Author��xsx
*Data��2016��4��29��11:29:32
***************************************************************************************************/
__attribute__((__noinline__))
static portBASE_TYPE prvUsart3_ISR_NonNakedBehaviour( void )
{
		/* Now we can declare the local variables. */
	signed portCHAR     cChar;
	portBASE_TYPE     xHigherPriorityTaskWoken = pdFALSE;

	portBASE_TYPE retstatus;

	if(USART_GetITStatus(USART3 , USART_IT_TXE) == SET)
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
			USART_SendData(USART3, cChar);
		}
		else
		{
			/* Queue empty, nothing to send so turn off the Tx interrupt. */
			USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
		}
	}

	if(USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
	{
		/* The interrupt was caused by the receiver getting data. */
		cChar = USART_ReceiveData(USART3);

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
void EnableUsart3TXInterrupt(void)
{
	USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
}

/***************************************************************************************************
*FunctionName��GetUsart3RXQueue, GetUsart3TXQueue,GetUsart3Mutex
*Description����ȡ����3�ķ��ͽ��ն���,�Ͷ��л�����
*Input��None
*Output��None
*Author��xsx
*Data��2016��4��29��11:22:06
***************************************************************************************************/
xQueueHandle GetUsart3RXQueue(void)
{
	return xRxQueue;
}

xQueueHandle GetUsart3TXQueue(void)
{
	return xTxQueue;
}

xSemaphoreHandle GetUsart3Mutex(void)
{
	return xMutex;
}

