#ifndef __MAINTENANCERECORD_P_H__
#define __MAINTENANCERECORD_P_H__

#include	"DeviceMaintenance.h"
#include	"UI_Data.h"


typedef struct MaintenanceRecordPageBuffer_tag {

	unsigned char selectindex;							//选中的索引
	unsigned int pageindex;							//页面索引
	unsigned int maxpagenum;
	
	DeviceMaintenanceReadPackge deviceMaintenanceReadPackge;		//读取数据
	
	char buf[100];										//临时缓冲区
	char tempBuf[100];										//临时缓冲区
	unsigned int tempvalue1;
	unsigned int tempvalue2;
	DeviceMaintenance *tempDeviceMaintenance;
	unsigned short lcdinput[100];
}MaintenanceRecordPageBuffer;

MyState_TypeDef createMaintenanceRecordActivity(Activity * thizActivity, Intent * pram);

#endif

