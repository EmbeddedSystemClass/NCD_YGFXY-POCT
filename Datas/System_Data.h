/****************************************file start****************************************************/
#ifndef	SYSTEM_D_H
#define	SYSTEM_D_H

#include	"NetWork.h"
#include	"DateTime.h"
#include	"PaiduiUnitData.h"
#include	"MotorData.h"

#pragma pack(1)
typedef struct
{
	DateTime systemDateTime;
	float enTemperature;
	PaiduiModuleStatus paiduiModuleStatus;
	WireNetInfo wireNetInfo;
	MotorData motorData;
}SystemData;
#pragma pack()

void * GetTestDataForLab(void);
void SetTestStatusFlorLab(unsigned char status);
unsigned char GetTestStatusFlorLab(void);

const SystemData * getSystemRunTimeData(void);
void setSystemDateTime(DateTime * dateTime);
void setSystemEnTemperature(float temperature);
void setSystemPaiduiModuleStatus(PaiduiModuleStatus status);
void setSystemWireIP(unsigned int ip);

void setSystemWireMac(unsigned char * mac);

void setSystemWireLinkStatus(LinkStatus linkStatus);

void setSystemMotorLocation(unsigned short location);

void setSystemMotorTargetLocation(unsigned short location);

void setSystemMotorDir(DRVDir dir);
	
#endif

/****************************************end of file************************************************/

