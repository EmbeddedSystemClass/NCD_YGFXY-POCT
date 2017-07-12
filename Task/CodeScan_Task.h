#ifndef __CODE_T_H__
#define __CODE_T_H__

#include	"Define.h"
#include	"QRCode.h"

char StartCodeScanTask(void);
MyRes StartScanQRCode(QRCode * parm);
MyRes TakeScanQRCodeResult(ScanCodeResult *scanresult);
void clearScanQRCodeResult(void);

#endif

