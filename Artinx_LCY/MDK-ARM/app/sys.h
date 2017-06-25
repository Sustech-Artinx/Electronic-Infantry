/********************************************************************************************************\
*                                     DJI System Global Macros Definations
*
*                                   (c) Copyright 2015; Dji, Inc.
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
\********************************************************************************************************/

#ifndef __SYS
#define __SYS

#include "can.h"
#include "mytype.h"
#include "usart.h"

#define FIRMWARE_VERSION #MAIN_VERSION##'.'
#define MAIN_VERSION 2
#define SUB_VERSION1 1
#define SUB_VERSION2 0
//#define SUB_VERSION3	4

/*================choose uart6 connect==================*/
#define USE_CV_SHOT_BIG_BUFF //connet to manifold, maybe add auto track armor later
//comment last line will choose to  USE_JUDGE_SYSTEM_UART
/*========================================================*/

//#define SAFE_CHASSIS						//define this will cut the chassis output
#define CHASSIS_CAN hcan1 //which can bus device mount
#define ZGYRO_CAN hcan2 //which can bus device mount
#define GIMBAL_CAN hcan2 //which can bus device mount

#define DBUS_HUART huart1 //for dji remote controler reciever use
#define BT_HUART huart2 //for debug use

#ifdef USE_CV_SHOT_BIG_BUFF
#define CV_HUART huart6 //connected to manifold
#else
#define JUDGE_HUART huart6 //connected to judge system
#endif

/*================ CHASSIS MOVE SPEED RATIO ==================*/
#define CHASSIS_RC_MOVE_RATIO_X 0.3f //biger = move faster, 1.0f = 100%
#define CHASSIS_RC_MOVE_RATIO_Y 0.3f //biger = move faster, 1.0f = 100%
#define CHASSIS_PC_MOVE_RATIO_X 1.0f //biger = move faster, 1.0f = 100%
#define CHASSIS_PC_MOVE_RATIO_Y 1.0f //biger = move faster, 1.0f = 100%

//!!!#define CHASSIS_RC_ROTATE_RATIO			1.0f		//rotate rate is define by gimbal yaw move speed
//!!!#define CHASSIS_MOUSE_ROTATE_RATIO		1.0f		//rotate rate is define by gimbal yaw move speed
/*================ CHASSIS MOVE SPEED RATIO ==================*/

/*================ GIMBAL MOVE SPEED RATIO ==================*/
#define GIMBAL_RC_MOVE_RATIO_PIT 0.5f //biger = move faster, 1.0f = 100%
#define GIMBAL_RC_MOVE_RATIO_YAW 1.0f //biger = move faster, 1.0f = 100%

#define GIMBAL_PC_MOVE_RATIO_PIT 1.0f //biger = move faster, 1.0f = 100%
#define GIMBAL_PC_MOVE_RATIO_YAW 1.0f //biger = move faster, 1.0f = 100%
/*================ GIMBAL MOVE SPEED RATIO ==================*/

/*================ GIMBAL MOVE POSITION LIMIT ==================*/
#define YAW_MOVE_RANGE 3000 //from center(aka.0)， also = [center-RANGE, center+RANGE], total = 8192
#define PIT_MOVE_RANGE 300 //from center(aka.0)， also = [center-RANGE, center+RANGE]
/*================ GIMBAL MOVE POSITION LIMIT ==================*/

/*================ GIMBAL SHOT PART PARAM ==================*/
#define POKE_MOTOR_SPEED 2000 //biger = faster
#define SHOT_FRIC_WHEEL_SPEED 1400 //biger = faster, max = 2000
/*================ GIMBAL SHOT PART PARAM ==================*/

/*================ Calibrate Variable Part ==================*/
/**global flash save var ==  [gAppParam]. conteins Gimbal + IMU +　Camera Offsets	
	@Cali how to cali 'GIMBAL'?	in debug mode. move gimbal pit+yaw to center.  
				set gAppParam.Gimbal.NeedCali = 1.  done! so easy!
	@Cali how to cali 'Camera'?	in debug mode. move gimbal pit+yaw to big buff center(aka num 5).  
				set gAppParam.Camera.NeedCali = 1.  done! so easy!
	@warning Remember : !!!Do NOT need to cali IMU
	
	@chinese 	UTF-8 encoding
					如何校准云台。使用jlink debug模式， 将云台扶正到中间。 输入观察变量[gAppParam]， 
					将gAppParam.Gimbal.NeedCali 设为1 按回车  done! so easy!
					
					如何校准相机。使用jlink debug模式，激光对准大符中间5，输入观察变量[gAppParam]， 
					将gAppParam.Camera.NeedCali 设为1 按回车  done! so easy!
					@remember 记得打开妙算
*/
/*================ Calibrate Variable Part ==================*/

/*================ AutoShot Big Buff Part ==================*/
/**	@bref. how to shot big buff.
		1.move to big buff platform, also try your best to move laser to big buff center.
		2.move rc.sw1 = up. 
		3.keep pressing key C, when laser move to 5 center, press Q to record this position
		4.keep pressing key V, when laser move to 9 center, press Q to record this position
		5.if fail to move to 5/9, press R and back to 3/4 retry
		6.when big buff refresh, keep press X to follow big buff shot little human.																
		@chinese 
		1.将车移动到正确的位置，面朝大符，尽量使激光对准中心（第5个格子）
		2.遥控器右边拨到上
		3.按住C直到激光移动到5中心， 按下Q记录这个位置。假如对准失败，按一次R再重试C
		4.按住V直到激光移动到5中心， 按下Q记录这个位置。假如对准失败，按一次R再重试V
		5.大符刚刷新瞬间按住X， enjoy。 !记得大符刚刷新的时候才按下X			*/
/*================ AutoShot Big Buff Part ==================*/

/*================ Error Detect Part ==================*/
/**
	when error occurs Red LED will flash. otherwise Green LED lights.
	in debug mode, see variable [gRxErr] gRxErr.str = "xxxx" 
											eg: gyro lose => gRxErr.str"gyro"
	*/
/*================ Error Detect Part ==================*/
#endif
