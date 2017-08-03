

/********************************************************************************************************\
*                                     DJI FreeRTOS TASK PART 
*
*                                   (c) Copyright 2015; Dji, Inc.
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
\********************************************************************************************************/
/**
  ******************************************************************************
  * @file    gimbal_task.c
  * @author  langgo
  * @version V1.0
  * @date    2016-11-30
  * @brief   
	* @update		
		@update 2   After this version, gimbal's encoder feedback position range [-xxxx , +xxxx], symmetric by 0，
								after call @function s16 get_relative_pos(s16 raw_ecd, s16 center_offset) will
								get a position relative to the center is center_encoder/offset= GimbalCaliData.GimbalPit/YawOffset;
								all input refence range also = [-xxxx, xxxx], Symmetric by 0
		@update 3   when rc sw2 = RC_DN, stop everything
	*	@attention			
  * @verbatim you should comment with english avoid diff text encoder cause ?!$@#%^@$%&$*
	****************************************************************************
	*/

#include "gimbal_task.h"
#include "bsp_can.h"
#include "bsp_uart.h"
#include "calibrate.h"
#include "chassis_task.h"
#include "cmsis_os.h"
#include "error_task.h"
#include "judge_sys.h"
#include "kb.h"
#include "mpu.h"
#include "mytype.h"
#include "pid.h"
#include "stm32f4xx_hal.h"
#include "sys.h"
#include <math.h>

#define VAL_LIMIT(x, min, max) \
    do                         \
    {                          \
        if ((x) < (min))       \
            (x) = (min);       \
        if ((x) > (max))       \
            (x) = (max);       \
    } while (0)

float yaw_pos_ref       = 0;
float pit_pos_ref       = 0; //rc_integral + mouse_integral
int   pit_center_offset = 0; //read from flash. == ecd value when gimbal pit in center position
int   yaw_center_offset = 0; //read from motor esc when yaw axis in center.		//this will read from flash in the future.

/*gimbal move delta position, init value = 0, range (-MOVE_RANGE, MOVE_RANGE)*/
s16          pit_relative_pos, yaw_relative_pos; //unit : encoder
float        watch_param[4]; //for jscope watch
float        watch_pit[4];
int          last_sw1;
int          fric_wheel_run = 0; //run or not
int          poke_spd_ref, poke_pos_ref; //poke bullet speed or
u8           mouse_left_sta  = 0;
u8           mouse_right_sta = 0;
int          reset_sequence  = 0;
int          poke_stuck_cnt  = 0;
int          poke_dir        = 1;
float        imu_tmp, imu_tmp_ref = 40; //keep imu in 40 degree
gimbal_yaw_t gYaw;
		
/*===============auto shoot big buff part ===============*/

s16 big_buff_table[9][2]; //this table represents 9 points,point format = [yaw,pit]
u8  manifold_cv_cmd; //watch which mode pc is
u8  autoshot_cmd = 0; //used for shot bullet when big buff num changed, such as 1->5
s16 img_center_yaw, img_center_pit; //read from flash.when camera assemble position changed, need cali
u8  is_in_big_buff_mode; //1 = //unsed now
/*===============auto shoot big buff part ===============*/
#if 0 //hide auto shot big buff.

#define CMD_TEST_UART 0xF1
#define CMD_FIND_NUM 0xF3
#define CMD_CALI_5 0XF4
#define CMD_CALI_9 0xF5
#define CMD_DEBUG 0xFF //return num, yaw+pit
/**
	*	@param cmd is upon	5 CMD xxxx
	*/
void set_manifold_vision_mode(u8 cmd){
#ifdef CV_HUART
		manifold_cv_cmd = cmd;
		HAL_UART_Transmit(&CV_HUART, &manifold_cv_cmd, 1, 10);
#endif
}

void shot_big_buff_init(){
	
	set_manifold_vision_mode(CMD_CALI_5);
	if(gAppParam.CameraCali.isAlreadyCalied == CALIED_FLAG){
		img_center_yaw = gAppParam.CameraCali.GimbalYawOffset;
		img_center_pit = gAppParam.CameraCali.GimbalPitOffset;
	}
	/*TODO : if camera NOT calied before, DO NOT allow enter big buff mode*/
}
/** @map  
		yaw+	
		<-----0						1		2		3
					|						4		5		6
					↓	/pit+			7		8		9,  5=(0,0) + offset(y5,p5);
	*	@note this pit yaw is always relative!*/
