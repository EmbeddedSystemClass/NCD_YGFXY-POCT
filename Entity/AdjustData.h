/****************************************file start****************************************************/
#ifndef _ADJUSTDATA_E_H
#define	_ADJUSTDATA_E_H

#include	"ItemConstData.h"

#pragma pack(1)
typedef struct AdjustData_tag
{
	char ItemName[ItemNameLen];
	float parm;
}AdjustData;
#pragma pack()

#define	MaxAdjustItemNum	50
#define	AdjustDataStructSize	sizeof(AdjustData)

#endif

/****************************************end of file************************************************/
