#include "stm32f4xx_hal.h"
#ifndef _LASER_H_
#define _LASER_H_
#define LASER_ON()  HAL_GPIO_WritePin(LASER_GPIO_Port, LASER_Pin, GPIO_PIN_SET)
#define LASER_OFF() HAL_GPIO_WritePin(LASER_GPIO_Port, LASER_Pin, GPIO_PIN_RESET)
#endif
