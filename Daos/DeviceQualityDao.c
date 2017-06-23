/***************************************************************************************************
*FileName: DeviceAdjustDao
*Description: 读写设备校准记录
*Author: xsx_kair
*Data: 2017年6月2日 15:53:25
***************************************************************************************************/

/***************************************************************************************************/
/******************************************Header List********************************************/
/***************************************************************************************************/
#include	"DeviceQualityDao.h"

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

MyState_TypeDef writeDeviceQualityToFile(DeviceQuality * deviceQuality)
{
	FatfsFileInfo_Def * myfile = NULL;
	DeviceRecordHeader * deviceRecordHeader = NULL;
	MyState_TypeDef statues = My_Fail;
	
	myfile = MyMalloc(MyFileStructSize);
	deviceRecordHeader = MyMalloc(DeviceRecordHeaderStructSize);
	
	if(myfile && deviceQuality && deviceRecordHeader)
	{
		memset(myfile, 0, MyFileStructSize);
		memset(deviceRecordHeader, 0, DeviceRecordHeaderStructSize);
		
		myfile->res = f_open(&(myfile->file), DeviceQualityFileName, FA_OPEN_ALWAYS | FA_WRITE | FA_READ);
			
		if(FR_OK == myfile->res)
		{
			myfile->size = f_size(&(myfile->file));
			f_lseek(&(myfile->file), 0);
			
			//读取数据头
			myfile->res = f_read(&(myfile->file), deviceRecordHeader, DeviceRecordHeaderStructSize, &(myfile->br));
			if(deviceRecordHeader->crc != CalModbusCRC16Fun1(deviceRecordHeader, DeviceRecordHeaderStructCrcSize))
			{
				deviceRecordHeader->itemSize = 0;
				deviceRecordHeader->uploadIndex = 0;
				deviceRecordHeader->crc = CalModbusCRC16Fun1(deviceRecordHeader, DeviceRecordHeaderStructCrcSize);
			}
			
			//先写一次数据头，防止没有数据头写数据异常
			if(0 == myfile->size)
			{
				f_lseek(&(myfile->file), 0);
				myfile->res = f_write(&(myfile->file), deviceRecordHeader, DeviceRecordHeaderStructSize, &(myfile->bw));
				if((FR_OK != myfile->res) || (myfile->bw != DeviceRecordHeaderStructSize))
					goto Finally;
			}
			
			//写数据
			f_lseek(&(myfile->file), deviceRecordHeader->itemSize * DeviceQualityStructSize + DeviceRecordHeaderStructSize);
			myfile->res = f_write(&(myfile->file), deviceQuality, DeviceQualityStructSize, &(myfile->bw));

			if((FR_OK != myfile->res) || (myfile->bw != DeviceQualityStructSize))
				goto Finally;
			
			//更新数据头
			deviceRecordHeader->itemSize += 1;
			deviceRecordHeader->crc = CalModbusCRC16Fun1(deviceRecordHeader, DeviceRecordHeaderStructCrcSize);
			
			f_lseek(&(myfile->file), 0);
			myfile->res = f_write(&(myfile->file), deviceRecordHeader, DeviceRecordHeaderStructSize, &(myfile->bw));
			if((FR_OK == myfile->res) && (myfile->bw == DeviceRecordHeaderStructSize))
				statues = My_Pass;
			
			Finally:
				f_close(&(myfile->file));
		}
	}
	
	MyFree(myfile);
	MyFree(deviceRecordHeader);
	
	return statues;
}

