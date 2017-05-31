/******************************************************************************************/
/*****************************************ͷ�ļ�*******************************************/

#include	"CodeScan_Task.h"
#include	"CodeScanFunction.h"

#include 	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"
#include	"semphr.h"

#include	<string.h>
#include	"stdio.h"
#include 	"stdlib.h"
/******************************************************************************************/
/*****************************************�ֲ���������*************************************/

#define vCodeScanTask_PRIORITY			( ( unsigned portBASE_TYPE ) 2U )
const char * CodeScanTaskName = "vCodeScanTask";

static xQueueHandle xStartScanQueue = NULL ;			//ɨ���ά�����ݿռ��ַ�Ķ��У�������ܵ��ռ��ַ������ɨ���ά������
static QRCode * cardQR;							//ɨ���ά�����ݿռ��ַ

static xQueueHandle xScanResultQueue = NULL;			//����ɨ��������
static ScanCodeResult scanresult;						//ɨ���ά��Ľ��
/******************************************************************************************/
/*****************************************�ֲ���������*************************************/

static void vCodeScanTask( void *pvParameters );

/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/

/************************************************************************
** ������:
** ��  ��:
** ��  ��:��
** ��  ��:��
** ��  �أ���
** ��  ע����
** ʱ  ��:  
** ��  �ߣ�xsx                                                 
************************************************************************/
char StartCodeScanTask(void)
{
	if(xStartScanQueue == NULL)
		xStartScanQueue = xQueueCreate(1, sizeof(void *));
	
	if(xStartScanQueue == NULL)
		return My_Fail;
	
	if(xScanResultQueue == NULL)
		xScanResultQueue = xQueueCreate(1, sizeof(ScanCodeResult));
	
	if(xScanResultQueue == NULL)
		return My_Fail;
	
	return xTaskCreate( vCodeScanTask, CodeScanTaskName, configMINIMAL_STACK_SIZE+100, NULL, vCodeScanTask_PRIORITY, NULL );
}

/************************************************************************
** ������:
** ��  ��:
** ��  ��:��
** ��  ��:��
** ��  �أ���
** ��  ע����
** ʱ  ��:  
** ��  �ߣ�xsx                                                 
************************************************************************/
static void vCodeScanTask( void *pvParameters )
{
	while(1)
	{
		if(pdPASS == xQueueReceive( xStartScanQueue, &cardQR, portMAX_DELAY))
		{
			clearScanQRCodeResult();
			vTaskDelay(50 / portTICK_RATE_MS);
			
			scanresult = ScanCodeFun(cardQR);										//��ȡ��ά��
			
			vTaskDelay(10 / portTICK_RATE_MS);
			
			/*���Ͳ��Խ��*/
			xQueueSend( xScanResultQueue, &scanresult, 100/portTICK_RATE_MS );
		}
	}
}

/***************************************************************************************************
*FunctionName��StartScanQRCode
*Description����ʼ��ȡ��ά�룬��ɨ�������������״̬
*Input��parm -- ��ȡ��ά��ĵ�ַ�������Ӧ���Ǵ�Ŷ�ά�����ݵĵ�ַ�����Զ��з��͵�ʱ���͵��Ǵ�Ŷ�ά���ַ��ָ��ĵ�ַ
*Output��None
*Author��xsx
*Data��2016��7��11��14:56:57
***************************************************************************************************/
MyState_TypeDef StartScanQRCode(void * parm)
{
	if(pdPASS == xQueueSend( xStartScanQueue, &parm, 10*portTICK_RATE_MS))
		return My_Pass;
	else
		return My_Fail;	
}


MyState_TypeDef TakeScanQRCodeResult(ScanCodeResult *scanresult)
{
	if(pdPASS == xQueueReceive( xScanResultQueue, scanresult,  0/portTICK_RATE_MS))
		return My_Pass;
	else
		return My_Fail;
}

void clearScanQRCodeResult(void)
{
	while(pdPASS == TakeScanQRCodeResult(&scanresult))
		;
}

