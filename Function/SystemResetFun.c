/***************************************************************************************************
*FileName:SystemResetFun
*Description:	�ָ���������
*Author: xsx_kair
*Data:2017��2��16��11:16:38
***************************************************************************************************/

/***************************************************************************************************/
/******************************************Header List********************************************/
/***************************************************************************************************/
#include	"SystemResetFun.h"

#include	"SystemSet_Dao.h"
#include	"DeviceDao.h"
#include	"WifiDao.h"
#include	"DeviceQualityDao.h"
#include	"DeviceMaintenanceDao.h"
#include	"DeviceErrorDao.h"
#include	"DeviceAdjustDao.h"
#include	"TestDataDao.h"
#include	"RecordDataDao.h"

#include	"SystemSet_Data.h"

#include	"MyMem.h"
#include	"String.h"
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/****************************************File Start*************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************
*FunctionName: SystemReset
*Description: �ָ���������
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2017��2��16��11:20:46
***************************************************************************************************/
MyRes SystemReset(void)
{
	SystemSetData * systemSetData = NULL;
	
	systemSetData = MyMalloc(SystemSetDataStructSize * 2);
	if(systemSetData)
	{
		//��ȡ��ǰ����
		memcpy(&(systemSetData[0]), getGBSystemSetData(), SystemSetDataStructSize);
		//�ָ�Ĭ��
		setDefaultSystemSetData(&(systemSetData[1]));
		
		//�����豸ID
		memcpy(systemSetData[1].deviceId, systemSetData[0].deviceId, DeviceIdLen);

		//������У׼��ledֵ
		systemSetData[1].testLedLightIntensity = systemSetData[0].testLedLightIntensity;
		
		//������У׼��У׼����
		memcpy(systemSetData[1].adjustData, systemSetData[0].adjustData, AdjustDataStructSize * MaxAdjustItemNum);
		
		if(My_Pass != SaveSystemSetData(&systemSetData[1]))
		{
			MyFree(systemSetData);
			return My_Fail;
		}
		else
		{
			upDateSystemSetData(&systemSetData[1]);
			MyFree(systemSetData);
		}
	}
	else
		return My_Fail;
	
	//ɾ���豸��Ϣ������������
	if(My_Fail == deleteDeviceFile())
		return My_Fail;
	
	//ɾ��wifi����
	if(My_Fail == ClearWifi())
		return My_Fail;
	
	if(My_Fail == deleteRecordDataFile(TestDataFileName))
		return My_Fail;
	
	if(My_Fail == deleteDeviceAdjustFile())
		return My_Fail;
	
	if(My_Fail == deleteDeviceErrorFile())
		return My_Fail;
	
	if(My_Fail == deleteDeviceMaintenanceFile())
		return My_Fail;
	
	if(My_Fail == deleteDeviceQualityFile())
		return My_Fail;
	
	return My_Pass;
}



/****************************************end of file************************************************/
