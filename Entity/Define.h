#ifndef _DEFINE_H_H
#define _DEFINE_H_H

#include	"stm32f4xx.h"
#include 	"FreeRTOS.h"
#include	"ff.h"

/*¶¨ÒåÉè±¸ÀàĞÍ*/
#define		DeviceType		DeviceManual

/*µÚÒ»´ú»úÆ÷£¬ÊÖ¶¯¼ÓÑù£¬ÓĞÍâÖÃ8¸öÅÅ¶ÓÎ»£¬ÅÅ¶ÓĞèÈË¹¤²ÎÓë*/
#define		DeviceManual			1
/*µÚ¶ş´ú»úÆ÷£¬°ë×Ô¶¯£¬ÍâÖÃ×ªÅÌ£¬ÊÖ¶¯¼ÓÑù£¬×Ô¶¯ÅÅ¶Ó²âÊÔ£¬ÎŞĞèÈË¹¤²ÎÓë*/
#define		DeviceSemiAutomatic		2

/*ÉèÖÃÖĞ¸÷¸ö¹¦ÄÜµÄÃÜÂë*/
#define		AdminPassWord				"201300\0"								//¹ÜÀíÔ±ÃÜÂë£¬ÓÃÓÚĞŞ¸ÄÉè±¸id
#define		AdjustPassWord				"201301\0"								//Ğ£×¼ÃÜÂë
#define		TestPassWord				"201302\0"								//ÀÏ»¯²âÊÔÃÜÂë
#define		CheckQRPassWord				"201303\0"								//²âÊÔ¶şÎ¬ÂëÃÜÂë
#define		AdjLedPassWord				"201304\0"								//Ğ£×¼ledÃÜÂë
#define		FactoryResetPassWord		"201305\0"								//»Ö¸´³ö³§ÉèÖÃÃÜÂë
#define		ChangeValueShowTypePassWord	"201306\0"								//ÇĞ»»½á¹ûÏÔÊ¾Ä£Ê½£¬ÊÇ·ñÏÔÊ¾ÕæÊµÖµ
#define		UnlockLCDPassWord			"201307\0"								//½âËøÆÁÄ»Ò»´Î

/*V1.0.03*/
#define	GB_SoftVersion	(unsigned short)500
#define	GB_SoftVersion_Build	"Build17050101\0"

/*·şÎñÆ÷ĞÅÏ¢*/
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

/*SD¿¨ÎÄ¼şÃû*/
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
/******************************************²Ù×÷½á¹û±äÁ¿*************************************************/
/**********************************************************************************************************/
typedef enum
{ 
	My_Pass = pdPASS,			//²Ù×÷³É¹¦
	My_Fail = pdFAIL			//²Ù×÷Ê§°Ü
}MyRes;

typedef enum
{ 
	true = 1,
	false = 0
}bool;

typedef enum
{ 
	LED_Error = 1,		//led´íÎó
	LED_OK = 0			//ledÕı³£
}MyLEDCheck_TypeDef;
/**********************************************************************************************************/
/**********************************************************************************************************/
typedef enum
{ 
	NormalTestType = 0,																	//³£¹æ²âÊÔ
	PaiDuiTestType = 1																	//ÅÅ¶Ó²âÊÔ
}TestType;
/**********************************************************************************************************/
/******************************************fatfs½á¹¹Ìå¶¨Òå*************************************************/
/**********************************************************************************************************/
/*´Ë½á¹¹ÌåÓÃÓÚÔÚ½øĞĞsd²Ù×÷ÊÇÉêÇëÄÚ´æ*/
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

