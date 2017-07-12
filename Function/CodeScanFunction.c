/******************************************************************************************/
/*****************************************ͷ�ļ�*******************************************/

#include	"CodeScanner_Driver.h"
#include	"CodeScanFunction.h"
#include	"Usart2_Driver.h"
#include	"CardLimit_Driver.h"

#include	"MyEncryptTool.h"
#include	"QueueUnits.h"
#include	"Motor_Fun.h"
#include	"System_Data.h"
#include	"Timer_Data.h"
#include	"ItemConst_Data.h"

#include	"Define.h"
#include	"CRC16.h"
#include	"MyMem.h"

#include 	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"
#include	"semphr.h"

#include	<string.h>
#include	"stdio.h"
#include 	"stdlib.h"

/******************************************************************************************/
/*****************************************�ֲ���������*************************************/

/******************************************************************************************/
/*****************************************�ֲ���������*************************************/
static void ReadBasicCodeData(ReadQRCodeBuffer * readQRCodeBuffer);
static void AnalysisCode(ReadQRCodeBuffer * readQRCodeBuffer);
static MyRes CheckCardIsTimeOut(ReadQRCodeBuffer * readQRCodeBuffer);
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/

ScanCodeResult ScanCodeFun(QRCode * parm)
{
	ReadQRCodeBuffer * readQRCodeBuffer;
	ScanCodeResult scanresult;							//ɨ���ά����
	
	if(parm == NULL)
		return CardCodeScanFail;
	
	readQRCodeBuffer = MyMalloc(sizeof(ReadQRCodeBuffer));
	if(NULL == readQRCodeBuffer)
		return CardCodeScanFail;
	
	memset(readQRCodeBuffer, 0, sizeof(ReadQRCodeBuffer));
	
	readQRCodeBuffer->cardQR = parm;
	memset(readQRCodeBuffer->cardQR, 0, sizeof(QRCode));
		
	MotorMoveTo(0, 0);
	MotorMoveTo(500, 0);
	readQRCodeBuffer->motorLocation = getSystemRunTimeData()->motorData.location;
		
	OpenCodeScanner();
	
	while(pdPASS == xQueueReceive(GetUsart2RXQueue(), readQRCodeBuffer->originalcode , 10/portTICK_RATE_MS))
		;

	readQRCodeBuffer->scanresult = CardCodeScanning;
	
	//���ö�ʱ��
	timer_set(&(readQRCodeBuffer->timer), MAX_SCAN_QR_TIME);
	
	while(readQRCodeBuffer->scanresult == CardCodeScanning)
	{
		if(CardPinIn == NoCard)				//�����γ�
		{
			readQRCodeBuffer->scanresult = CardCodeCardOut;
			break;
		}
		else
		{
			if((readQRCodeBuffer->motorLocation >= 1000)||(readQRCodeBuffer->motorLocation <= 200))
				readQRCodeBuffer->motorDir++;
			
			if(readQRCodeBuffer->motorDir%2 == 0)
				readQRCodeBuffer->motorLocation += 1;
			else
				readQRCodeBuffer->motorLocation -= 1;
			
			MotorMoveTo(readQRCodeBuffer->motorLocation, 0);
			
			if(readQRCodeBuffer->motorLocation % 50 == 0)
				ReadBasicCodeData(readQRCodeBuffer);

			vTaskDelay(2 / portTICK_RATE_MS);
			
			if(TimeOut == timer_expired(&(readQRCodeBuffer->timer)))
				readQRCodeBuffer->scanresult = CardCodeScanTimeOut;				
		}
	}

	CloseCodeScanner();
	
	scanresult = readQRCodeBuffer->scanresult;
	
	MyFree(readQRCodeBuffer);
	
	return scanresult;
}

/***************************************************************************************************
*FunctionName��ReadBasicCodeData
*Description����ȡԭʼ��ά��
*Input��None
*Output��None
*Author��xsx
*Data��2016��4��25��14:07:38
***************************************************************************************************/
static void ReadBasicCodeData(ReadQRCodeBuffer * readQRCodeBuffer)
{
	if(readQRCodeBuffer == NULL)
		return;
	
	memset(readQRCodeBuffer->originalcode, 0, MAX_QR_CODE_LENGHT+1);
	ReceiveDataFromQueue(GetUsart2RXQueue(), GetUsart2RXMutex(), readQRCodeBuffer->originalcode , MAX_QR_CODE_LENGHT, 
		&(readQRCodeBuffer->originalCodeLen), 1, 10 / portTICK_RATE_MS, 10 / portTICK_RATE_MS);
	
	if(readQRCodeBuffer->originalCodeLen > 0)
	{
		readQRCodeBuffer->originalCodeLen -= 1;
		AnalysisCode(readQRCodeBuffer);
	}
}

