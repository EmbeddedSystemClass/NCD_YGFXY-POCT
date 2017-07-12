/****************************************file start****************************************************/
#ifndef _MYLOCK_H
#define	_MYLOCK_H

#include	"Define.h"

#include 	"FreeRTOS.h"
#include 	"queue.h"
#include	"semphr.h"

#pragma pack(1)
typedef struct MyLock_Tag
{
	void * ownerId;							//拥有者id，0 -- 没有拥有者表明没上锁， 1-255有效
}MyLock;
#pragma pack()

MyRes LockObject(MyLock * myLock, void * ownerId, unsigned char cnt);
MyRes UnLockObject(MyLock * myLock, void * ownerId);

#endif

/****************************************end of file***************************************************/
