#include "shoot_control.h"
#include "tim.h"
#include "laser.h"
#include "delay.h"
#include "pid_algorithm.h"
#include "encoder.h"
#include "tim.h"

uint8_t last_s2;//s2的上一次key
uint8_t last_pressr;
int rub_flag;//s2-1位开启状态
int shoot_flag;//s2-2位开启状态
int shoot_speed_new;


//拨弹电机速度环控制函数：ShootMotor_Velocity_Control(float TargetShootSpeed)在哪个timer里调用需要考虑，涉及到调节pid的值
void ShootMotor_Velocity_Control(float TargetShootSpeed)
{
	 int32_t PWM_Output;
	 PWM_Output = (int32_t)Velocity_Control_Shoot((float)(GetQuadEncoderDiff()) ,TargetShootSpeed);
	 ShootMotorSpeedSet(PWM_Output);
}

//rc和key控制摩擦轮BLDC开启
void BLDC_control(uint8_t s2, uint8_t press_r)
{	
	//键盘键位解析
	
	//int key_G = KEY_PRESSED_OFFSET_SHIFT & v; if (key_G!=0) key_G=1;

	if (rub_flag == 1 && ( (s2==1 && last_s2!=s2) || (press_r == 1 && last_pressr!=press_r) ))
  {
    //pwm控制电调2312 close；
			PWM1=1000;
			PWM2=1000;
	  	rub_flag=0;
		  LASER_OFF();
  }
  else if (rub_flag == 0 && ( (s2==1 && last_s2!=s2) || (press_r == 1 && last_pressr!=press_r) ))
  {
    
		//pwm控制电调2312 open；
    		PWM1=RUB_SPEED;
				PWM2=RUB_SPEED;
		    printf("shoot_control_PWM");
    		rub_flag=1;
		    LASER_ON();
	}
	last_s2=s2;
	last_pressr=press_r;
}


//rc和mouse控制拨弹电机开启（开火）
void Fire(uint8_t s2, uint8_t press_l)
{
	shoot_flag=0;
	if ((rub_flag == 1) && ( (s2==2) || (press_l == 1) ))
  {

    shoot_flag=1;
		
	}
  if (shoot_flag == 1)
	{ 
		shoot_speed_new=-1000;
		
	}
	else if(shoot_flag == 0)
	{ PWM3=0;
	shoot_speed_new=0;
	}
	delay_us(10);
	last_s2=s2;
	
}
