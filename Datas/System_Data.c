/***************************************************************************************************
*FileName:
*Description:
*Author: xsx_kair
*Data:
***************************************************************************************************/

/***************************************************************************************************/
/******************************************Header List**********************************************/
/***************************************************************************************************/
#include	"System_Data.h"

#include	"CRC16.h"

#include	<string.h>
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

static PaiduiUnitData	S_PaiduiUnitData;										//��������,����Ŧ��������ʵ����ʹ��

static unsigned char S_TestStatus = 0;											//����״̬

static SystemData systemData=
{
	.systemDateTime = {0, 0, 0, 0, 0, 0},
	.enTemperature = 1,
	.paiduiModuleStatus = Connect_Error,
	.wireNetInfo = 
		{
			.ip = {0, 0, 0, 0},													//������ip
			.LineMAC = {0, 0, 0, 0, 0, 0},										//������MAC
			.lineStatus = LinkDown
		},
	.motorData = 
		{
			.location = 10000,													//�����ǰλ��
			.targetLocation = 0,												//Ŀ��λ��
			.motorDir = Reverse
		}
};
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/****************************************File Start*************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/*********************************************************************************************/
/*********************************************************************************************/
/*********************************************************************************************/
/*********************************************************************************************/
/*********************************************************************************************/


/*********************************************************************************************/
/*********************************************************************************************/
/*********************************************************************************************/
/*********************************************************************************************/
/*********************************************************************************************/
void * GetTestDataForLab(void)
{
	//��ϵͳ���������л�ȡ����ʱled������ֵ
	//S_PaiduiUnitData.ledLight = getTestLedLightIntensity(getGBSystemSetData());
	
	return &S_PaiduiUnitData;
}

void SetTestStatusFlorLab(unsigned char status)
{
	S_TestStatus = status;
}

unsigned char GetTestStatusFlorLab(void)
{
	return S_TestStatus;
}

/***************************************************************************************************
*FunctionName:  getSystemRunTimeData
*Description:  ֻ����ģʽ��ȡϵͳ����
*Input:  
*Output:  
*Return:  
*Author:  xsx
*Date: 2017��5��26�� 15:11:06
***************************************************************************************************/
const SystemData * getSystemRunTimeData(void)
{
	return &systemData;
}

/***************************************************************************************************
*FunctionName:  setSystemDateTime
*Description:  ����ϵͳʱ��
*Input:  
*Output:  
*Return:  
*Author:  xsx
*Date: 2017��5��26�� 15:13:29
***************************************************************************************************/
void setSystemDateTime(DateTime * dateTime)
{
	if(dateTime == NULL)
		return;
	
	vTaskSuspendAll();
	
	systemData.systemDateTime.year = dateTime->year;
	systemData.systemDateTime.month = dateTime->month;
	systemData.systemDateTime.day = dateTime->day;
	systemData.systemDateTime.hour = dateTime->hour;
	systemData.systemDateTime.min = dateTime->min;
	systemData.systemDateTime.sec = dateTime->sec;
	
	xTaskResumeAll();
}

/***************************************************************************************************
*FunctionName:  setSystemEnTemperature
*Description:  ���»����¶�
*Input:  
*Output:  
*Return:  
*Author:  xsx
*Date: 2017��5��26�� 15:14:45
***************************************************************************************************/
void setSystemEnTemperature(float temperature)
{

	vTaskSuspendAll();
	
	systemData.enTemperature = temperature;
	
	xTaskResumeAll();
}

/***************************************************************************************************
*FunctionName:  setSystemPaiduiModuleStatus
*Description:  �����Ŷ�ģ������״̬
*Input:  
*Output:  
*Return:  
*Author:  xsx
*Date: 2017��5��26�� 15:14:45
***************************************************************************************************/
void setSystemPaiduiModuleStatus(PaiduiModuleStatus status)
{

	vTaskSuspendAll();
	
	systemData.paiduiModuleStatus = status;
	
	xTaskResumeAll();
}

/***************************************************************************************************
*FunctionName:  setSystemPaiduiModuleStatus
*Description:  �����Ŷ�ģ������״̬
*Input:  
*Output:  
*Return:  
*Author:  xsx
*Date: 2017��5��26�� 15:14:45
***************************************************************************************************/
void setSystemWireIP(unsigned int ip)
{
	vTaskSuspendAll();
	
	systemData.wireNetInfo.ip.ip_4 = (ip>>24)&0xff;
	systemData.wireNetInfo.ip.ip_3 = (ip>>16)&0xff;
	systemData.wireNetInfo.ip.ip_2 = (ip>>8)&0xff;
	systemData.wireNetInfo.ip.ip_1 = ip&0xff;
	
	xTaskResumeAll();
}

void setSystemWireMac(unsigned char * mac)
{
	if(mac == NULL)
		return;
	
	vTaskSuspendAll();
	
	systemData.wireNetInfo.LineMAC[0] = mac[0];
	systemData.wireNetInfo.LineMAC[1] = mac[1];
	systemData.wireNetInfo.LineMAC[2] = mac[2];
	systemData.wireNetInfo.LineMAC[3] = mac[3];
	systemData.wireNetInfo.LineMAC[4] = mac[4];
	systemData.wireNetInfo.LineMAC[5] = mac[5];
	
	xTaskResumeAll();
}

void setSystemWireLinkStatus(LinkStatus linkStatus)
{
	vTaskSuspendAll();
	
	systemData.wireNetInfo.lineStatus = linkStatus;
	
	xTaskResumeAll();
}

void setSystemMotorLocation(unsigned short location)
{
	vTaskSuspendAll();
	
	systemData.motorData.location = location;
	
	xTaskResumeAll();
}

void setSystemMotorTargetLocation(unsigned short location)
{
	vTaskSuspendAll();
	
	systemData.motorData.targetLocation = location;
	
	xTaskResumeAll();
}

void setSystemMotorDir(DRVDir dir)
{
	vTaskSuspendAll();
	
	systemData.motorData.motorDir = dir;
	
	xTaskResumeAll();
}

/****************************************end of file************************************************/
