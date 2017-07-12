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

MyRes writeDeviceAdjustToFile(DeviceAdjust * deviceAdjust)
{
	FatfsFileInfo_Def * myfile = NULL;
	DeviceRecordHeader * deviceRecordHeader = NULL;
	MyRes statues = My_Fail;
	
	myfile = MyMalloc(MyFileStructSize);
	deviceRecordHeader = MyMalloc(DeviceRecordHeaderStructSize);
	
	if(myfile && deviceAdjust && deviceRecordHeader)
	{
		memset(myfile, 0, MyFileStructSize);
		memset(deviceRecordHeader, 0, DeviceRecordHeaderStructSize);
		
		myfile->res = f_open(&(myfile->file), DeviceAdjustFileName, FA_OPEN_ALWAYS | FA_WRITE | FA_READ);
			
		if(FR_OK == myfile->res)
		{
			myfile->size = f_size(&(myfile->file));
			f_lseek(&(myfile->file), 0);
			
			//读取数据头
			myfile->res = f_read(&(myfile->file), deviceRecordHeader, DeviceRecordHeaderStructSize, &(myfile->br));
			if(deviceRecordHeader->crc != CalModbusCRC16Fun(deviceRecordHeader, DeviceRecordHeaderStructCrcSize, NULL))
			{
				deviceRecordHeader->itemSize = 0;
				deviceRecordHeader->userUpLoadIndex = 0;
				deviceRecordHeader->crc = CalModbusCRC16Fun(deviceRecordHeader, DeviceRecordHeaderStructCrcSize, NULL);
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
			f_lseek(&(myfile->file), deviceRecordHeader->itemSize * DeviceAdjustStructSize + DeviceRecordHeaderStructSize);
			myfile->res = f_write(&(myfile->file), deviceAdjust, DeviceAdjustStructSize, &(myfile->bw));

			if((FR_OK != myfile->res) || (myfile->bw != DeviceAdjustStructSize))
				goto Finally;
			
			//更新数据头
			deviceRecordHeader->itemSize += 1;
			deviceRecordHeader->crc = CalModbusCRC16Fun(deviceRecordHeader, DeviceRecordHeaderStructCrcSize, NULL);
			
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

MyRes readDeviceAdjustFromFile(DeviceAdjustReadPackge * deviceAdjustReadPackge)
{
	FatfsFileInfo_Def * myfile = NULL;
	MyRes statues = My_Fail;
	unsigned char i=0;
	
	myfile = MyMalloc(MyFileStructSize);

	if(myfile && deviceAdjustReadPackge)
	{
		memset(myfile, 0, MyFileStructSize);
		memset(&(deviceAdjustReadPackge->deviceRecordHeader), 0, DeviceRecordHeaderStructSize);
		memset(&(deviceAdjustReadPackge->deviceAdjust), 0, DeviceAdjustStructSize * DeviceAdjustRecordPageShowNum);
		deviceAdjustReadPackge->readTotalNum = 0;
		
		myfile->res = f_open(&(myfile->file), DeviceAdjustFileName, FA_READ);
		
		if(FR_OK == myfile->res)
		{
			myfile->size = f_size(&(myfile->file));
			
			f_lseek(&(myfile->file), 0);
			
			//读取数据头
			myfile->res = f_read(&(myfile->file), &(deviceAdjustReadPackge->deviceRecordHeader), DeviceRecordHeaderStructSize, &(myfile->br));
			if(deviceAdjustReadPackge->deviceRecordHeader.crc != CalModbusCRC16Fun(&(deviceAdjustReadPackge->deviceRecordHeader), DeviceRecordHeaderStructCrcSize, NULL))
				goto Finally;
			
			//如果pageRequest的crc错误，表示是按照上传索引读取数据进行上传
			if(deviceAdjustReadPackge->pageRequest.crc != CalModbusCRC16Fun(&(deviceAdjustReadPackge->pageRequest), PageRequestStructCrcSize, NULL))
			{
				if(deviceAdjustReadPackge->deviceRecordHeader.userUpLoadIndex < deviceAdjustReadPackge->deviceRecordHeader.itemSize)
				{
					f_lseek(&(myfile->file), deviceAdjustReadPackge->deviceRecordHeader.userUpLoadIndex * DeviceAdjustStructSize + DeviceRecordHeaderStructSize);
				
					if((deviceAdjustReadPackge->deviceRecordHeader.itemSize - deviceAdjustReadPackge->deviceRecordHeader.userUpLoadIndex) >= DeviceAdjustRecordPageShowNum)
						f_read(&(myfile->file), deviceAdjustReadPackge->deviceAdjust, DeviceAdjustRecordPageShowNum * DeviceAdjustStructSize, &(myfile->br));
					else
						f_read(&(myfile->file), deviceAdjustReadPackge->deviceAdjust, (deviceAdjustReadPackge->deviceRecordHeader.itemSize - deviceAdjustReadPackge->deviceRecordHeader.userUpLoadIndex) * DeviceAdjustStructSize, &(myfile->br));
				}
			}
			//如果pageRequest的crc正确，表示是按照pageRequest的请求内容进行读取数据
			else if(deviceAdjustReadPackge->pageRequest.startElementIndex < deviceAdjustReadPackge->deviceRecordHeader.itemSize)
			{
				if(deviceAdjustReadPackge->pageRequest.pageSize > (deviceAdjustReadPackge->deviceRecordHeader.itemSize - deviceAdjustReadPackge->pageRequest.startElementIndex))
					deviceAdjustReadPackge->pageRequest.pageSize = (deviceAdjustReadPackge->deviceRecordHeader.itemSize - deviceAdjustReadPackge->pageRequest.startElementIndex);
					
				if(deviceAdjustReadPackge->pageRequest.orderType == DESC)
					myfile->res = f_lseek(&(myfile->file), (deviceAdjustReadPackge->pageRequest.startElementIndex) * DeviceAdjustStructSize + DeviceRecordHeaderStructSize);
				else
					myfile->res = f_lseek(&(myfile->file), (deviceAdjustReadPackge->deviceRecordHeader.itemSize - (deviceAdjustReadPackge->pageRequest.pageSize + deviceAdjustReadPackge->pageRequest.startElementIndex)) * DeviceAdjustStructSize + DeviceRecordHeaderStructSize);
					
				myfile->res = f_read(&(myfile->file), deviceAdjustReadPackge->deviceAdjust, deviceAdjustReadPackge->pageRequest.pageSize * DeviceAdjustStructSize, &(myfile->br));	
				
				for(i=0; i<DeviceAdjustRecordPageShowNum; i++)
				{
					if(deviceAdjustReadPackge->deviceAdjust[i].crc == CalModbusCRC16Fun(&deviceAdjustReadPackge->deviceAdjust[i], DeviceAdjustStructCrcSize, NULL))
						deviceAdjustReadPackge->readTotalNum++;
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

MyRes plusDeviceAdjustHeaderuserUpLoadIndexToFile(unsigned int index)
{
	FatfsFileInfo_Def * myfile = NULL;
	DeviceRecordHeader * deviceRecordHeader = NULL;
	MyRes statues = My_Fail;
	
	myfile = MyMalloc(MyFileStructSize);
	deviceRecordHeader = MyMalloc(DeviceRecordHeaderStructSize);

	if(myfile && deviceRecordHeader)
	{
		memset(myfile, 0, MyFileStructSize);

		myfile->res = f_open(&(myfile->file), DeviceAdjustFileName, FA_OPEN_ALWAYS | FA_WRITE | FA_READ);
		
		if(FR_OK == myfile->res)
		{
			f_lseek(&(myfile->file), 0);

			myfile->res = f_read(&(myfile->file), deviceRecordHeader, DeviceRecordHeaderStructSize, &(myfile->br));
			if((FR_OK == myfile->res) && (DeviceRecordHeaderStructSize == myfile->br) 
				&& (deviceRecordHeader->crc == CalModbusCRC16Fun(deviceRecordHeader, DeviceRecordHeaderStructCrcSize, NULL)))
			{
				deviceRecordHeader->userUpLoadIndex += index;
				deviceRecordHeader->crc = CalModbusCRC16Fun(deviceRecordHeader, DeviceRecordHeaderStructCrcSize, NULL);
				
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

MyRes deleteDeviceAdjustFile(void)
{
	FRESULT res;
	
	res = f_unlink(DeviceAdjustFileName);
	
	if((FR_OK == res) || (FR_NO_FILE == res))
		return My_Pass;
	else
		return My_Fail;
}

/****************************************end of file************************************************/
