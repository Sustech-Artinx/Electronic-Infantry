/**
  ******************************************************************************
  * File Name          : main.h
  * Description        : This file contains the common defines of the application
  ******************************************************************************
  *
  * COPYRIGHT(c) 2017 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H
  /* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define IST_INT_Pin GPIO_PIN_3
#define IST_INT_GPIO_Port GPIOE
#define IST_RST_Pin GPIO_PIN_2
#define IST_RST_GPIO_Port GPIOE
#define LASER_Pin GPIO_PIN_13
#define LASER_GPIO_Port GPIOG
#define KEY_Pin GPIO_PIN_10
#define KEY_GPIO_Port GPIOD
#define LED_RED_Pin GPIO_PIN_14
#define LED_RED_GPIO_Port GPIOF
#define LED_GREEN_Pin GPIO_PIN_7
#define LED_GREEN_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */
//常用函数 取最大值和绝对值
#define abs(x) ((x)>0? (x):(-(x)))
#define maxs(a,b) (a>b? a:b)
//定义车号,主要对应电机和云台
#define CAR_A
//定义自动车
#define AUTO_TRACK
//CAR1 RM35-ROBOMODULE_NEW
//CAR2 RM35-ROBOMODULE_NEW
//CAR3 RM3510

//定义工作与调试模式
//#define DEBUG



//根据车号定义
#ifdef CAR1
//定义底盘电机和驱动板
#define ROBOMODULE_NEW

//云台角度
#define YAW_LEFT 2900
#define YAW_RIGHT	5400
#define YAW_MID 4100
#define PITCH_UP 2600
#define PITCH_DOWN 1800
#define PITCH_MID 2100

//云台PID
#define YAW_205_VEL_P 20.0
#define YAW_205_VEL_D 0.0
#define YAW_205_POS_P 3.5
#define YAW_205_POS_I 0.0
#define YAW_205_POS_D 0.1
#define PITCH_206_VEL_P 15.0
#define PITCH_206_VEL_D 5.0
#define PITCH_206_POS_P 2.0
#define PITCH_206_POS_I 0.0
#define PITCH_206_POS_D 0.0
//定义拨弹电机速度
#define SHOOT_SPEED  10   //0-10000
#endif

#ifdef CAR2 
//定义底盘电机和驱动板
#define ROBOMODULE_NEW

//云台角度
#define YAW_LEFT 1450
#define YAW_RIGHT	5550
#define YAW_MID 3400
#define PITCH_UP 3250
#define PITCH_DOWN 2250
#define PITCH_MID 2500
//云台PID
#define YAW_205_VEL_P 20.0
#define YAW_205_VEL_D 0.0
#define YAW_205_POS_P 2.0
#define YAW_205_POS_I 0.0
#define YAW_205_POS_D 0.1
#define PITCH_206_VEL_P 20.0
#define PITCH_206_VEL_D 0.0
#define PITCH_206_POS_P 1.0
#define PITCH_206_POS_I 0.0
#define PITCH_206_POS_D 0.0

#endif

#ifdef CAR3 
//定义底盘电机
#define RM3510 

//云台角度
#define YAW_LEFT 1450
#define YAW_RIGHT	5550
#define YAW_MID 3400
#define PITCH_UP 3250
#define PITCH_DOWN 2050
#define PITCH_MID 2500
//云台PID
#define YAW_205_VEL_P 20.0
#define YAW_205_VEL_D 0.0
#define YAW_205_POS_P 2.0
#define YAW_205_POS_I 0.0
#define YAW_205_POS_D 0.1
#define PITCH_206_VEL_P 20.0
#define PITCH_206_VEL_D 0.0
#define PITCH_206_POS_P 1.0
#define PITCH_206_POS_I 0.0
#define PITCH_206_POS_D 1.0
//定义拨弹电机速度
#define SHOOT_SPEED  10   //0-10000
#endif

#ifdef CAR_HERO   //英雄车
	//定义底盘电机
	#define RM3510 

	//云台角度
	#define YAW_LEFT -850
	#define YAW_RIGHT	3150
	#define YAW_MID 1095
	#define PITCH_UP 2400
	#define PITCH_DOWN 1320
	#define PITCH_MID 1550
	//云台PID
	#define YAW_205_VEL_P 20.0
	#define YAW_205_VEL_D 0.0
	#define YAW_205_POS_P 2.0
	#define YAW_205_POS_I 0.0
	#define YAW_205_POS_D 0.1
	#define PITCH_206_VEL_P 20.0
	#define PITCH_206_VEL_D 0.0
	#define PITCH_206_POS_P 1.0
	#define PITCH_206_POS_I 0.0
	#define PITCH_206_POS_D 1.0
	
	//定义RM3510的pid
	#define ESC_820R_VEL_P 10.0//10.0
	#define ESC_820R_VEL_I 0.0
	#define ESC_820R_VEL_D 0.5//0.5
	#define ESC_820R_POS_P 10.0//10.0
	#define ESC_820R_POS_I 0.0
	#define ESC_820R_POS_D 0.0


	#define SHOOT_SPEED  10   //0-10000
