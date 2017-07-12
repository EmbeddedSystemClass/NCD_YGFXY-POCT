/****************************************file start****************************************************/
#ifndef _APPFILE_D_H
#define	_APPFILE_D_H

#include	"Define.h"

MyRes WriteAppFile(char * file, unsigned short len, bool isNew);
MyRes ReadAppFile(unsigned int startAddr, unsigned char * dataBuf, unsigned short size, unsigned short *br,
	unsigned int *fileSize);
MyRes checkNewAppFileIsExist(void);
MyRes deleteAppFileIfExist(void);
#endif

/****************************************end of file************************************************/
