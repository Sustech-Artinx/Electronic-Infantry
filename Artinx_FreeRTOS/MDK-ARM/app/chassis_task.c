 /* Includes ------------------------------------------------------------------*/
#include "chassis_task.h"
#include "bsp_can.h"
#include "bsp_uart.h"
#include "can.h"
#include "cmsis_os.h"
#include "error_task.h"
#include "gimbal_task.h"
#include "judge_sys.h"
#include "kb.h"
#include "pid.h"
#include "stm32f4xx_hal.h"
#include "sys.h"
#include <math.h>
#define MyAbs(x) ((x > 0) ? (x) : (-x))
#define MAX_WHEEL_SPEED 1000
#define MAX_CHASSIS_VX_SPEED 660
#define MAX_CHASSIS_VY_SPEED 660
#define MAX_CHASSIS_OMEGA_SPEED 300
//#define CHASSIS_OLD //diff chassis type
//#define CHASSIS_VERY_OLD

extern int last_sw1;

chassis_t chassis; // main chassis object

s16 buff_3510iq[4];

// actually this belong to gimbal will better
void reset_zgyro()
{
    while (ZGYRO_CAN.State == HAL_CAN_STATE_BUSY_TX)
        ;
    ZGYRO_CAN.pTxMsg->StdId   = 0x404;
    ZGYRO_CAN.pTxMsg->IDE     = CAN_ID_STD;
    ZGYRO_CAN.pTxMsg->RTR     = CAN_RTR_DATA;
    ZGYRO_CAN.pTxMsg->DLC     = 0x08;
    ZGYRO_CAN.pTxMsg->Data[0] = 0;
    ZGYRO_CAN.pTxMsg->Data[1] = 1;
    ZGYRO_CAN.pTxMsg->Data[2] = 2;
    ZGYRO_CAN.pTxMsg->Data[3] = 3;
    ZGYRO_CAN.pTxMsg->Data[4] = 4;
    ZGYRO_CAN.pTxMsg->Data[5] = 5;
    ZGYRO_CAN.pTxMsg->Data[6] = 6;
    ZGYRO_CAN.pTxMsg->Data[7] = 7;
    HAL_CAN_Transmit(&ZGYRO_CAN, 1000);
}

/**
* @function mecanum calculation function.
* @input vx, vy omega
* @output 4 wheel speed.
* @bref omega: + = cw, - = ccw
* @output wheel_speed[4];
* @note : 1:FR, 2=FL 3=BL, 4=BR, ↑↓=Vy， ←→=Vx
* @map TYPE=1
        2	%++++++%	1
              ++++
              ++++
        3	%++++++%	4    ↑=+Vy  →=+Vx

//TYPE=BASE
//        X FRONT X
//      X           X
//    X  P2       P1  X
//            X
//           XXX
//            X
//    X  P3       P4  X
//      X           X
//        X       X					↑=+Vx  →=+Vy
					


*/
void mecanum_calc(float vx, float vy, float omega, const int each_max_spd,
                  s16 speed[])
{
    s16   buf[4];
    int   i;
    float max = 0, rate;

    vx    = vx > MAX_CHASSIS_VX_SPEED ? MAX_CHASSIS_VX_SPEED : vx;
    vx    = vx < -MAX_CHASSIS_VX_SPEED ? -MAX_CHASSIS_VX_SPEED : vx;
    vy    = vy > MAX_CHASSIS_VY_SPEED ? MAX_CHASSIS_VY_SPEED : vy;
    vy    = vy < -MAX_CHASSIS_VY_SPEED ? -MAX_CHASSIS_VY_SPEED : vy;
    omega = omega > MAX_CHASSIS_OMEGA_SPEED ? MAX_CHASSIS_OMEGA_SPEED : omega;
    omega = omega < -MAX_CHASSIS_OMEGA_SPEED ? -MAX_CHASSIS_OMEGA_SPEED : omega;
	

#ifdef BASE_CHASSIS
    buf[0] = -(-vx + vy - omega);
    buf[1] = -(vx + vy - omega);
    buf[2] = (-vx + vy + omega);
    buf[3] = (vx + vy + omega);	
	
#else //you should test on your own.
    buf[0] = (-vx - vy + omega);
    buf[1] = vx - vy + omega;
    buf[2] = (-vx + vy + omega);
    buf[3] = vx + vy + omega; //因为轮子方向的关系
#endif
    // find max item
    for (i = 0; i < 4; i++)
    {
        if (MyAbs(buf[i]) > max)
            max = MyAbs(buf[i]);
    }
    //等比例缩放， 都是对于绝对值。 不允许单个轮子速度超过最大值，否则合成不正常
    if (max > each_max_spd)
    {
        rate = each_max_spd / max;
        for (i = 0; i < 4; i++)
            buf[i] *= rate;
    }
    // output
    memcpy(speed, buf, sizeof(s16) * 4);
}
/**
        * @bref   send 4 calculated current to driver
        * @assume 3510 driver in current mode
        */
