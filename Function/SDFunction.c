/***************************************************************************************************
*FileName:SDFunction
*Description:һЩSD���Ĳ�������
*Author:xsx
*Data:2016��4��30��16:06:36
***************************************************************************************************/


/***************************************************************************************************/
/******************************************ͷ�ļ�***************************************************/
/***************************************************************************************************/
#include	"SDFunction.h"
#include	"System_Data.h"
#include	"Timer_Data.h"

#include	"CRC16.h"
#include	"MyMem.h"
#include	"MyTools.h"

#include 	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"
#include	"semphr.h"

#include	<string.h>
#include	"stdio.h"
#include 	"stdlib.h"





/***************************************************************************************************/
/**************************************�ֲ���������*************************************************/
/***************************************************************************************************/




/***************************************************************************************************/
/**************************************�ֲ���������*************************************************/
/***************************************************************************************************/





/***************************************************************************************************/
/***************************************************************************************************/
/***************************************����********************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/



/***************************************************************************************************/
/***************************************************************************************************/
/***************************************wifi���뱣��************************************************/
/***************************************************************************************************/
/***************************************************************************************************/


/*********************************************************************************************/
/*********************************************************************************************/
/************************************д���ϻ�����*********************************************/
/*********************************************************************************************/
/*********************************************************************************************/

MyRes SavereTestData(ReTestData *reTestData, unsigned char type)
{
	FatfsFileInfo_Def * myfile = NULL;
	char *buf;
	MyRes statues = My_Fail;
	
	myfile = MyMalloc(sizeof(FatfsFileInfo_Def));
	buf = MyMalloc(1024);
	
	if(myfile && reTestData && buf)
	{
		memset(myfile, 0, sizeof(FatfsFileInfo_Def));
		
		myfile->res = f_open(&(myfile->file), "0:/laohua.csv", FA_OPEN_ALWAYS | FA_WRITE | FA_READ);
			
		if(FR_OK == myfile->res)
		{
			myfile->size = f_size(&(myfile->file));
			f_lseek(&(myfile->file), myfile->size);
			
			if(myfile->size == 0)
			{
				memset(buf, 0, 1024);
				sprintf(buf, "���Դ���,����ʱ��,����ʱ��(��),�������,[DA-AD],[DA-AD],[DA-AD],LED״̬,�����¶�,�������¶�,��⿨�¶�,[Tֵ-Tλ��],[Cֵ-Cλ��],[Bֵ-Bλ��],��߱�,ԭʼ���,У׼���,��ǰ��Ƶ��ʼʱ��,��ǰ��Ƶ����ʱ��,��ǰ��Ƶʱ��,��Ƶ��ʱ��,��Ƶ���Ŵ���\r");
				myfile->res = f_write(&(myfile->file), buf, strlen(buf), &(myfile->bw));
				if(FR_OK != myfile->res)
					goto END;
			}
			
			//�����������
			if(type == 0)
			{
				memset(buf, 0, 1024);
				sprintf(buf, "%d/%d,%d-%d-%d %d:%d:%d,%d,%s,[100-%.3f],[200-%.3f],[300-%.3f],%d,%.1f,%.1f,%.1f,[%d-%d],[%d-%d],[%d-%d],%.3f,%.3f,%.3f\r", reTestData->retestedcount, reTestData->retestcount, reTestData->paiduiUnitData.testData.testDateTime.year
					, reTestData->paiduiUnitData.testData.testDateTime.month, reTestData->paiduiUnitData.testData.testDateTime.day, reTestData->paiduiUnitData.testData.testDateTime.hour, reTestData->paiduiUnitData.testData.testDateTime.min, reTestData->paiduiUnitData.testData.testDateTime.sec
					, timer_Count(&(reTestData->oneretesttimer)), reTestData->result, reTestData->advalue1, reTestData->advalue2, reTestData->advalue3, reTestData->ledstatus, reTestData->paiduiUnitData.testData.temperature.E_Temperature, reTestData->paiduiUnitData.testData.temperature.I_Temperature
					, reTestData->paiduiUnitData.testData.temperature.O_Temperature, reTestData->paiduiUnitData.testData.testSeries.T_Point[0], reTestData->paiduiUnitData.testData.testSeries.T_Point[1], reTestData->paiduiUnitData.testData.testSeries.C_Point[0]
					, reTestData->paiduiUnitData.testData.testSeries.C_Point[1], reTestData->paiduiUnitData.testData.testSeries.B_Point[0], reTestData->paiduiUnitData.testData.testSeries.B_Point[1], reTestData->paiduiUnitData.testData.testSeries.BasicBili
					, reTestData->paiduiUnitData.testData.testSeries.BasicResult, reTestData->paiduiUnitData.testData.testSeries.AdjustResult);
				myfile->res = f_write(&(myfile->file), buf, strlen(buf), &(myfile->bw));
				
				if(FR_OK != myfile->res)
					goto END;
				
				statues = My_Pass;
			}
			//������Ƶ��������
			else
			{
				memset(buf, 0, 1024);
				sprintf(buf, ",,,,,,,,,,,,,,,,,%d-%d-%d %d:%d:%d,%d-%d-%d %d:%d:%d,%d,%d,%d\r", reTestData->startplayTime.year, reTestData->startplayTime.month, reTestData->startplayTime.day
					,reTestData->startplayTime.hour, reTestData->startplayTime.min, reTestData->startplayTime.sec, reTestData->endplayTime.year, reTestData->endplayTime.month
					, reTestData->endplayTime.day, reTestData->endplayTime.hour, reTestData->endplayTime.min, reTestData->endplayTime.sec, timer_Count(&(reTestData->oneplaytimer))
					, timer_Count(&(reTestData->playtimer)), reTestData->playcount);
				myfile->res = f_write(&(myfile->file), buf, strlen(buf), &(myfile->bw));
				
				if(FR_OK != myfile->res)
					goto END;
				
				statues = My_Pass;
			}
			
			END:
				f_close(&(myfile->file));
		}
	}
	
	MyFree(myfile);
	MyFree(buf);
	
	return statues;
}