#define	PaiDuiWeiNum	8							//ÅÅ¶ÓÎ»ÖÃÊıÄ¿
typedef enum
{
	statusNull = 0,									//Ã»¿ªÊ¼,Ä¬ÈÏ×´Ì¬
	status_user = 1,								//Ñ¡Ôñ²Ù×÷ÈË×´Ì¬
	status_sample = 2,								//»ñÈ¡ÑùÆ·ID×´Ì¬
	status_wait1 = 3,								//µÈ´ı²å¿¨1
	status_preread = 4,								//Ô¤¶Á¿¨×´Ì¬
	status_start = 5,								//Æô¶¯ÅÅ¶Ó
	status_outcard = 6,								//µÈ´ı´Ó¿¨²Û°Î³ö
	status_in_n = 7,								//µ¹¼ÆÊ±×´Ì¬ -- µÈ´ı²åÈëÅÅ¶ÓÎ»
	status_out_n = 8,								//µ¹¼ÆÊ±×´Ì¬ -- µÈ´ı°Î³öÅÅ¶ÓÎ»
	status_incard_n = 9,							//µ¹¼ÆÊ±×´Ì¬ -- µÈ´ı²åÈë¿¨²Û
	status_timedown = 10,							//µ¹¼ÆÊ±×´Ì¬
	status_prereadagain_n = 11,						//µ¹¼ÆÊ±×´Ì¬ -- ÔÙ´ÎÔ¤¶Á¿¨£¬Ö÷Òª¼ì²â¿¨±ä¸ü
	status_in_o = 12,								//³¬Ê±×´Ì¬ -- µÈ´ı²åÈëÅÅ¶ÓÎ»
	status_out_o = 13,								//³¬Ê±×´Ì¬ -- µÈ´ı°Î³öÅÅ¶ÓÎ»
	status_incard_o = 14,							//³¬Ê±×´Ì¬ -- µÈ´ı²åÈë¿¨²Û
	status_timeup = 15,								//³¬Ê±×´Ì¬
	status_prereadagain_o = 16,						//³¬Ê±×´Ì¬ -- ÔÙ´ÎÔ¤¶Á¿¨£¬Ö÷Òª¼ì²â¿¨±ä¸ü
	status_timedownagain = 17,						//µ¹¼ÆÊ±½çÃæ
	status_test = 18,								//²âÊÔ
}MyPaiDuiStatues;

/**********************************************************************************************************/
/**********************************************************************************************************/

/**********************************************************************************************************/
/**********************************************************************************************************/

/**********************************************************************************************************/
/******************************************¶şÎ¬ÂëÏà¹Ø¶¨Òå**************************************************/
/**********************************************************************************************************/


typedef enum
{
	CardCodeScanning = 0,											//ÕıÔÚÉ¨Ãè
	CardCodeScanOK = 1,												//É¨Ãè¶şÎ¬Âë³É¹¦
	CardCodeScanFail = 2,											//É¨ÃèÊ§°Ü
	CardCodeTimeOut = 3,											//¹ıÆÚ
	CardCodeCardOut = 4,											//¿¨±»°Î³ö
	CardCodeScanTimeOut = 5,										//É¨Ãè³¬Ê±
	CardCodeCRCError = 6,											//crc´íÎó
	CardUnsupported = 7												//µ±Ç°³ÌĞò²»Ö§³Ö
}ScanCodeResult;





#define	MaxLocation			6500						//×î´óĞĞ³Ì
#define	StartTestLocation	1250							//²âÊÔÆğÊ¼Î»ÖÃ
#define	EndTestLocation		4300						//²âÊÔ½áÊøÎªÖ¹
#define	WaittingCardLocation	MaxLocation					//µÈ´ı²å¿¨µÄÎ»ÖÃ
#define	AvregeNum		10								//Æ½¾ùÖµÂË²¨¸öÊı
#define	FilterNum		5								//Æ½»¬ÂË²¨¸öÊı
#define	MaxPointLen		((EndTestLocation - StartTestLocation)/AvregeNum - FilterNum)	//²âÊÔµã¸öÊı


/**********************************************************************************************************/
/**********************************************************************************************************/

/**********************************************************************************************************/
/******************************************ÑùÆ·IDÏà¹Ø¶¨Òå**************************************************/
/**********************************************************************************************************/
#define	MaxSampleIDLen	20											//×îóÖ§³ÖÑùÆ·id³¤¶ÈÎª20
/**********************************************************************************************************/
/**********************************************************************************************************/

/**********************************************************************************************************/
/******************************************½çÃæÏà¹Ø¶¨Òå****************************************************/
/***************************************************************************´*******************************/

typedef struct LineInfo_Tag
{
	unsigned short MaxData;				//ÊµÊ±ÇúÏß×î´óµÄÊıÖµ
	unsigned short Y_Scale;				//ÇúÏßyÖá×îµÍµÄ¿Ì¶È
	unsigned short MUL_Y;				//YÖá·Å´ó±¶Êı
}LineInfo;

/**********************************************************************************************************/
/**********************************************************************************************************/

/**********************************************************************************************************/
/******************************************ÆÁÄ»Ïà¹Ø¶¨Òå****************************************************/
/**********************************************************************************************************/
#define	LCD_Head_1		0xAA											//Ğ­ÒéÍ·1
#define	LCD_Head_2		0xBB											//Ğ­ÒéÍ·2