#endif

#ifdef CAR_A
	//定义底盘电机
	#define RM3510 

	//云台角度
	#define YAW_LEFT 750
	#define YAW_RIGHT	4880
	#define YAW_MID 2800
	#define PITCH_UP 775
	#define PITCH_DOWN -300
	#define PITCH_MID 150
	//云台PID
	#define YAW_205_VEL_P 20.0
	#define YAW_205_VEL_D 0.0
	#define YAW_205_POS_P 2.0
	#define YAW_205_POS_I 0.0
	#define YAW_205_POS_D 0.1
	#define PITCH_206_VEL_P 20.0
	#define PITCH_206_VEL_D 0.0
	#define PITCH_206_POS_P 1.0
	#define PITCH_206_POS_I 0.0
	#define PITCH_206_POS_D 1.0
	
	//定义RM3510的pid
	#define ESC_820R_VEL_P 10.0//10.0
	#define ESC_820R_VEL_I 0.0
	#define ESC_820R_VEL_D 0.5//0.5
	#define ESC_820R_POS_P 10.0//10.0
	#define ESC_820R_POS_I 0.0
	#define ESC_820R_POS_D 0.0

	#define SHOOT_SPEED  13   //0-10000
#endif

#ifdef CAR_B
	//定义底盘电机
	#define RM3510 

	//云台角度
	#define YAW_LEFT 1480
	#define YAW_RIGHT	5630
	#define YAW_MID 3400
	#define PITCH_UP 5200
	#define PITCH_DOWN 3920
	#define PITCH_MID 4350
	//云台PID
	#define YAW_205_VEL_P 20.0
	#define YAW_205_VEL_D 0.0
	#define YAW_205_POS_P 2.0
	#define YAW_205_POS_I 0.0
	#define YAW_205_POS_D 0.1
	#define PITCH_206_VEL_P 18.0
	#define PITCH_206_VEL_D 0.0
	#define PITCH_206_POS_P 1.0
	#define PITCH_206_POS_I 0.0
	#define PITCH_206_POS_D 1.0
	
	//定义RM3510的pid
	#define ESC_820R_VEL_P 10.0//10.0
	#define ESC_820R_VEL_I 0.0
	#define ESC_820R_VEL_D 0.5//0.5
	#define ESC_820R_POS_P 10.0//10.0
	#define ESC_820R_POS_I 0.0
	#define ESC_820R_POS_D 0.0

	#define SHOOT_SPEED  10   //0-10000
#endif

#ifdef CAR_C
	//定义底盘电机
	#define RM3510 

	//云台角度
	#define YAW_LEFT 1200
	#define YAW_RIGHT	5840
	#define YAW_MID 3550
	#define PITCH_UP 4980
	#define PITCH_DOWN 3820
	#define PITCH_MID 4320
	//云台PID
	#define YAW_205_VEL_P 20.0
	#define YAW_205_VEL_D 0.0
	#define YAW_205_POS_P 2.0
	#define YAW_205_POS_I 0.0
	#define YAW_205_POS_D 0.1
	#define PITCH_206_VEL_P 20.0
	#define PITCH_206_VEL_D 0.0
	#define PITCH_206_POS_P 1.0
	#define PITCH_206_POS_I 0.0
	#define PITCH_206_POS_D 1.0
	
	//定义RM3510的pid
#define ESC_820R_VEL_P 10.0//10.0
#define ESC_820R_VEL_I 0.0
#define ESC_820R_VEL_D 0.5//0.5
#define ESC_820R_POS_P 10.0//10.0
#define ESC_820R_POS_I 0.0
#define ESC_820R_POS_D 0.0

	#define SHOOT_SPEED  5   //0-10000
#endif

//底盘运动速度定义
 #define NormalSpeed 180
 #define HighSpeed 230
 #define LowSpeed 50

//仅仅定义驱动板就可以知道用RM35电机了
#ifdef ROBOMODULE_NEW
#define RM35
#endif

#ifdef ROBOMODULE_OLD
#define RM35
#endif


//之后再整合吧
//定义摩擦轮速度
#define RUB_SPEED    1400  //1000-2200

#define PWM1  TIM12->CCR1
#define PWM2  TIM12->CCR2
#define PWM3  TIM2->CCR1

/* USER CODE END Private defines */

/**
  * @}
  */ 

/**
  * @}
*/ 

#endif /* __MAIN_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
