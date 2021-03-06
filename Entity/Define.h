#ifndef _DEFINE_H_H
#define _DEFINE_H_H

#include	"stm32f4xx.h"
#include 	"FreeRTOS.h"
#include	"ff.h"

/*定义设备类型*/
#define		DeviceType		DeviceManual

/*第一代机器，手动加样，有外置8个排队位，排队需人工参与*/
#define		DeviceManual			1
/*第二代机器，半自动，外置转盘，手动加样，自动排队测试，无需人工参与*/
#define		DeviceSemiAutomatic		2

/*设置中各个功能的密码*/
#define		AdminPassWord				"201300\0"								//管理员密码，用于修改设备id
#define		AdjustPassWord				"201301\0"								//校准密码
#define		TestPassWord				"201302\0"								//老化测试密码
#define		CheckQRPassWord				"201303\0"								//测试二维码密码
#define		AdjLedPassWord				"201304\0"								//校准led密码
#define		FactoryResetPassWord		"201305\0"								//恢复出厂设置密码
#define		ChangeValueShowTypePassWord	"201306\0"								//切换结果显示模式，是否显示真实值
#define		UnlockLCDPassWord			"201307\0"								//解锁屏幕一次

/*V1.0.03*/
#define	GB_SoftVersion	(unsigned short)500
#define	GB_SoftVersion_Build	"Build17050101\0"

/*服务器信息*/
#define	NCD_ServerIp_1		116
#define	NCD_ServerIp_2		62
#define	NCD_ServerIp_3		108
#define	NCD_ServerIp_4		201
#define NCD_ServerPort		8080

#define	HttpResponeOK				"HTTP/1.1 200 OK\0"

//poct
#define	ReadTimeUrl					"DeviceReadTime\0"
#define	QueryDeviceByDeviceIdUrl	"DeviceQueryDeviceByDeviceId\0"
#define	UpLoadDeviceAdjustUrl		"UpLoadDeviceAdjust\0"
#define	UpLoadDeviceErrorUrl		"UpLoadDeviceError\0"
#define	UpLoadYGFXYDataUrl			"upLoadYGFXY_Data\0"
#define	UpLoadDeviceMaintenanceUrl	"UpLoadDeviceMaintenance\0"
#define	UpLoadDeviceQualityUrl		"UpLoadDeviceQuality\0"

//ncd server
#define	NcdUpLoadYGFXYDataUrl		"UpLoadYGFXY\0"
#define	QueryRemoteSoftVersionUrl	"QuerySoftInfo\0"
#define	DownRemoteSoftFileUrl		"DownloadSoftFile\0"

/*SD卡文件名*/
#define	TestDataFileName			"0:/TD.ncd\0"
#define	DeviceFileName				"0:/Device.ncd\0"
#define	DeviceAdjustFileName		"0:/Dadj.ncd\0"
#define	DeviceErrorFileName			"0:/Derr.ncd\0"
#define	DeviceMainenanceFileName	"0:/Dmai.ncd\0"
#define	DeviceQualityFileName		"0:/Dqua.ncd\0"


#define	Char_r		'\r'
#define	Char_n		'\n'
#define	Char_r_n	"\r\n"
/**********************************************************************************************************/
/******************************************操作结果变量*************************************************/
/**********************************************************************************************************/
typedef enum
{ 
	My_Pass = pdPASS,			//操作成功
	My_Fail = pdFAIL			//操作失败
}MyRes;

typedef enum
{ 
	true = 1,
	false = 0
}bool;

typedef enum
{ 
	LED_Error = 1,		//led错误
	LED_OK = 0			//led正常
}MyLEDCheck_TypeDef;
/**********************************************************************************************************/
/**********************************************************************************************************/
typedef enum
{ 
	NormalTestType = 0,																	//常规测试
	PaiDuiTestType = 1																	//排队测试
}TestType;
/**********************************************************************************************************/
/******************************************fatfs结构体定义*************************************************/
/**********************************************************************************************************/
/*此结构体用于在进行sd操作是申请内存*/
typedef struct
{
	FIL file;
	UINT br;
	UINT bw;
	FRESULT res;
	FSIZE_t size;
}FatfsFileInfo_Def; 

