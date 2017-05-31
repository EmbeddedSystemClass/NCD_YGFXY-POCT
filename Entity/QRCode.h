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
	char			ItemName[ItemNameLen];						//测试项目
	unsigned short	ItemLocation;								//T线位置
	unsigned char 	ChannelNum;									//通道号(0-7)
	float			ItemFenDuan[FenDunNum];						//分段峰高比
	float			ItemBiaoQu[QuXianNum][QuXianParmNum];		//标准曲线
			//2 --- ?2???
			//4 --- a , b, c d (????)
	unsigned char 	CardWaitTime;								//检测卡反应时间
	unsigned short 	CLineLocation;								//c线位置
	char			PiHao[PiHaoLen];							//批次号
	char			piNum[PiNumLen];							//批内编号
	DateTime		CardBaoZhiQi;								//保质期
	ItemConstData 	itemConstData;								//此项目的固定数据
	unsigned short 	CRC16;										//crc
}QRCode;
#pragma pack()

#endif

/****************************************end of file************************************************/