void set_cm_current(CAN_HandleTypeDef* hcan, s16 iq1, s16 iq2, s16 iq3, s16 iq4)
{

    hcan->pTxMsg->StdId   = 0x200;
    hcan->pTxMsg->IDE     = CAN_ID_STD;
    hcan->pTxMsg->RTR     = CAN_RTR_DATA;
    hcan->pTxMsg->DLC     = 0x08;
    hcan->pTxMsg->Data[0] = iq1 >> 8;
    hcan->pTxMsg->Data[1] = iq1;
    hcan->pTxMsg->Data[2] = iq2 >> 8;
    hcan->pTxMsg->Data[3] = iq2;
    hcan->pTxMsg->Data[4] = iq3 >> 8;
    hcan->pTxMsg->Data[5] = iq3;
    hcan->pTxMsg->Data[6] = iq4 >> 8;
    hcan->pTxMsg->Data[7] = iq4;
    HAL_CAN_Transmit(hcan, 1000);
}

void get_chassis_mode_set_ref(RC_Type* rc)
{

    chassis.last_mode = chassis.mode;

    switch (rc->sw1)
    {
    case RC_UP:
        chassis.mode = CHASSIS_STOP;
        break;

    case RC_MI:
        chassis.mode = CHASSIS_OPEN_LOOP; // release
        break;

    case RC_DN:
        // chassis.mode = CHASSIS_CLOSE_GYRO_LOOP;
        chassis.mode = CHASSIS_FOLLOW_GIMBAL_ENCODER; // release
        break;
    default:

        break;
    }
#ifdef SAFE_CHASSIS
    chassis.mode = CHASSIS_RELAX;
#endif
#ifdef CHASSIS_VERY_OLD
    chassis.mode = CHASSIS_OPEN_LOOP;
#endif
    switch (chassis.mode)
    {
    case CHASSIS_STOP:
    {
        chassis.vx    = 0;
        chassis.vy    = 0;
        chassis.omega = 0;
    }
    break;
    case CHASSIS_OPEN_LOOP:
    {

        chassis.vy = -rc->ch1 * CHASSIS_RC_MOVE_RATIO_Y + km.vy;
        chassis.vx = -rc->ch2 * CHASSIS_RC_MOVE_RATIO_X + km.vx;
        //				//chassis.omega = 0;
        chassis.omega = rc->ch3 / 2 + rc->mouse.x * 10; // remove this will

#ifdef CHASSIS_VERY_OLD
        chassis.vy = -rc->ch2 * CHASSIS_RC_MOVE_RATIO_Y + km.vy;
        chassis.vx = -rc->ch1 * CHASSIS_RC_MOVE_RATIO_X + km.vx;
        //				//chassis.omega = 0;
        chassis.omega = -rc->ch3 / 2 + rc->mouse.x * 10; // remove this will
// better
#endif
    }
    break;
    case CHASSIS_FOLLOW_GIMBAL_ENCODER:
    {
        chassis.vy = -rc->ch1 * CHASSIS_RC_MOVE_RATIO_Y + km.vy * CHASSIS_PC_MOVE_RATIO_Y; //+ tu_rc.bits.ch2; // last item is or
        chassis.vx = -rc->ch2 * CHASSIS_RC_MOVE_RATIO_X + km.vx * CHASSIS_PC_MOVE_RATIO_X; //+ tu_rc.bits.ch1;

        // omega according to yaw encoder pid calc
        // chassis.target_angle += rc->ch3 * 0.001f;
    }
    break;
    }
}