#define W_REGSITER 		0x80          									//Ğ´ÈëÊı¾İ,ÎŞÓ¦´ğ          
#define R_REGSITER 		0x81          									//¶ÁÈ¡Êı¾İ£¬ÓĞÓ¦´ğ
#define W_ADDRESS  		0x82											//Ğ´±äÁ¿¼Ä´æÆ÷£¬ÎŞÓ¦´ğ
#define R_ADDRESS  		0x83											//¶Á±äÁ¿¼Ä´æÆ÷Êı¾İ£¬ÓĞÓ¦´ğ
#define W_LINE     		0x84          									//Ğ´ÇúÏß»º³åÇø£¬ÎŞÓ¦´ğ

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
/******************************************×Ô¼ì¹¦ÄÜÏà¹Ø¶¨Òå****************************************************/
/**********************************************************************************************************/



typedef enum
{
	SelfCheck_None = 0,													//×Ô¼ìÎ´¿ªÊ¼
	SelfChecking = 1,
	SelfCheck_OK = 2,													//×Ô¼ìÕı³£
	SelfCheck_Error = 3,												//×Ô¼ìÓĞ²»¿ÉºöÂÔµÄ´íÎó
}SelfCheckStatus;
/**********************************************************************************************************/
/**********************************************************************************************************/
typedef enum
{
	DESC = 0,												//ÕıĞò
	ASC = 1													//ÄæĞò
}OrderByEnum;

//¶ÁÈ¡ÇëÇóĞÅÏ¢
#pragma pack(1)
typedef struct PageRequest_tag {
	unsigned int startElementIndex;											//ÆğÊ¼¶ÁÈ¡Ë÷Òı£¬0ÎªµÚÒ»¸ö
	unsigned int pageSize;													//Ã¿Ò³µÄÊıÄ¿
	OrderByEnum orderType;													//ÅÅĞò·½Ê½
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

#define	DeviceRecordHeaderStructSize		sizeof(DeviceRecordHeader)								//×î¶à±£´æµÄÓÃ»§ÊıÄ¿
#define	DeviceRecordHeaderStructCrcSize		DeviceRecordHeaderStructSize - 2						//×î¶à±£´æµÄÓÃ»§ÊıÄ¿
/**********************************************************************************************************/
/******************************************ÍøÂçÏà¹Ø¶¨Òå****************************************************/
/**********************************************************************************************************/


#define	MaxSaveWifiNum	50												//×î¶à±£´æ50¸öÈÈµã
#define	MaxWifiListNum	20												//×î¶àÏÔÊ¾20¸öÈÈµã
#define	PageWifiNum		8
#define	MaxSSIDLen		50
#define	MaxKEYLen		20

/*wifiÈÈµã*/
#pragma pack(1)
typedef struct wifi_Tag
{
	char ssid[MaxSSIDLen];														//SSID
	char auth[20];														//ÈÏÖ¤Ä£Ê½
	char encry[10];														//¼ÓÃÜ·½Ê½
	unsigned char indicator;											//ĞÅºÅÇ¿¶È
	char key[MaxKEYLen];														//ÃÜÂë
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



/*ip»ñÈ¡·½Ê½*/
typedef enum
{
	Dynamic_IP = 1,														//¶¯Ì¬IP
	Static_IP = 2														//¾²Ì¬IP
}IPMode;







/**********************************************************************************************************/
/**********************************************************************************************************/

#define	MaxTestDataSaveNum	(unsigned int)1000000
	
/*********************************************************************************************/
/*********************************************************************************************/
/*********************************************************************************************/
/*******************************¶¨Ê±Æ÷********************************************************/
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
	TimeNotTo = 0,		//¼ÆÊ±Ê±¼äÎ´µ½
	TimeOut = 1,		//¼ÆÊ±Ê±¼äµ½
	TimeError = 2,		//´íÎó
}TimerState_Def;

/*********************************************************************************************/
/*********************************************************************************************/
/*********************************************************************************************/
/**************************Ğ£×¼²ÎÊı***********************************************************/
/*********************************************************************************************/
typedef enum
{
	NoResult = 0,										//»¹Î´²âÊÔ£¬½á¹ûÎ´null
	NoSample = 1,										//Î´¼ÓÑù
	PeakError = 2,										//¼ì²â¿¨Ã»ÓĞC·å
	MemError = 3,										//ÄÚ´æ´íÎó
	TestInterrupt = 8,									//²âÊÔÖĞ¶Ï
	ResultIsOK = 99,									//²âÊÔ³É¹¦
}ResultState;

typedef enum
{
	Connect_Ok = 1,
	Connect_Error = 0
}PaiduiModuleStatus;

typedef enum
{ 
	NoCard = 0,								//ÎŞ¿¨
	CardIN = 1								//ÓĞ¿¨
}CardState_Def;


#endif

