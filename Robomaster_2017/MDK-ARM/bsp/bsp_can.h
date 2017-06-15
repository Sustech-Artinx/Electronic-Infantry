/**
  ******************************************************************************
  * @file			
  * @author		Ginger
  * @version	V1.0.0
  * @date			2015/11/30
  * @brief   
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
#ifndef __my_can_H
#define __my_can_H
/* Includes ------------------------------------------------------------------*/
#ifdef STM32F4
#include "stm32f4xx_hal.h"
#elif defined STM32F1
#include "stm32f1xx_hal.h"
#endif
#include "can.h"
#include "mytype.h"

/*CAN发送或是接收的ID*/
typedef enum {

    CAN_TxGimbal_ID           = 0x1FF, //云台发送ID
    CAN_YAW_FEEDBACK_ID       = 0x205, //云台Yaw24v
    CAN_PIT_FEEDBACK_ID       = 0x206, //云台Yaw24v
    CAN_POKE_FEEDBACK_ID      = 0x207,
    CAN_ZGYRO_RST_ID          = 0x404,
    CAN_ZGYRO_FEEDBACK_MSG_ID = 0x401,
    CAN_MotorLF_ID            = 0x041, //左前
    CAN_MotorRF_ID            = 0x042, //右前
    CAN_MotorLB_ID            = 0x043, //左后
    CAN_MotorRB_ID            = 0x044, //右后
    CAN_4Moto_Target_Speed_ID = 0x046, //
    CAN_GyroRecev_ID          = 0x011, //陀螺仪接收
    CAN_GyroReset_ID          = 0x012, //陀螺仪复位
    CAN_EC60_All_ID           = 0x200, //EC60接收程序
    CAN_backLeft_EC60_ID      = 0x203, //ec60
    CAN_frontLeft_EC60_ID     = 0x201, //ec60
    CAN_backRight_EC60_ID     = 0x202, //ec60
    CAN_frontRight_EC60_ID    = 0x204, //ec60

    CAN_3510MotoAll_ID = 0x200,
    CAN_3510Moto1_ID   = 0x201,
    CAN_3510Moto2_ID   = 0x202,
    CAN_3510Moto3_ID   = 0x203,
    CAN_3510Moto4_ID   = 0x204,
    CAN_DriverPower_ID = 0x80,

    CAN_HeartBeat_ID = 0x156,

} eCAN_MSG_ID;

#define FILTER_BUF_LEN 5
/*接收到的云台电机的参数结构体*/
typedef struct
{
    int16_t  speed_rpm;
    int16_t  real_current;
    int16_t  given_current;
    uint8_t  hall;
    uint16_t angle; //abs angle range:[0,8191]
    uint16_t last_angle; //abs angle range:[0,8191]
    uint16_t offset_angle;
    int32_t  round_cnt;
    int32_t  total_angle;
    u8       buf_idx;
    u16      angle_buf[FILTER_BUF_LEN];
    u16      fited_angle;
    u32      msg_cnt;
} moto_measure_t;

/* Extern  ------------------------------------------------------------------*/
extern moto_measure_t moto_chassis[];
extern moto_measure_t moto_yaw, moto_pit, moto_poke;
extern float          real_current_from_judgesys; //unit :mA
extern float          dynamic_limit_current; //unit :mA,;	//from judge_sys
extern float          ZGyroModuleAngle, yaw_zgyro_angle;

void my_can_filter_init(CAN_HandleTypeDef* hcan);
void my_can_filter_init_recv_all(CAN_HandleTypeDef* _hcan);
void can_filter_recv_special(CAN_HandleTypeDef* hcan, uint8_t filter_number, uint16_t filtered_id);
void get_moto_measure(moto_measure_t* ptr, CAN_HandleTypeDef* hcan);
void can_receive_onetime(CAN_HandleTypeDef* _hcan, int time);

#endif
