/***************************************************************************************************
*FileName:SystemResetFun
*Description:	恢复出厂设置
*Author: xsx_kair
*Data:2017年2月16日11:16:38
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
*Description: 恢复出厂设置
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2017年2月16日11:20:46
***************************************************************************************************/
MyRes SystemReset(void)
{
	SystemSetData * systemSetData = NULL;
	
	systemSetData = MyMalloc(SystemSetDataStructSize * 2);
	if(systemSetData)
	{
		//读取当前设置
		memcpy(&(systemSetData[0]), getGBSystemSetData(), SystemSetDataStructSize);
		//恢复默认
		setDefaultSystemSetData(&(systemSetData[1]));
		
		//保留设备ID
		memcpy(systemSetData[1].deviceId, systemSetData[0].deviceId, DeviceIdLen);

		//保留已校准的led值
		systemSetData[1].testLedLightIntensity = systemSetData[0].testLedLightIntensity;
		
		//保留已校准的校准参数
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
	
	//删除设备信息，包含操作人
	if(My_Fail == deleteDeviceFile())
		return My_Fail;
	
	//删除wifi数据
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
