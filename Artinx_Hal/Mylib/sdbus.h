#include "stm32f4xx_hal.h"
#include "stdint.h"
#ifndef _SDBUS_H_
#define _SDBUS_H_

typedef struct{
int PitchAngle;
int YawAngle;	
}SDBUS;

extern SDBUS sdbus;
extern int SDFlag;
extern int LastQKey;
extern int LastEKey;
extern int BlueFlag;
void SDState_Set(uint16_t v);
void SD_TriggerControl(void);
void SDBUS_Enc(const SDBUS* sdbus,unsigned char* sdbuf);
void SDBUS_Dec(SDBUS* sdbus,const unsigned char* sdbuf);
#endif