void update_gimbal_position_table(s16 y5,s16 p5, s16 y9,s16 p9 ){
	u8 i;
	s16 deltaP,deltaY;
	
	deltaP = abs(p9-p5);
	deltaY = abs(y9-y5);
	//9 line 2 col
	//this table represents 9 points,point format = [yaw,pit]
	s16 tab[9][2] = 
	{
		deltaY,-deltaP,			0, -deltaP,			-deltaY, -deltaP,
		deltaY,			 0,			0, 			 0,			-deltaY, 			 0,
		deltaY, deltaP,			0,  deltaP,			-deltaY,  deltaP,
	};
	for(i=0; i<9; i++){
		big_buff_table[i][0] = tab[i][0] + y5;	//point5 is offset
		big_buff_table[i][1] = tab[i][1] + p5;
	}	
}

void gimbal_point2target(u8 num){
	if (num>8) return;
	gYaw.ctrl_mode = GIMBAL_CLOSE_LOOP_ENCODER;
	yaw_pos_ref = big_buff_table[num][0];
	pit_pos_ref = big_buff_table[num][1] - 20;
	//pit add -20, because of gravity make gun a little down(pit)
}

//eg: full image = 1280*720, so image center = 640*360
//it have offset of cause, so add offset will be [xxx,xxx]
//#define IMG_CENTER_YAW	775	//this 2 offset read from flash when calied before.
//#define IMG_CENTER_PIT	459


int t[4];	//just for test

void autoshot_big_buff(){
	//first move laser to center(5), remember here as img y5,p5.
	//send CMD_CALI_5 enter cali 5 mode, manifold return image position ,
	//control gimbal move to image y5,p5, record p y relative pos,
	//control gimbal move to image y9 p9, record p y relative pos.\
	//call @func update_gimbal_position_table(...)
	//switch cmd to CMD_FIND_NUM, ready to shoot
	//a key to triger big buff mode.
	//ps. wait to add more protect code when cali 5+9
	/*============== accoding to camera cali_point_5_9 ==============*/
	
	
	//cameraCaliHook(cv_big_buff.y5, cv_big_buff.p5);
	is_in_big_buff_mode = 0;
	if(rc.kb.bit.C || t[2]){
		is_in_big_buff_mode = 1;
		set_manifold_vision_mode(CMD_CALI_5);
		t[0] = pid_calc(&pid_cali_bby, img_center_yaw, cv_big_buff.y5);
		t[1] = pid_calc(&pid_cali_bbp, img_center_pit, cv_big_buff.p5);
		
		yaw_pos_ref = -t[0]*1.078f;
		pit_pos_ref = t[1]*1.078f;//same reason. pit have gravity
		if(rc.kb.bit.Q){//record current position as 5
			cv_big_buff.calied_y5 = yaw_relative_pos;
			cv_big_buff.calied_p5 = pit_relative_pos;
		}
	}
	
	if(rc.kb.bit.V  ||  t[3] ){
		is_in_big_buff_mode = 1;
		set_manifold_vision_mode(CMD_CALI_9);
		//if(img calied before	&& uart rx img y p is normal)
		t[0] = pid_calc(&pid_cali_bby, img_center_yaw, cv_big_buff.y9);
		t[1] = pid_calc(&pid_cali_bbp, img_center_pit, cv_big_buff.p9);
		
		yaw_pos_ref = -t[0]*1.078f;
		pit_pos_ref = t[1]*1.078f;//same reason. pit have gravity
		
		if(rc.kb.bit.Q){//record current position as 9
			cv_big_buff.calied_y9 = yaw_relative_pos;
			cv_big_buff.calied_p9 = pit_relative_pos;
		}
	}
	if(rc.kb.bit.R){		//if cali failed, use R to re cali
		pid_cali_bby.iout = 0;
		pid_cali_bbp.iout = 0;
	}
	/*============== accoding to camera cali_point_5_9 ==============*/
	
	
	update_gimbal_position_table(cv_big_buff.calied_y5,
																	cv_big_buff.calied_p5,
																	cv_big_buff.calied_y9,
																	cv_big_buff.calied_p9);
	/*============== start recognise and shooting ==============*/
	if (rc.kb.bit.X){
		is_in_big_buff_mode = 1;
		set_manifold_vision_mode(CMD_FIND_NUM);
		if(cv_big_buff.target_num != 0)				
			gimbal_point2target(cv_big_buff.target_num-1);
			
		if(cv_big_buff.last_num != cv_big_buff.target_num ){
			autoshot_cmd = 1;	//poke rotate 60'
		}
		else{
			autoshot_cmd = 0;
		}
		cv_big_buff.last_num = cv_big_buff.target_num;//update old num
	}
	
}
#endif

