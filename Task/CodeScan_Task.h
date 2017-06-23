#ifndef __CODE_T_H__
#define __CODE_T_H__

#include	"Define.h"
#include	"QRCode.h"

char StartCodeScanTask(void);
MyState_TypeDef StartScanQRCode(QRCode * parm);
MyState_TypeDef TakeScanQRCodeResult(ScanCodeResult *scanresult);
void clearScanQRCodeResult(void);

#endif

