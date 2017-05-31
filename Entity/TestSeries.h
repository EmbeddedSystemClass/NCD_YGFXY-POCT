/****************************************file start****************************************************/
#ifndef _TESTSERIES_E_H
#define	_TESTSERIES_E_H

#include	"Define.h"

#pragma pack(1)
typedef struct TestSeries_tag {
	unsigned short TestPoint[MaxPointLen];
	unsigned short C_Point[2];
	unsigned short T_Point[2];
	unsigned short B_Point[2];
	float BasicBili;
	float BasicResult;
	float AdjustResult;
} TestSeries;
#pragma pack()

#endif

/****************************************end of file************************************************/
