/************************************************************************************
  File Name     :  gimbal_control.c 
  cpu           :  STM32F405RGT6
  Create Date   :  2016/6/29
  Author        :  yf
  Description   :  针对6623云台pithc和yaw轴运动的自下而上的控制。
									 其中首先是两轴云台（2DOF）的PID位置环的控制，
									 然后是云台远程控制函数（遥控器和键鼠），键鼠优先级更高一些。
									 

-------------------------------Revision Histroy-----------------------------------
No   Version    Date     Revised By       Item       Description   
1     1.1       6/28       yf   			  两层云台控制	
2     1.2       6/29       gyf 
3     1.3       6/29       yf 					  注释			   
************************************************************************************/
//#include "i2c_mpu6050.h"
#include "gimbal_control.h"
#include "pid_algorithm.h"
#include "test_imu.h"
#include "delay.h"
#include "i2c_mpu6050.h"
#include "test_can2.h"

//初始化云台角度
M6623 yaw = {YAW_LEFT,YAW_RIGHT,YAW_MID,0,0,0,0,YAW_MID,0,0};
M6623 pitch = {PITCH_DOWN,PITCH_UP,PITCH_MID,0,0,0,0,PITCH_MID,0,0};
double a=0;
double b=0;

/*********************************************************************
Name：          void Gimbal_Control(void)  

Description：  云台控制程序
               向上运动电流为正值
*********************************************************************/
void Gimbal_Control(void)  
{
	//外环PID控制
	//计算位置闭环输出量
	yaw.position_output = Position_Control_205(yaw.thisAngle,yaw.targetAngle);
	//内环PID控制
  //Yaw.velocity_output = Velocity_Control_205(-MPU6050_Real_Data.Gyro_Z ,Yaw.position_output);
  yaw.velocity_output = Velocity_Control_205(-imu_data.gz ,yaw.position_output);
	//计算位置闭环输出量
	pitch.position_output = Position_Control_206(pitch.thisAngle,pitch.targetAngle);
	//内环PID控制
  //Pitch.velocity_output = Velocity_Control_206(-MPU6050_Real_Data.Gyro_Y ,Pitch.position_output);
	//pitch.velocity_output = Velocity_Control_206(0 ,pitch.position_output);
	pitch.velocity_output = Velocity_Control_206(-imu_data.gy ,pitch.position_output);
	//can1的数据输出，在写出can1和can2后需要加上
	Cmd_ESC(yaw.velocity_output,pitch.velocity_output);
  //Cmd_ESC(0,pitch.velocity_output);
	//Cmd_ESC(0,0);
	//Cmd_ESC(yaw.velocity_output,0);
	//printf("%d--%d\n",pitch.thisAngle,yaw.thisAngle);
	//printf("%d\n",imu_data.gx);
}

/*********************************************************************
Name：         void Trigger_Control(int16_t x, int16_t y, uint16_t ch3)

Description：  云台远程控制程序（遥控器和键盘）            
*********************************************************************/

void Trigger_Control(int16_t x, int16_t y, uint16_t ch3)
{
				//暂时不用yaw轴
				if (yaw.targetAngle < yaw.minAngle){yaw.targetAngle=yaw.minAngle;}
				if (yaw.targetAngle > yaw.maxAngle){yaw.targetAngle=yaw.maxAngle;}				
				if (y>3) {pitch.targetAngle += -3;}
        if (y<-3) {pitch.targetAngle += 3;}
				pitch.targetAngle += (float)((float)(ch3-1024)/100);
				delay_us(500);
				if (pitch.targetAngle < pitch.minAngle){pitch.targetAngle=pitch.minAngle;}
				if (pitch.targetAngle > pitch.maxAngle){pitch.targetAngle=pitch.maxAngle;}		
			
}
