/***************************************************************************************************
*FileName: DeviceAdjustDao
*Description: 读写设备校准记录
*Author: xsx_kair
*Data: 2017年6月2日 15:53:25
***************************************************************************************************/

/***************************************************************************************************/
/******************************************Header List********************************************/
/***************************************************************************************************/
#include	"DeviceMaintenanceDao.h"

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

MyState_TypeDef writeDeviceMaintenanceToFile(DeviceMaintenance * deviceMaintenance)
{
	FatfsFileInfo_Def * myfile = NULL;
	DeviceRecordHeader * deviceRecordHeader = NULL;
	MyState_TypeDef statues = My_Fail;
	
	myfile = MyMalloc(MyFileStructSize);
	deviceRecordHeader = MyMalloc(DeviceRecordHeaderStructSize);
	
	if(myfile && deviceMaintenance && deviceRecordHeader)
	{
		memset(myfile, 0, MyFileStructSize);
		memset(deviceRecordHeader, 0, DeviceRecordHeaderStructSize);
		
		myfile->res = f_open(&(myfile->file), DeviceMainenanceFileName, FA_OPEN_ALWAYS | FA_WRITE | FA_READ);
			
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
			f_lseek(&(myfile->file), deviceRecordHeader->itemSize * DeviceMaintenanceStructSize + DeviceRecordHeaderStructSize);
			myfile->res = f_write(&(myfile->file), deviceMaintenance, DeviceMaintenanceStructSize, &(myfile->bw));

			if((FR_OK != myfile->res) || (myfile->bw != DeviceMaintenanceStructSize))
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

MyState_TypeDef readDeviceMaintenanceFromFile(DeviceMaintenanceReadPackge * deviceMaintenanceReadPackge)
{
	FatfsFileInfo_Def * myfile = NULL;
	MyState_TypeDef statues = My_Fail;
	unsigned char i=0;
	
	myfile = MyMalloc(MyFileStructSize);

	if(myfile && deviceMaintenanceReadPackge)
	{
		memset(myfile, 0, MyFileStructSize);
		memset(&(deviceMaintenanceReadPackge->deviceRecordHeader), 0, DeviceRecordHeaderStructSize);
		memset(&(deviceMaintenanceReadPackge->deviceMaintenance), 0, DeviceMaintenanceStructSize * DeviceMaintenanceRecordPageShowNum);
		deviceMaintenanceReadPackge->readTotalNum = 0;
		
		myfile->res = f_open(&(myfile->file), DeviceMainenanceFileName, FA_READ);
		
		if(FR_OK == myfile->res)
		{
			myfile->size = f_size(&(myfile->file));
			
			f_lseek(&(myfile->file), 0);
			
			//读取数据头
			myfile->res = f_read(&(myfile->file), &(deviceMaintenanceReadPackge->deviceRecordHeader), DeviceRecordHeaderStructSize, &(myfile->br));
			if(deviceMaintenanceReadPackge->deviceRecordHeader.crc != CalModbusCRC16Fun1(&(deviceMaintenanceReadPackge->deviceRecordHeader), DeviceRecordHeaderStructCrcSize))
				goto Finally;
			
			//如果pageRequest的crc错误，表示是按照上传索引读取数据进行上传
			if(deviceMaintenanceReadPackge->pageRequest.crc != CalModbusCRC16Fun1(&(deviceMaintenanceReadPackge->pageRequest), PageRequestStructCrcSize))
			{
				if(deviceMaintenanceReadPackge->deviceRecordHeader.uploadIndex < deviceMaintenanceReadPackge->deviceRecordHeader.itemSize)
				{
					f_lseek(&(myfile->file), deviceMaintenanceReadPackge->deviceRecordHeader.uploadIndex * DeviceMaintenanceStructSize + DeviceRecordHeaderStructSize);
				
					if((deviceMaintenanceReadPackge->deviceRecordHeader.itemSize - deviceMaintenanceReadPackge->deviceRecordHeader.uploadIndex) >= DeviceMaintenanceRecordPageShowNum)
						f_read(&(myfile->file), deviceMaintenanceReadPackge->deviceMaintenance, DeviceMaintenanceRecordPageShowNum * DeviceMaintenanceStructSize, &(myfile->br));
					else
						f_read(&(myfile->file), deviceMaintenanceReadPackge->deviceMaintenance, (deviceMaintenanceReadPackge->deviceRecordHeader.itemSize - deviceMaintenanceReadPackge->deviceRecordHeader.uploadIndex) * DeviceMaintenanceStructSize, &(myfile->br));
				}
			}
			//如果pageRequest的crc正确，表示是按照pageRequest的请求内容进行读取数据
			else if(deviceMaintenanceReadPackge->pageRequest.startElementIndex < deviceMaintenanceReadPackge->deviceRecordHeader.itemSize)
			{
				if(deviceMaintenanceReadPackge->pageRequest.pageSize > (deviceMaintenanceReadPackge->deviceRecordHeader.itemSize - deviceMaintenanceReadPackge->pageRequest.startElementIndex))
					deviceMaintenanceReadPackge->pageRequest.pageSize = (deviceMaintenanceReadPackge->deviceRecordHeader.itemSize - deviceMaintenanceReadPackge->pageRequest.startElementIndex);
					
				if(deviceMaintenanceReadPackge->pageRequest.orderType == DESC)
					myfile->res = f_lseek(&(myfile->file), (deviceMaintenanceReadPackge->pageRequest.startElementIndex) * DeviceMaintenanceStructSize + DeviceRecordHeaderStructSize);
				else
					myfile->res = f_lseek(&(myfile->file), (deviceMaintenanceReadPackge->deviceRecordHeader.itemSize - (deviceMaintenanceReadPackge->pageRequest.pageSize + deviceMaintenanceReadPackge->pageRequest.startElementIndex)) * DeviceMaintenanceStructSize + DeviceRecordHeaderStructSize);
					
				myfile->res = f_read(&(myfile->file), deviceMaintenanceReadPackge->deviceMaintenance, deviceMaintenanceReadPackge->pageRequest.pageSize * DeviceMaintenanceStructSize, &(myfile->br));	
				
				for(i=0; i<DeviceMaintenanceRecordPageShowNum; i++)
				{
					if(deviceMaintenanceReadPackge->deviceMaintenance[i].crc == CalModbusCRC16Fun1(&deviceMaintenanceReadPackge->deviceMaintenance[i], DeviceMaintenanceStructCrcSize))
						deviceMaintenanceReadPackge->readTotalNum++;
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

MyState_TypeDef plusDeviceMaintenanceHeaderUpLoadIndexToFile(unsigned int index)
{
	FatfsFileInfo_Def * myfile = NULL;
	DeviceRecordHeader * deviceRecordHeader = NULL;
	MyState_TypeDef statues = My_Fail;
	
	myfile = MyMalloc(MyFileStructSize);
	deviceRecordHeader = MyMalloc(DeviceRecordHeaderStructSize);

	if(myfile && deviceRecordHeader)
	{
		memset(myfile, 0, MyFileStructSize);

		myfile->res = f_open(&(myfile->file), DeviceMainenanceFileName, FA_OPEN_ALWAYS | FA_WRITE | FA_READ);
		
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

MyState_TypeDef deleteDeviceMaintenanceFile(void)
{
	FRESULT res;
	
	res = f_unlink(DeviceMainenanceFileName);
	
	if((FR_OK == res) || (FR_NO_FILE == res))
		return My_Pass;
	else
		return My_Fail;
}

/****************************************end of file************************************************/
