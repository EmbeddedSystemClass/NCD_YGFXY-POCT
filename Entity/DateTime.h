/****************************************file start****************************************************/
#ifndef _DATETIME_E_H
#define	_DATETIME_E_H


#pragma pack(1)
typedef struct DateTime_Tag
{
	unsigned char year;
	unsigned char month;
	unsigned char day;
	unsigned char hour;
	unsigned char min;
	unsigned char sec;
}DateTime;
#pragma pack()

#define	DateTimeStructSize	sizeof(DateTime)

#endif

/****************************************end of file************************************************/

