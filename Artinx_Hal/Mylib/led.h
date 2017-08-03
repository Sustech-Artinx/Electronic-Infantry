#include "stm32f4xx_hal.h"
#ifndef __LED_H__
#define __LED_H__

#define  LED_GREEN_ON()      HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_SET)
#define  LED_GREEN_OFF()     HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_RESET)
#define  LED_GREEN_TOGGLE()  HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin)

#define  LED_RED_ON()        HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_SET)
#define  LED_RED_OFF()       HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_RESET)
#define  LED_RED_TOGGLE()    HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin)

#endif
