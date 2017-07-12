/***************************************************************************************************
*FileName:	SystemSet_Data
*Description:	系统设置参数
*Author: xsx_kair
*Data:	2016年12月16日16:20:43
***************************************************************************************************/

/***************************************************************************************************/
/******************************************Header List********************************************/
/***************************************************************************************************/
#include	"SystemSet_Data.h"

#include	"MyTools.h"
#include	"CRC16.h"

#include	<string.h>
#include	"stdio.h"
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
static SystemSetData GBSystemSetData;								//系统参数


static bool isShowRealValue = false;								//是否显示真实数据
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
*FunctionName: setDefaultSystemSetData
*Description: 初始化为默认配置
*Input: systemSetData -- 需要重置的系统设置地址
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月19日09:17:30
***************************************************************************************************/
void setDefaultSystemSetData(SystemSetData * systemSetData)
{
	if(systemSetData)
	{
		memset(systemSetData, 0, SystemSetDataStructSize);
		
		snprintf(systemSetData->deviceId, DeviceIdLen+1, "NCD-Device");
		
		systemSetData->isAutoPrint = true;
		systemSetData->isMute = false;
		systemSetData->ledLightIntensity = 100;
		systemSetData->ledSleepTime = 60;

		systemSetData->wireNetSet.ipMode = Dynamic_IP;
		
		
		systemSetData->testLedLightIntensity = 200;
		
		systemSetData->crc = CalModbusCRC16Fun(systemSetData, SystemSetDataStructCrcSize, NULL);
	}
}

const SystemSetData * getGBSystemSetData(void)
{
	return &GBSystemSetData;
}


void upDateSystemSetData(SystemSetData * systemSetData)
{
	if(systemSetData)
	{
		vTaskSuspendAll();
		
		memcpy(&GBSystemSetData, systemSetData, SystemSetDataStructSize);
		
		xTaskResumeAll();
	}
}

/***************************************************************************************************
*FunctionName: addAdjPram
*Description: 保存一个校准参数，先查找是否已存在，有则覆盖，无则添加
*Input: adjData -- 校准参数
*Output: 
*Return: MyRes -- fail表示无空闲位置保存
*Author: xsx
*Date: 2017年2月8日14:47:14
***************************************************************************************************/
MyRes addAdjPram(SystemSetData * systemSetData, AdjustData * adjData)
{
	unsigned char i = 0;
	
	if(adjData == NULL)
		return My_Fail;
	
	if(systemSetData->crc != CalModbusCRC16Fun(systemSetData, SystemSetDataStructCrcSize, NULL))
		setDefaultSystemSetData(systemSetData);
	
	//查找是否存在
	for(i=0; i<MaxAdjustItemNum; i++)
	{
		//存在，替换
		if(CheckStrIsSame(systemSetData->adjustData[i].ItemName, adjData->ItemName, ItemNameLen) == true)
		{
			systemSetData->adjustData[i].parm = adjData->parm;
			break;
		}
	}
	
	//不存在
	if(i >= MaxAdjustItemNum)
	{
		//找空闲位置，添加
		for(i=0; i<MaxAdjustItemNum; i++)
		{
			//名字的第一个字符是\0表明空
			if(systemSetData->adjustData[i].ItemName[0] == 0)
			{
				memcpy(&(systemSetData->adjustData[i]), adjData, sizeof(AdjustData));
				break;
			}
		}
	}
	
	//保存校准数目已满
	if(i >= MaxAdjustItemNum)
		return My_Fail;
	else
		return My_Pass;
}

/***************************************************************************************************
*FunctionName: getAdjPram
*Description: 读取一个项目的校准参数
*Input: systemSetData -- 读取源， adjData -- 读取参数目标地址
*Output: 
*Return: 
*Author: xsx
*Date: 2017年2月8日15:01:17
***************************************************************************************************/
void getAdjPram(const SystemSetData * systemSetData, AdjustData * adjData)
{
	unsigned char i = 0;
	
	if(adjData == NULL)
		return;
	
	if(systemSetData == NULL)
		return;
	
	adjData->parm = 0;
	
	if(systemSetData->crc != CalModbusCRC16Fun(systemSetData, SystemSetDataStructCrcSize, NULL))
	{
		adjData->parm = 1;
		return;
	}
	
	//查找是否存在
	for(i=0; i<MaxAdjustItemNum; i++)
	{
		if(CheckStrIsSame(systemSetData->adjustData[i].ItemName, adjData->ItemName, ItemNameLen) == true)
		{
			adjData->parm = systemSetData->adjustData[i].parm;
			break;
		}
	}
	
	if((i >= MaxAdjustItemNum) || (adjData->parm <= 0))
		adjData->parm = 1;
}



/***************************************************************************************************
*FunctionName: setIsShowRealValue, IsShowRealValue
*Description: 是否显示真实值
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2017年2月27日14:04:02
***************************************************************************************************/
void setIsShowRealValue(bool isShow)
{
	isShowRealValue = isShow;
}

bool IsShowRealValue(void)
{
	return isShowRealValue;
}
/****************************************end of file************************************************/