#define	MyFileStructSize sizeof(FatfsFileInfo_Def)
/**********************************************************************************************************/
/**********************************************************************************************************/

#define	PaiDuiWeiNum	8							//排队位置数目
typedef enum
{
	statusNull = 0,									//没开始,默认状态
	status_user = 1,								//选择操作人状态
	status_sample = 2,								//获取样品ID状态
	status_wait1 = 3,								//等待插卡1
	status_preread = 4,								//预读卡状态
	status_start = 5,								//启动排队
	status_outcard = 6,								//等待从卡槽拔出
	status_in_n = 7,								//倒计时状态 -- 等待插入排队位
	status_out_n = 8,								//倒计时状态 -- 等待拔出排队位
	status_incard_n = 9,							//倒计时状态 -- 等待插入卡槽
	status_timedown = 10,							//倒计时状态
	status_prereadagain_n = 11,						//倒计时状态 -- 再次预读卡，主要检测卡变更
	status_in_o = 12,								//超时状态 -- 等待插入排队位
	status_out_o = 13,								//超时状态 -- 等待拔出排队位
	status_incard_o = 14,							//超时状态 -- 等待插入卡槽
	status_timeup = 15,								//超时状态
	status_prereadagain_o = 16,						//超时状态 -- 再次预读卡，主要检测卡变更
	status_timedownagain = 17,						//倒计时界面
	status_test = 18,								//测试
}MyPaiDuiStatues;

/**********************************************************************************************************/
/**********************************************************************************************************/

/**********************************************************************************************************/
/**********************************************************************************************************/

/**********************************************************************************************************/
/******************************************二维码相关定义**************************************************/
/**********************************************************************************************************/


typedef enum
{
	CardCodeScanning = 0,											//正在扫描
	CardCodeScanOK = 1,												//扫描二维码成功
	CardCodeScanFail = 2,											//扫描失败
	CardCodeTimeOut = 3,											//过期
	CardCodeCardOut = 4,											//卡被拔出
	CardCodeScanTimeOut = 5,										//扫描超时
	CardCodeCRCError = 6,											//crc错误
	CardUnsupported = 7												//当前程序不支持
}ScanCodeResult;





#define	MaxLocation			6500						//最大行程
#define	StartTestLocation	1250							//测试起始位置
#define	EndTestLocation		4300						//测试结束为止
#define	WaittingCardLocation	MaxLocation					//等待插卡的位置
#define	AvregeNum		10								//平均值滤波个数
#define	FilterNum		5								//平滑滤波个数
#define	MaxPointLen		((EndTestLocation - StartTestLocation)/AvregeNum - FilterNum)	//测试点个数


/**********************************************************************************************************/
/**********************************************************************************************************/

/**********************************************************************************************************/
/******************************************样品ID相关定义**************************************************/
/**********************************************************************************************************/
#define	MaxSampleIDLen	20											//最笾С盅穒d长度为20
/**********************************************************************************************************/
/**********************************************************************************************************/

/**********************************************************************************************************/
/******************************************界面相关定义****************************************************/
/***************************************************************************�*******************************/

typedef struct LineInfo_Tag
{
	unsigned short MaxData;				//实时曲线最大的数值
	unsigned short Y_Scale;				//曲线y轴最低的刻度
	unsigned short MUL_Y;				//Y轴放大倍数
}LineInfo;

/**********************************************************************************************************/
/**********************************************************************************************************/

/**********************************************************************************************************/
/******************************************屏幕相关定义****************************************************/
/**********************************************************************************************************/
#define	LCD_Head_1		0xAA											//协议头1
#define	LCD_Head_2		0xBB											//协议头2

#define W_REGSITER 		0x80          									//写入数据,无应答          
#define R_REGSITER 		0x81          									//读取数据，有应答
#define W_ADDRESS  		0x82											//写变量寄存器，无应答
#define R_ADDRESS  		0x83											//读变量寄存器数据，有应答
#define W_LINE     		0x84          									//写曲线缓冲区，无应答