/**
	*@aim 		get position relative to center,center = GimbalCaliData.GimbalPit/Yaw Offset;
	*@range  	gimbal move range = [center-MOVE_RANGE, center+MOVE_RANGE]
	*@note	 	process when angle cross 0 !
	*/
s16 get_relative_pos(s16 raw_ecd, s16 center_offset)
{
    s16 tmp = 0;
    if (center_offset >= 4096)
    {
        if (raw_ecd > center_offset - 4096)
            tmp = raw_ecd - center_offset;
        else
            tmp = raw_ecd + 8192 - center_offset;
    }
    else
    {
        if (raw_ecd > center_offset + 4096)
            tmp = raw_ecd - 8192 - center_offset;
        else
            tmp = raw_ecd - center_offset;
    }
    return tmp;
}

void can_send_gimbal_iq(s16 yaw_iq, s16 pit_iq, s16 poke_iq)
{

    GIMBAL_CAN.pTxMsg->StdId   = 0x1ff;
    GIMBAL_CAN.pTxMsg->IDE     = CAN_ID_STD;
    GIMBAL_CAN.pTxMsg->RTR     = CAN_RTR_DATA;
    GIMBAL_CAN.pTxMsg->DLC     = 8;
    GIMBAL_CAN.pTxMsg->Data[0] = yaw_iq >> 8;
    GIMBAL_CAN.pTxMsg->Data[1] = yaw_iq;
    GIMBAL_CAN.pTxMsg->Data[2] = pit_iq >> 8;
    GIMBAL_CAN.pTxMsg->Data[3] = pit_iq;
    GIMBAL_CAN.pTxMsg->Data[4] = poke_iq >> 8;
    GIMBAL_CAN.pTxMsg->Data[5] = poke_iq;
    GIMBAL_CAN.pTxMsg->Data[6] = 0;
    GIMBAL_CAN.pTxMsg->Data[7] = 0;
    HAL_CAN_Transmit(&GIMBAL_CAN, 1000);
}

void can_cali_gimbal()
{

    GIMBAL_CAN.pTxMsg->StdId   = 0x3f0;
    GIMBAL_CAN.pTxMsg->IDE     = CAN_ID_STD;
    GIMBAL_CAN.pTxMsg->RTR     = CAN_RTR_DATA;
    GIMBAL_CAN.pTxMsg->DLC     = 8;
    GIMBAL_CAN.pTxMsg->Data[0] = 'c';
    GIMBAL_CAN.pTxMsg->Data[1] = 0;
    GIMBAL_CAN.pTxMsg->Data[2] = 0 >> 8;
    GIMBAL_CAN.pTxMsg->Data[3] = 0;
    GIMBAL_CAN.pTxMsg->Data[4] = 0 >> 8;
    GIMBAL_CAN.pTxMsg->Data[5] = 0;
    GIMBAL_CAN.pTxMsg->Data[6] = 0;
    GIMBAL_CAN.pTxMsg->Data[7] = 0;
    HAL_CAN_Transmit(&GIMBAL_CAN, 1000);
}
void can_cali_old()
{

    GIMBAL_CAN.pTxMsg->StdId   = 0x1ff;
    GIMBAL_CAN.pTxMsg->IDE     = CAN_ID_STD;
    GIMBAL_CAN.pTxMsg->RTR     = CAN_RTR_DATA;
    GIMBAL_CAN.pTxMsg->DLC     = 8;
    GIMBAL_CAN.pTxMsg->Data[0] = 0;
    GIMBAL_CAN.pTxMsg->Data[1] = 0;
    GIMBAL_CAN.pTxMsg->Data[2] = 0 >> 8;
    GIMBAL_CAN.pTxMsg->Data[3] = 0;
    GIMBAL_CAN.pTxMsg->Data[4] = 0 >> 8;
    GIMBAL_CAN.pTxMsg->Data[5] = 0;
    GIMBAL_CAN.pTxMsg->Data[6] = 4;
    GIMBAL_CAN.pTxMsg->Data[7] = 0;
    HAL_CAN_Transmit(&GIMBAL_CAN, 1000);
}

