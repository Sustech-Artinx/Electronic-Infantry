/************************************************************************************
  File Name     :  gimbal_control.c
  cpu           :  STM32F405RGT6
  Create Date   :  2016/6/29
  Author        :  yf
  Description   :  ����6623��̨pithc��yaw���˶������¶��ϵĿ��ơ�
									 ����������������̨��2DOF����PIDλ�û��Ŀ��ƣ�
									 Ȼ������̨Զ�̿��ƺ�����ң�����ͼ��󣩣��������ȼ�����һЩ��


-------------------------------Revision Histroy-----------------------------------
No   Version    Date     Revised By       Item       Description
1     1.1       6/28       yf   			  ������̨����
2     1.2       6/29       gyf
3     1.3       6/29       yf 					  ע��
************************************************************************************/
//#include "i2c_mpu6050.h"
#include "gimbal_control.h"
#include "pid_algorithm.h"
#include "test_imu.h"
#include "delay.h"
#include "i2c_mpu6050.h"
#include "test_can2.h"

//��ʼ����̨�Ƕ�
M6623 yaw = {YAW_LEFT,YAW_RIGHT,YAW_MID,0,0,0,0,YAW_MID,0,0};
M6623 pitch = {PITCH_DOWN,PITCH_UP,PITCH_MID,0,0,0,0,PITCH_MID,0,0};
double a=0;
double b=0;

/*********************************************************************
Name��          void Gimbal_Control(void)

Description��  ��̨���Ƴ���
               �����˶�����Ϊ��ֵ
*********************************************************************/
void Gimbal_Control(void)
{
	//�⻷PID����
	//����λ�ñջ�������
	yaw.position_output = Position_Control_205(yaw.thisAngle,yaw.targetAngle);
	//�ڻ�PID����
  //Yaw.velocity_output = Velocity_Control_205(-MPU6050_Real_Data.Gyro_Z ,Yaw.position_output);
  yaw.velocity_output = Velocity_Control_205(imu_data.gz ,yaw.position_output);
	//����λ�ñջ�������
	pitch.position_output = Position_Control_206(pitch.thisAngle,pitch.targetAngle);
	//�ڻ�PID����
  //Pitch.velocity_output = Velocity_Control_206(-MPU6050_Real_Data.Gyro_Y ,Pitch.position_output);
	//pitch.velocity_output = Velocity_Control_206(0 ,pitch.position_output);
	pitch.velocity_output = Velocity_Control_206(-imu_data.gy ,pitch.position_output); //因为主控板位置调转
	//can1��������������д��can1��can2����Ҫ����
	Cmd_ESC(yaw.velocity_output,pitch.velocity_output);
  //Cmd_ESC(0,pitch.velocity_output);
	//Cmd_ESC(0,0);
	//Cmd_ESC(yaw.velocity_output,0);
	printf("%d--%d\n",pitch.thisAngle,yaw.thisAngle);
	//printf("%d\n",imu_data.gx);
}

/*********************************************************************
Name��         void Trigger_Control(int16_t x, int16_t y, uint16_t ch3)
Description��  ��̨Զ�̿��Ƴ�����ң�����ͼ��̣�
*********************************************************************/

void Trigger_Control(int16_t x, int16_t y, uint16_t ch3)
{
				//��ʱ����yaw��
				if (yaw.targetAngle < yaw.minAngle){yaw.targetAngle=yaw.minAngle;}
				if (yaw.targetAngle > yaw.maxAngle){yaw.targetAngle=yaw.maxAngle;}
				if (y>3) {pitch.targetAngle += -3;}
        if (y<-3) {pitch.targetAngle += 3;}
				pitch.targetAngle += (float)((float)(ch3-1024)/100);
				delay_us(500);
				if (pitch.targetAngle < pitch.minAngle){pitch.targetAngle=pitch.minAngle;}
				if (pitch.targetAngle > pitch.maxAngle){pitch.targetAngle=pitch.maxAngle;}

}
