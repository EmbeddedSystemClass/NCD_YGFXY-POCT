#ifndef	_HTTPPOST_T_H_H
#define _HTTPPOST_T_H_H

#include	"Define.h"
#include	"tcp.h"
#include 	"api.h" 

#define	SERVERSENDBUFLEN	4096
#define	SERVERRECVBUFLEN	1024

typedef struct
{
	void * tempPoint;											//��ʱָ��
	char sendBuf[SERVERSENDBUFLEN];						//�������ݻ�����
	unsigned short sendBufferLen;							//�������ݳ���
	char recvBuf[SERVERRECVBUFLEN];						//�������ݻ�����
	unsigned int recvBufferLen;							//�������ݳ���
	
	struct ip_addr server_ip;						//������ip
	unsigned short server_port;							//�������˿ں�
	struct netconn *clientconn;							//��ǰ�ͻ���
	struct netbuf *recvNetBuf;								//���ջ�����
	struct pbuf *q;										//�������ݵ�����
}HttpPostBuffer;

void PostData(HttpPostBuffer * httpPostBuffer);

#endif