MyState_TypeDef readDeviceQualityFromFile(DeviceQualityReadPackge * readPackge)
{
	FatfsFileInfo_Def * myfile = NULL;
	MyState_TypeDef statues = My_Fail;
	unsigned char i=0;
	
	myfile = MyMalloc(MyFileStructSize);

	if(myfile && readPackge)
	{
		memset(myfile, 0, MyFileStructSize);
		memset(&(readPackge->deviceRecordHeader), 0, DeviceRecordHeaderStructSize);
		memset(&(readPackge->deviceQuality), 0, DeviceQualityStructSize * DeviceQualityRecordPageShowNum);
		readPackge->readTotalNum = 0;
		
		myfile->res = f_open(&(myfile->file), DeviceQualityFileName, FA_READ);
		
		if(FR_OK == myfile->res)
		{
			myfile->size = f_size(&(myfile->file));
			
			f_lseek(&(myfile->file), 0);
			
			//读取数据头
			myfile->res = f_read(&(myfile->file), &(readPackge->deviceRecordHeader), DeviceRecordHeaderStructSize, &(myfile->br));
			if(readPackge->deviceRecordHeader.crc != CalModbusCRC16Fun1(&(readPackge->deviceRecordHeader), DeviceRecordHeaderStructCrcSize))
				goto Finally;
			
			//如果pageRequest的crc错误，表示是按照上传索引读取数据进行上传
			if(readPackge->pageRequest.crc != CalModbusCRC16Fun1(&(readPackge->pageRequest), PageRequestStructCrcSize))
			{
				if(readPackge->deviceRecordHeader.uploadIndex < readPackge->deviceRecordHeader.itemSize)
				{
					f_lseek(&(myfile->file), readPackge->deviceRecordHeader.uploadIndex * DeviceQualityStructSize + DeviceRecordHeaderStructSize);
				
					if((readPackge->deviceRecordHeader.itemSize - readPackge->deviceRecordHeader.uploadIndex) >= DeviceQualityRecordPageShowNum)
						f_read(&(myfile->file), readPackge->deviceQuality, DeviceQualityRecordPageShowNum * DeviceQualityStructSize, &(myfile->br));
					else
						f_read(&(myfile->file), readPackge->deviceQuality, (readPackge->deviceRecordHeader.itemSize - readPackge->deviceRecordHeader.uploadIndex) * DeviceQualityStructSize, &(myfile->br));
				}
			}
			//如果pageRequest的crc正确，表示是按照pageRequest的请求内容进行读取数据
			else if(readPackge->pageRequest.startElementIndex < readPackge->deviceRecordHeader.itemSize)
			{
				if(readPackge->pageRequest.pageSize > (readPackge->deviceRecordHeader.itemSize - readPackge->pageRequest.startElementIndex))
					readPackge->pageRequest.pageSize = (readPackge->deviceRecordHeader.itemSize - readPackge->pageRequest.startElementIndex);
					
				if(readPackge->pageRequest.orderType == DESC)
					myfile->res = f_lseek(&(myfile->file), (readPackge->pageRequest.startElementIndex) * DeviceQualityStructSize + DeviceRecordHeaderStructSize);
				else
					myfile->res = f_lseek(&(myfile->file), (readPackge->deviceRecordHeader.itemSize - (readPackge->pageRequest.pageSize + readPackge->pageRequest.startElementIndex)) * DeviceQualityStructSize + DeviceRecordHeaderStructSize);
					
				myfile->res = f_read(&(myfile->file), readPackge->deviceQuality, readPackge->pageRequest.pageSize * DeviceQualityStructSize, &(myfile->br));	
				
				for(i=0; i<DeviceQualityRecordPageShowNum; i++)
				{
					if(readPackge->deviceQuality[i].crc == CalModbusCRC16Fun1(&readPackge->deviceQuality[i], DeviceQualityStructCrcSize))
						readPackge->readTotalNum++;
				}
			}
			
			statues = My_Pass;
			
			Finally:
				f_close(&(myfile->file));
		}
	}
	MyFree(myfile);
	
	return statues;
}

MyState_TypeDef plusDeviceQualityHeaderUpLoadIndexToFile(unsigned int index)
{
	FatfsFileInfo_Def * myfile = NULL;
	DeviceRecordHeader * deviceRecordHeader = NULL;
	MyState_TypeDef statues = My_Fail;
	
	myfile = MyMalloc(MyFileStructSize);
	deviceRecordHeader = MyMalloc(DeviceRecordHeaderStructSize);

	if(myfile && deviceRecordHeader)
	{
		memset(myfile, 0, MyFileStructSize);

		myfile->res = f_open(&(myfile->file), DeviceQualityFileName, FA_OPEN_ALWAYS | FA_WRITE | FA_READ);
		
		if(FR_OK == myfile->res)
		{
			f_lseek(&(myfile->file), 0);

			myfile->res = f_read(&(myfile->file), deviceRecordHeader, DeviceRecordHeaderStructSize, &(myfile->br));
			if((FR_OK == myfile->res) && (DeviceRecordHeaderStructSize == myfile->br) 
				&& (deviceRecordHeader->crc == CalModbusCRC16Fun1(deviceRecordHeader, DeviceRecordHeaderStructCrcSize)))
			{
				deviceRecordHeader->uploadIndex += index;
				deviceRecordHeader->crc = CalModbusCRC16Fun1(deviceRecordHeader, DeviceRecordHeaderStructCrcSize);
				
				f_lseek(&(myfile->file), 0);
				myfile->res = f_write(&(myfile->file), deviceRecordHeader, DeviceRecordHeaderStructSize, &(myfile->bw));
				if((FR_OK == myfile->res)&&(myfile->bw == DeviceRecordHeaderStructSize))
					statues = My_Pass;
			}
			
			f_close(&(myfile->file));
		}
	}
	MyFree(myfile);
	MyFree(deviceRecordHeader);
	
	return statues;
}

MyState_TypeDef deleteDeviceQualityFile(void)
{
	FRESULT res;
	
	res = f_unlink(DeviceQualityFileName);
	
	if((FR_OK == res) || (FR_NO_FILE == res))
		return My_Pass;
	else
		return My_Fail;
}

/****************************************end of file************************************************/
