#include "stm32f4xx_hal.h"
#include "stdint.h"
#ifndef __Encoder_H__
#define __Encoder_H__

//void Quad_Encoder_Configuration(void);
void Encoder_Start(void);
int32_t GetQuadEncoderDiff(void);

#endif