/***************************************************************************************************
*FunctionName��DecryptCode
*Description�����벢������ά������
*Input��None
*Output��None
*Author��xsx
*Data��2016��4��25��14:08:25
***************************************************************************************************/
static void AnalysisCode(ReadQRCodeBuffer * readQRCodeBuffer)
{
	unsigned char i=0;
	
	if(readQRCodeBuffer == NULL)
		return;
	
	/*��ն�ά��ռ�*/
	memset(readQRCodeBuffer->decryptcode, 0, MAX_QR_CODE_LENGHT);

	/*���ݽ���ʧ��*/
	if(pdFAIL == MyDencrypt(readQRCodeBuffer->originalcode, readQRCodeBuffer->decryptcode, readQRCodeBuffer->originalCodeLen))
		goto END;
	
	memcpy(readQRCodeBuffer->originalcode, readQRCodeBuffer->decryptcode, readQRCodeBuffer->originalCodeLen);
	readQRCodeBuffer->pbuf2 = readQRCodeBuffer->originalcode;
	
	/*��ȡ������Ŀ����*/
	readQRCodeBuffer->pbuf1 = strtok(readQRCodeBuffer->decryptcode, "#");
	if(readQRCodeBuffer->pbuf1)
	{
		memcpy(readQRCodeBuffer->cardQR->ItemName, readQRCodeBuffer->pbuf1 ,strlen(readQRCodeBuffer->pbuf1));
		if(getItemConstData(&(readQRCodeBuffer->cardQR->itemConstData), readQRCodeBuffer->cardQR->ItemName) == My_Fail)
		{
			readQRCodeBuffer->scanresult = CardUnsupported;
			goto END;
		}
	}
	else
		goto END;
		
	//��ȡ��⿨�ϵļ��ͨ��
	readQRCodeBuffer->pbuf1 = strtok(NULL , "#");
	if(readQRCodeBuffer->pbuf1)
		readQRCodeBuffer->cardQR->ChannelNum = strtol(readQRCodeBuffer->pbuf1, NULL, 10);
	else
		goto END;
	
	/*��ȡ��⿨T��λ��*/
	readQRCodeBuffer->pbuf1 = strtok(NULL , "#");
	if(readQRCodeBuffer->pbuf1)
		readQRCodeBuffer->cardQR->ItemLocation = strtol(readQRCodeBuffer->pbuf1 , NULL, 10);
	else
		goto END;
		
	/*��ȡ��⿨��׼�����ٽ�Ũ��1*/
	readQRCodeBuffer->pbuf1 = strtok(NULL , "#");
	if(readQRCodeBuffer->pbuf1)
		readQRCodeBuffer->cardQR->ItemFenDuan[0] = strtod(readQRCodeBuffer->pbuf1 , NULL);
	else
		goto END;
	
	/*��ȡ��⿨��׼�����ٽ�Ũ��2*/
	readQRCodeBuffer->pbuf1 = strtok(NULL , "#");
	if(readQRCodeBuffer->pbuf1)
		readQRCodeBuffer->cardQR->ItemFenDuan[1] = strtod(readQRCodeBuffer->pbuf1 , NULL);
	else
		goto END;

	/*��׼����1*/
	for(i=0; i<3; i++)
	{
		readQRCodeBuffer->pbuf1 = strtok(NULL , "#");
		if(readQRCodeBuffer->pbuf1)
			readQRCodeBuffer->cardQR->ItemBiaoQu[0][i] = strtod(readQRCodeBuffer->pbuf1 , NULL);
		else
			goto END;
	}
	
	/*��׼����2*/
	if(readQRCodeBuffer->cardQR->ItemFenDuan[0] > 0)
	{
		for(i=0; i<3; i++)
		{
			readQRCodeBuffer->pbuf1 = strtok(NULL , "#");
			if(readQRCodeBuffer->pbuf1)
				readQRCodeBuffer->cardQR->ItemBiaoQu[1][i] = strtod(readQRCodeBuffer->pbuf1 , NULL);
			else
				goto END;
		}
	}
	
	/*��׼����3*/
	if(readQRCodeBuffer->cardQR->ItemFenDuan[1] > 0)
	{
		for(i=0; i<3; i++)
		{
			readQRCodeBuffer->pbuf1 = strtok(NULL , "#");
			if(readQRCodeBuffer->pbuf1)
				readQRCodeBuffer->cardQR->ItemBiaoQu[2][i] = strtod(readQRCodeBuffer->pbuf1 , NULL);
			else
				goto END;
		}
	}
		
	/*��ȡ��⿨��Ӧʱ��*/
	readQRCodeBuffer->pbuf1 = strtok(NULL , "#");
	if(readQRCodeBuffer->pbuf1)
		readQRCodeBuffer->cardQR->CardWaitTime = strtol(readQRCodeBuffer->pbuf1 , NULL , 10);
	else
		goto END;
		
	/*��ȡ��⿨C��λ��*/
	readQRCodeBuffer->pbuf1 = strtok(NULL , "#");
	if(readQRCodeBuffer->pbuf1)
		readQRCodeBuffer->cardQR->CLineLocation = strtol(readQRCodeBuffer->pbuf1 , NULL , 10);
	else
		goto END;


	/*��ȡ��⿨����*/
	readQRCodeBuffer->pbuf1 = strtok(NULL , "#");
	if(readQRCodeBuffer->pbuf1)
		memcpy(readQRCodeBuffer->cardQR->PiHao, readQRCodeBuffer->pbuf1, strlen(readQRCodeBuffer->pbuf1));
	else
		goto END;
	
	
	/*��ȡ��⿨����*/
	readQRCodeBuffer->pbuf1 = strtok(NULL , "#");
	if(readQRCodeBuffer->pbuf1)
		memcpy(readQRCodeBuffer->cardQR->piNum, readQRCodeBuffer->pbuf1, strlen(readQRCodeBuffer->pbuf1));
	else
		goto END;

	/*��ȡ��⿨������*/
	readQRCodeBuffer->pbuf1 = strtok(NULL , "#");
	if(readQRCodeBuffer->pbuf1)
	{
		static char year[10] ,month[10],day[10];
		memcpy(year, readQRCodeBuffer->pbuf1, 2);
		readQRCodeBuffer->cardQR->CardBaoZhiQi.year = strtol(year , NULL , 10);

		memcpy(month, &readQRCodeBuffer->pbuf1[2], 2);
		readQRCodeBuffer->cardQR->CardBaoZhiQi.month = (unsigned char)strtod(month , NULL );

		memcpy(day, &readQRCodeBuffer->pbuf1[4], 2);
		readQRCodeBuffer->cardQR->CardBaoZhiQi.day = (unsigned char)strtod(day , NULL );
	}
	else
		goto END;

	/*��ȡ��ά��CRC*/
	readQRCodeBuffer->pbuf1 = strtok(NULL , "#");
	if(readQRCodeBuffer->pbuf1)
	{
		readQRCodeBuffer->cardQR->CRC16 = strtol(readQRCodeBuffer->pbuf1 , NULL , 10);
			
		readQRCodeBuffer->tempV1 = strlen(readQRCodeBuffer->pbuf1);
		goto END;
	}
	else
		goto END;
	
	END:
		if(readQRCodeBuffer->scanresult != CardCodeScanning)
			return;
		else if(readQRCodeBuffer->cardQR->CRC16 != CalModbusCRC16Fun(readQRCodeBuffer->pbuf2 , readQRCodeBuffer->originalCodeLen - 
			readQRCodeBuffer->tempV1, NULL))
			readQRCodeBuffer->scanresult = CardCodeCRCError;		
		else if(My_Fail == CheckCardIsTimeOut(readQRCodeBuffer))
			readQRCodeBuffer->scanresult = CardCodeTimeOut;
		else
			readQRCodeBuffer->scanresult = CardCodeScanOK;
}


static MyRes CheckCardIsTimeOut(ReadQRCodeBuffer * readQRCodeBuffer)
{
	if(readQRCodeBuffer)
	{
		memcpy(&(readQRCodeBuffer->temptime), &(getSystemRunTimeData()->systemDateTime), sizeof(DateTime));
		
		if(readQRCodeBuffer->cardQR->CardBaoZhiQi.year == readQRCodeBuffer->temptime.year)
		{
			if(readQRCodeBuffer->cardQR->CardBaoZhiQi.month == readQRCodeBuffer->temptime.month)
			{
				if(readQRCodeBuffer->cardQR->CardBaoZhiQi.day >= readQRCodeBuffer->temptime.day)
					return My_Pass;
			}
			else if(readQRCodeBuffer->cardQR->CardBaoZhiQi.month > readQRCodeBuffer->temptime.month)
				return My_Pass;
		}
		else if(readQRCodeBuffer->cardQR->CardBaoZhiQi.year > readQRCodeBuffer->temptime.year)
			return My_Pass;
	}
	
	return My_Fail;
}

