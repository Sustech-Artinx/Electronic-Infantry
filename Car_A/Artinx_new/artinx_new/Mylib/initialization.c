#include "initialization.h"
#include "delay.h"
#include "tim.h"
#include "test_imu.h"
void  Initialization(void){
	delay_ms(10);
	HAL_TIM_Base_Start_IT(&htim3);
  if(HAL_TIM_Base_Start_IT(&htim3)!=HAL_OK){
	printf("timer3 default\n");
	HAL_TIM_Base_Start_IT(&htim3);
	}
	printf("timer3 ok\n");
	delay_ms(10);
}
