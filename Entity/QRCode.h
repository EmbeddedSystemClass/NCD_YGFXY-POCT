/****************************************file start****************************************************/
#ifndef _QRCODE_E_H
#define	_QRCODE_E_H

#include	"ItemConstData.h"
#include	"DateTime.h"

#define	FenDunNum	2
#define	QuXianNum	3
#define	QuXianParmNum	3
#define	PiHaoLen	15
#define	PiNumLen	10

#pragma pack(1)
typedef struct QRCode_Tag
{
	char			ItemName[ItemNameLen];						//������Ŀ
	unsigned short	ItemLocation;								//T��λ��
	unsigned char 	ChannelNum;									//ͨ����(0-7)
	float			ItemFenDuan[FenDunNum];						//�ֶη�߱�
	float			ItemBiaoQu[QuXianNum][QuXianParmNum];		//��׼����
			//2 --- ?2???
			//4 --- a , b, c d (????)
	unsigned char 	CardWaitTime;								//��⿨��Ӧʱ��
	unsigned short 	CLineLocation;								//c��λ��
	char			PiHao[PiHaoLen];							//���κ�
	char			piNum[PiNumLen];							//���ڱ��
	DateTime		CardBaoZhiQi;								//������
	ItemConstData 	itemConstData;								//����Ŀ�Ĺ̶�����
	unsigned short 	CRC16;										//crc
}QRCode;
#pragma pack()

#endif

/****************************************end of file************************************************/
