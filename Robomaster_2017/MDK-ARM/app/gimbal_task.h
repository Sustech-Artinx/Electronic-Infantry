
#pragma once
#include "mytype.h"
void gimbal_task(const void* argu);

typedef enum {

    GIMBAL_INIT = 0,
    GIMBAL_RELAX,
    GIMBAL_CLOSE_LOOP_ENCODER,
    GIMBAL_CLOSE_LOOP_ZGYRO,

} eGimbalMode;

typedef struct
{
    eGimbalMode ctrl_mode; //yaw 闭环模式
    eGimbalMode last_mode; //
    //	int is_standby_mode;	//rc no action
    //	int over_range_err_cnt;	//for safety purpose
    //	int gimbal_init_done;
    //	int is_zgyro_ready;
    //	float zgyro_target;
    //	float zgyro_rc;
    //	float zgyro_mouse;
    //	float zgyro_angle;
    float zgyro_offset;
    //	float final_position_get;
    //	float final_position_set;

} gimbal_yaw_t;

extern gimbal_yaw_t gYaw;
extern int          yaw_center_offset;
extern s16          yaw_relative_pos;
extern u8           is_in_big_buff_mode;
