/****************************************file start****************************************************/
#ifndef _ITEMCONSTDATA_E_H
#define	_ITEMCONSTDATA_E_H


#define	ItemConstDataNum				4
#define	ItemNameLen						20
#define ItemUnitLen						20
#define	ItemNormalValueLen				50

#pragma pack(1)
typedef struct ItemConstData_Tag
{
	char itemName[ItemNameLen];									//项目名称
	unsigned short icoIndex;									//排队界面显示图标的索引
	unsigned char pointNum;										//小数点个数
	float lowstResult;											//最低检测线
	float highestResult;										//最高检测线
	char normalResult[ItemNormalValueLen];										//参考值
	char itemMeasure[ItemUnitLen];								//单位
}ItemConstData;
#pragma pack()


#endif

/****************************************end of file************************************************/
