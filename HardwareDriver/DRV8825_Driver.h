#ifndef DRV_D_H_H
#define DRV_D_H_H

#include	"stm32f4xx.h"
#include	"MotorData.h"


#define	DRV_Power_Pin			GPIO_Pin_5
#define	DRV_Power_Group			GPIOG
#define	DRV_Power_RCC			RCC_AHB1Periph_GPIOG

#define	DRV_Sleep_Pin			GPIO_Pin_2
#define	DRV_Sleep_Group			GPIOG
#define	DRV_Sleep_RCC			RCC_AHB1Periph_GPIOG

#define	DRV_Dir_Pin				GPIO_Pin_3
#define	DRV_Dir_Group			GPIOG
#define	DRV_Dir_RCC				RCC_AHB1Periph_GPIOG



void DRV_Init(void);
void SetDRVSleepStatues(DRVSleep statues);
void SetDRVPowerStatues(DRVPower statues);
void SetDRVDir(DRVDir value);
void motorDisable(void);

#endif


