/***************************************************************************************************
*FileName: DeviceAdjustDao
*Description: 读写设备校准记录
*Author: xsx_kair
*Data: 2017年6月2日 15:53:25
***************************************************************************************************/

/***************************************************************************************************/
/******************************************Header List********************************************/
/***************************************************************************************************/
#include	"DeviceAdjustDao.h"

#include	"CRC16.h"
#include	"MyMem.h"

#include	"ff.h"

#include	<string.h>
#include	"stdio.h"
#include 	"stdlib.h"
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/


/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/****************************************File Start*************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

MyState_TypeDef SaveDeviceAdjustToFile(DeviceAdjust * deviceAdjust)
{
	FatfsFileInfo_Def * myfile = NULL;
	MyState_TypeDef statues = My_Fail;
	
	myfile = MyMalloc(sizeof(FatfsFileInfo_Def));
	
	if(myfile && deviceAdjust)
	{
		memset(myfile, 0, sizeof(FatfsFileInfo_Def));

		myfile->res = f_open(&(myfile->file), DeviceAdjustFileName, FA_OPEN_ALWAYS | FA_WRITE | FA_READ);
			
		if(FR_OK == myfile->res)
		{
			myfile->size = f_size(&(myfile->file));
			
			f_lseek(&(myfile->file), myfile->size);
			
			myfile->res = f_write(&(myfile->file), deviceAdjust, DeviceAdjustStructSize, &(myfile->bw));
				
			if((FR_OK == myfile->res)&&(myfile->bw == DeviceAdjustStructSize))
				statues = My_Pass;
				
			f_close(&(myfile->file));
		}
	}
	
	MyFree(myfile);
	
	return statues;
}

MyState_TypeDef ReadDeviceAdjustFromFile(DeviceAdjust * deviceAdjust)
{
	FatfsFileInfo_Def * myfile = NULL;
	MyState_TypeDef statues = My_Fail;
	
	myfile = MyMalloc(sizeof(FatfsFileInfo_Def));

	if(myfile && deviceAdjust)
	{
		memset(myfile, 0, sizeof(FatfsFileInfo_Def));

		myfile->res = f_open(&(myfile->file), DeviceAdjustFileName, FA_READ);
		
		if(FR_OK == myfile->res)
		{
			myfile->size = f_size(&(myfile->file));
			
			f_lseek(&(myfile->file), myfile->size - DeviceAdjustStructSize);

			f_read(&(myfile->file), deviceAdjust, DeviceAdjustStructSize, &(myfile->br));
			
			statues = My_Pass;
			
			f_close(&(myfile->file));
		}
	}
	MyFree(myfile);
	
	return statues;
}

MyState_TypeDef deleteDeviceAdjustFile(void)
{
	FRESULT res;
	
	res = f_unlink(DeviceAdjustFileName);
	
	if((FR_OK == res) || (FR_NO_FILE == res))
		return My_Pass;
	else
		return My_Fail;
}

/****************************************end of file************************************************/