#pragma pack(1)
typedef struct BasicICO_Tag
{
	unsigned short X;
	unsigned short Y;
	unsigned short ICO_ID;
}Basic_ICO;
#pragma pack()
/**********************************************************************************************************/
/**********************************************************************************************************/

/**********************************************************************************************************/
/******************************************自检功能相关定义****************************************************/
/**********************************************************************************************************/



typedef enum
{
	SelfCheck_None = 0,													//自检未开始
	SelfChecking = 1,
	SelfCheck_OK = 2,													//自检正常
	SelfCheck_Error = 3,												//自检有不可忽略的错误
}SelfCheckStatus;
/**********************************************************************************************************/
/**********************************************************************************************************/
typedef enum
{
	DESC = 0,												//正序
	ASC = 1													//逆序
}OrderByEnum;

//读取请求信息
#pragma pack(1)
typedef struct PageRequest_tag {
	unsigned int startElementIndex;											//起始读取索引，0为第一个
	unsigned int pageSize;													//每页的数目
	OrderByEnum orderType;													//排序方式
	unsigned short crc;
}PageRequest;
#pragma pack()

#define	PageRequestStructSize		sizeof(PageRequest)
#define	PageRequestStructCrcSize	PageRequestStructSize - 2

#pragma pack(1)
typedef struct
{
	unsigned int itemSize;
	unsigned int userUpLoadIndex;
	unsigned int ncdUpLoadIndex;
	unsigned short crc;
}DeviceRecordHeader;
#pragma pack()

#define	DeviceRecordHeaderStructSize		sizeof(DeviceRecordHeader)								//最多保存的用户数目
#define	DeviceRecordHeaderStructCrcSize		DeviceRecordHeaderStructSize - 2						//最多保存的用户数目
/**********************************************************************************************************/
/******************************************网络相关定义****************************************************/
/**********************************************************************************************************/


#define	MaxSaveWifiNum	50												//最多保存50个热点
#define	MaxWifiListNum	20												//最多显示20个热点
#define	PageWifiNum		8
#define	MaxSSIDLen		50
#define	MaxKEYLen		20

/*wifi热点*/
#pragma pack(1)
typedef struct wifi_Tag
{
	char ssid[MaxSSIDLen];														//SSID
	char auth[20];														//认证模式
	char encry[10];														//加密方式
	unsigned char indicator;											//信号强度
	char key[MaxKEYLen];														//密码
	unsigned short crc;
}WIFI_Def;
#pragma pack()


typedef struct mynetbuf_tag
{
	void * data;
	unsigned short datalen;
}mynetbuf;

typedef enum
{ 
	LinkDown = 0,
	LinkUp = 1
}LinkStatus;



/*ip获取方式*/
typedef enum
{
	Dynamic_IP = 1,														//动态IP
	Static_IP = 2														//静态IP
}IPMode;







/**********************************************************************************************************/
/**********************************************************************************************************/

#define	MaxTestDataSaveNum	(unsigned int)1000000
	
/*********************************************************************************************/
/*********************************************************************************************/
/*********************************************************************************************/
/*******************************定时器********************************************************/
/*********************************************************************************************/
#pragma pack(1)
typedef struct
{
	unsigned int start;
	unsigned int interval;
}Timer;
#pragma pack()

typedef enum
{ 
	TimeNotTo = 0,		//计时时间未到
	TimeOut = 1,		//计时时间到
	TimeError = 2,		//错误
}TimerState_Def;

/*********************************************************************************************/
/*********************************************************************************************/
/*********************************************************************************************/
/**************************校准参数***********************************************************/
/*********************************************************************************************/
typedef enum
{
	NoResult = 0,										//还未测试，结果未null
	NoSample = 1,										//未加样
	PeakError = 2,										//检测卡没有C峰
	MemError = 3,										//内存错误
	TestInterrupt = 8,									//测试中断
	ResultIsOK = 99,									//测试成功
}ResultState;

typedef enum
{
	Connect_Ok = 1,
	Connect_Error = 0
}PaiduiModuleStatus;

typedef enum
{ 
	NoCard = 0,								//无卡
	CardIN = 1								//有卡
}CardState_Def;


#endif

