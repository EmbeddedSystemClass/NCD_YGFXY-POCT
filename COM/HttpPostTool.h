#ifndef	_HTTPPOST_T_H_H
#define _HTTPPOST_T_H_H

#include	"Define.h"
#include	"tcp.h"
#include 	"api.h" 

#define	SERVERSENDBUFLEN	2048
#define	SERVERRECVBUFLEN	4096

typedef struct
{
	void * tempPoint;											//临时指针
	char sendBuf[SERVERSENDBUFLEN];						//发送数据缓冲区
	unsigned short sendBufferLen;							//发送数据长度
	char recvBuf[SERVERRECVBUFLEN];						//发送数据缓冲区
	unsigned short recvBufferLen;							//发送数据长度
	
	struct ip_addr server_ip;						//服务器ip
	unsigned short server_port;							//服务器端口号
	struct netconn *clientconn;							//当前客户端
	struct netbuf *recvNetBuf;								//接收缓冲区
	struct pbuf *q;										//接收数据的链表
}HttpPostBuffer;

MyState_TypeDef PostData(HttpPostBuffer * httpPostBuffer);

#endif
