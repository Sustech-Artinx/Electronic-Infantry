#include "initialization.h"
#include "delay.h"
#include "tim.h"
#include "test_imu.h"
void  Initialization(void){
	MPU6500_Init();
	delay_ms(1);
  HAL_TIM_Base_Start_IT(&htim3);
	delay_ms(1);
}
