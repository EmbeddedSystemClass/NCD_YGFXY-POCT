#ifndef __MAINTENANCE_P_H__
#define __MAINTENANCE_P_H__

#include	"UI_Data.h"
#include	"DeviceMaintenance.h"

typedef struct MaintenancePageBuffer_tag {
	unsigned short lcdinput[100];
	DeviceMaintenance * deviceMaintenance;
	char tempBuf[50];
}MaintenancePageBuffer;

#define	MaintenancePageBufferStructSize	sizeof(MaintenancePageBuffer)

MyState_TypeDef createMaintenanceActivity(Activity * thizActivity, Intent * pram);

#endif