float scope_param[2];

void chassis_task(void const* argu)
{
    int i = 0;

    for (int k = 0; k < 4; k++)
    {
        /*	max current = 20000, it may cause deadly injury !!! just like me today*/
        PID_struct_init(&pid_spd[k], POSITION_PID, 20000, 20000, 4, 0.05f, 0.0f);
    }
    PID_struct_init(&pid_chassis_angle, POSITION_PID, 300, 300, 0.5f, 0.0f, 1.3f);
    pid_chassis_angle.max_err  = 60 * 22.75f; // err angle > 60 cut the output
    pid_chassis_angle.deadband = 10; // err angle <10 cut the output

    HAL_Delay(1000);
    // TODO : chassis follows gimbal. chassis follow gimbal encoder.DONE
    while (1)
    {
        pc_kb_hook();

        get_chassis_mode_set_ref(&rc);
        /*TODO: refactory this code, combo to omega*/

        /* chassis follows gimbal yaw, just keep encoder = 0, ==> pid.set = 0*/

        /* CHASSIS_CLOSE_GYRO_LOOP actually NOT used yet# */
        if (chassis.mode == CHASSIS_CLOSE_GYRO_LOOP)
        {
            if (chassis.last_mode != CHASSIS_CLOSE_GYRO_LOOP)
            {
                chassis.target_angle = chassis.angle_from_gyro;
            }
            // chassis.omega = -pid_calc(&pid_chassis_angle, chassis.angle_from_gyro,
            // chassis.target_angle);
        }
        /* CHASSIS_CLOSE_GYRO_LOOP actually NOT used yet */

        if (chassis.mode == CHASSIS_FOLLOW_GIMBAL_ENCODER && rc.sw2 != RC_MI)
        { // need to change range(-xxxx -> 0 -> +xxxx)//done
            chassis.omega = pid_calc(&pid_chassis_angle, yaw_relative_pos,
                                     0); // chassis follow yaw, keep relative pos = 0 is good
        }
        else if (chassis.mode == CHASSIS_OPEN_LOOP)
        {
            //...ignore gimbal.
        }
        else
        {
            chassis.omega = 0;
        }
        if (fabs(chassis.vx) < 5)
            chassis.vx = 0; // avoid rc stick have little offset
        if (fabs(chassis.vy) < 5)
            chassis.vy = 0;
        if (chassis.is_snipe_mode)
            chassis.omega = 0; //|| ABS(chassis.omega) < 10
        mecanum_calc(chassis.vx, chassis.vy, chassis.omega, MAX_WHEEL_SPEED,
                     chassis.wheel_speed.s16_fmt);
        for (i = 0; i < 4; i++)
        {
            buff_3510iq[i] = pid_calc(&pid_spd[i], moto_chassis[i].speed_rpm,
                                      chassis.wheel_speed.s16_fmt[i] * 10);
        }

        if (chassis.mode == CHASSIS_RELAX || rc.sw2 != RC_UP || gRxErr.err_list[DbusTOE].err_exist
            //				||
            //gRxErr.err_list[ChassisGyroTOE].err_exist
            //|| gRxErr.err_list[GimbalYawTOE].err_exist
            )
        {
            memset(buff_3510iq, 0, sizeof(buff_3510iq));
            pid_spd[0].iout = 0;
            pid_spd[1].iout = 0;
            pid_spd[2].iout = 0;
            pid_spd[3].iout = 0;
        }

        scope_param[0] = pid_spd[0].set[0];
        scope_param[1] = pid_spd[0].get[0];

        set_cm_current(&CHASSIS_CAN, buff_3510iq[0], buff_3510iq[1], buff_3510iq[2],
                       buff_3510iq[3]);

        osDelay(10);
    }
}