int iq=50;
//void can_cali_gimbal();
/**
    *@bref - pitch + yaw gimbal double loop control
    *@TODO : ADD speed pid loop, done!
    */
void gimbal_task(const void* argu)
{

    PID_struct_init(&pid_pit, POSITION_PID, 4000, 1000,
										1.0f, 0.0f, 0.f); //15.0f,	0.08f,	50.1f
    PID_struct_init(&pid_pit_omg, POSITION_PID, 4000, 400,
                    14.0f, 0, 0.0f);

    PID_struct_init(&pid_yaw, POSITION_PID, 4000, 400,
                    180, 0, 0); //1.8f,	0.032f,	65	//230
    PID_struct_init(&pid_yaw_omg, POSITION_PID, 4000, 400,
                    25, 0, 0);

    PID_struct_init(&pid_poke_omg, POSITION_PID, 5000, 1000, //omega
                    0.1, 0, 0);
    PID_struct_init(&pid_poke, POSITION_PID, 5000, 1000, //position
                    2, 0.0f, 0);

    PID_struct_init(&pid_imu_tmp, POSITION_PID, 999, 999, //position
                    180, 0.1f, 0);

    /* mpu6500 & istxxx magnet meter init ,for angular rate measure*/
    mpu_device_init();
    //read gimbal offset from gAppParam.//if not calied before , not allow gimbal run
		
    if (gAppParam.GimbalCaliData.isAlreadyCalied == CALIED_FLAG)
    {
        yaw_center_offset = gAppParam.GimbalCaliData.GimbalYawOffset;
        pit_center_offset = gAppParam.GimbalCaliData.GimbalPitOffset;
    }
    else
    {
			
				//gAppParam.GimbalCaliData.GimbalNeedCali = 1;
				//gimbalCaliHook();
        //err.
				
        while (1)
            ;
    }
//		can_cali_gimbal();
//		can_cali_old();
		gYaw.ctrl_mode    = GIMBAL_CLOSE_LOOP_ENCODER; //hide encoder mode!!!
//		gYaw.zgyro_offset = get_relative_pos(moto_yaw.angle, yaw_center_offset) / 22.75f; //get offset

    while (1)
    {

        //keep imu in a constant temperature
        imu_tmp = 21 + mpu_data.temp / 333.87f;
        pid_calc(&pid_imu_tmp, imu_tmp, imu_tmp_ref); //keep in 40 degree
        TIM3->CCR2 = pid_imu_tmp.pos_out; //heat imu.

        /* get angular speed, for gimbal speed loop */
        mpu_get_data();
        gimbalCaliHook();

        if (gYaw.last_mode == GIMBAL_RELAX && gYaw.ctrl_mode != GIMBAL_RELAX)
            pit_pos_ref = 0; //back to center
        pit_pos_ref += -rc.ch4 * 0.05f * GIMBAL_RC_MOVE_RATIO_PIT
            + rc.mouse.y * 0.8f * GIMBAL_PC_MOVE_RATIO_PIT;
        VAL_LIMIT(pit_pos_ref, -PIT_MOVE_RANGE, PIT_MOVE_RANGE);
        //pit_pos_ref = pit_pos_rc_int + pit_pos_mouse_int - (euler[2]+160)*40.73f;//add imu stable,wait to increse imu accuracy

        gYaw.last_mode = gYaw.ctrl_mode;
        switch (rc.sw2)
        {
        case (RC_UP):
            gYaw.ctrl_mode = GIMBAL_CLOSE_LOOP_ENCODER;
            break;
        case (RC_MI):
            gYaw.ctrl_mode = GIMBAL_RELAX;
            break;
        case (RC_DN):
            gYaw.ctrl_mode = GIMBAL_RELAX;
            //gYaw.ctrl_mode = GIMBAL_CLOSE_LOOP_ENCODER;//hide encoder mode!!!
            //gYaw.ctrl_mode = GIMBAL_CLOSE_LOOP_ENCODER;//no matter how gimbal work, chassis follow gimbal
            break;
        default:
            break;
        }

        if (gYaw.ctrl_mode == GIMBAL_CLOSE_LOOP_ENCODER)
        {
            yaw_pos_ref = -rc.ch3;
        }

        //autoshot_big_buff();//NOT USE

        /*shoot poke moto task*/
        
				if (fric_wheel_run && (autoshot_cmd || key_fsm(&mouse_left_sta, rc.mouse.l)))
        {
            poke_pos_ref += 135863 * poke_dir; //60'理论值=136530 实际测100圈 get 135863 stable as dog
            autoshot_cmd = 0;
        }
				
        if (rc.sw2 == RC_UP && fric_wheel_run && rc.sw1 == RC_DN)
            poke_pos_ref += POKE_MOTOR_SPEED * poke_dir;

        if (key_fsm(&mouse_right_sta, rc.mouse.r) || last_sw1 == RC_MI && rc.sw1 == RC_UP && rc.sw2 == RC_UP)
        {
            fric_wheel_run = !fric_wheel_run;
        }
				
        last_sw1 = rc.sw1;
				
        if (fric_wheel_run && rc.sw2 == RC_UP && !gRxErr.err_list[DbusTOE].err_exist)
        {
            TIM12->CCR1 = TIM12->CCR2 = SHOT_FRIC_WHEEL_SPEED; //if fric run faster, gyro will have too much noise
            HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_SET);
            //open laser
        }
        else
        {
            TIM12->CCR1 = TIM12->CCR2 = 1000;
            //HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_RESET);
            //close laser
        }

        //transform  absolute ecd range [0,8192] to relative range [-MOVE_RANGE, MOVE_RANGE],
        yaw_relative_pos = get_relative_pos(moto_yaw.angle, yaw_center_offset);
        pit_relative_pos = get_relative_pos(moto_pit.angle, pit_center_offset);

        /* double loop gimbal pid calculate, stable as dog~ */
        if (gYaw.ctrl_mode == GIMBAL_CLOSE_LOOP_ENCODER)
            pid_calc(&pid_yaw, yaw_relative_pos / 22.75f, yaw_pos_ref / 22.75f); //change to 360'

        pid_calc(&pid_yaw_omg, mpu_data.gz / 10.0f, -pid_yaw.pos_out / 10.0f); //

        pid_calc(&pid_pit, pit_relative_pos, pit_pos_ref);
        pid_calc(&pid_pit_omg, mpu_data.gy / 10.0f, pid_pit.pos_out / 10.0f); //

        pid_calc(&pid_poke, moto_poke.total_angle / 100, poke_pos_ref / 100);
        pid_calc(&pid_poke_omg, moto_poke.speed_rpm, pid_poke.pos_out);

        //final output, for safe protect purpose
        if (rc.sw2 == RC_UP && gAppParam.GimbalCaliData.isAlreadyCalied == 0x55
            && gYaw.ctrl_mode != GIMBAL_RELAX
            && !gRxErr.err_list[DbusTOE].err_exist
            && !gRxErr.err_list[GimbalYawTOE].err_exist
            && !gRxErr.err_list[GimbalPitTOE].err_exist)
        {

            can_send_gimbal_iq(pid_yaw_omg.pos_out, -pid_pit_omg.pos_out, pid_poke.pos_out);
					//can_send_gimbal_iq(iq, iq, pid_poke.pos_out);
        }
        else
        {
            pid_poke.iout = 0;
            can_send_gimbal_iq(0, 0, 0); //relax state
        }
        /* just for debug function*/
        /* just for debug function*/
        /* for jscope watch */
        watch_param[0] = pid_yaw_omg.get[0];
        watch_param[1] = pid_yaw_omg.set[0];
        watch_param[2] = pid_yaw.get[0];
        watch_param[3] = pid_yaw.set[0];

        watch_pit[0] = pid_pit_omg.get[0];
        watch_pit[1] = pid_pit_omg.set[0];
        watch_pit[2] = pid_pit.get[0];
        watch_pit[3] = pid_pit.set[0];
        /* for jscope watch */

        osDelay(5);
    }
}
