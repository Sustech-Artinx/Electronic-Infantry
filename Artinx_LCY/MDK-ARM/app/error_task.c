/* Includes ------------------------------------------------------------------*/
#include "error_task.h"
#include "cmsis_os.h"
#include "led.h"
#include "mytype.h"
#include "stm32f4xx_hal.h"

#define BEEP_ON (TIM3->CCR1 = 599)
#define BEEP_OFF (TIM3->CCR1 = 0)

#define BEEP_ARR (TIM3->ARR)
#define BEEP_CH (TIM3->CCR1)

/* global variables --------------------------*/
GlbRxErrTypeDef gRxErr;
//={
//ErrorFlagTypedef	err_list[ErrorListLength];
//ErrorFlagTypedef* err_now = NULL;
//};

void* err_str[ErrorListLength] = {
    "none",
    "Gyro",
    "mot1",
    "mot2",
    "mot3",
    "mot4",
    "CuSa",
    "Dbus",
    "Judg",
    "PitX",
    "YawX",
    "Poke",
};

const u16 note_arr_tab[] = {
    382, 341, 303, 286, 255, 227, 202, //低音7
    191, 170, 152, 143, 128, 114, 101, //中音7
    96, 85, 76, 72, 64, 57, 51, //高音7
};

//sing 100ms
void sing(u8 note)
{
    if (Silent == note)
        BEEP_CH = 0;
    else
    {
        BEEP_ARR = note_arr_tab[note];
        BEEP_CH  = note_arr_tab[note] / 2;
    }
}

void beep_sing(int s)
{

    BEEP_ARR = 999;
    if (s & 0x80000000)
        LED_R_ON; //BEEP_ON;
    else
        LED_R_OFF; //BEEP_OFF;
}

void global_err_detector_init(void)
{
    gRxErr.err_now = NULL;

    gRxErr.err_list[DbusTOE].err_exist = 0;
    //	gRxErr.err_list[DbusTOE].err_id = DbusTOE;	//seems no use
    gRxErr.err_list[DbusTOE].warn_pri    = 10; //max !
    gRxErr.err_list[DbusTOE].beep_mask   = Bi_Bi;
    gRxErr.err_list[DbusTOE].set_timeout = 100; //ms
    gRxErr.err_list[DbusTOE].delta_time  = 0;
    gRxErr.err_list[DbusTOE].last_time   = 0x00;
    gRxErr.err_list[DbusTOE].enable      = 1;

    for (int i = 0; i < 4; i++)
    {
        gRxErr.err_list[ChassisMoto1TOE + i].err_exist = 0;
        //		gRxErr.err_list[ChassisMoto1TOE+i].err_id = ChassisMoto1TOE;
        gRxErr.err_list[ChassisMoto1TOE + i].warn_pri    = 6 + i; //6,7,8,9
        gRxErr.err_list[ChassisMoto1TOE + i].beep_mask   = Biii_Biii_Bi;
        gRxErr.err_list[ChassisMoto1TOE + i].set_timeout = 200;
        gRxErr.err_list[ChassisMoto1TOE + i].delta_time  = 0;
        gRxErr.err_list[ChassisMoto1TOE + i].last_time   = 0x00;
        gRxErr.err_list[ChassisMoto1TOE + i].enable      = 1;
    }
    //gRxErr.err_list[ChassisMoto1TOE].enable = 1;

    gRxErr.err_list[ChassisGyroTOE].err_exist = 0;
    //gRxErr.err_list[ChassisGyroTOE].err_id = ChassisGyroTOE;
    gRxErr.err_list[ChassisGyroTOE].warn_pri    = 5;
    gRxErr.err_list[ChassisGyroTOE].beep_mask   = Bi_Biii_Bi;
    gRxErr.err_list[ChassisGyroTOE].set_timeout = 10;
    gRxErr.err_list[ChassisGyroTOE].delta_time  = 0;
    gRxErr.err_list[ChassisGyroTOE].last_time   = 0x00;
    gRxErr.err_list[ChassisGyroTOE].enable      = 1;

    gRxErr.err_list[CurrentSampleTOE].err_exist = 0;
    //	gRxErr.err_list[CurrentSampleTOE].err_id = CurrentSampleTOE;
    gRxErr.err_list[CurrentSampleTOE].warn_pri    = 4;
    gRxErr.err_list[CurrentSampleTOE].beep_mask   = Bi_Bi_Bi;
    gRxErr.err_list[CurrentSampleTOE].set_timeout = 200;
    gRxErr.err_list[CurrentSampleTOE].delta_time  = 0;
    gRxErr.err_list[CurrentSampleTOE].last_time   = 0x00;
    //gRxErr.err_list[CurrentSampleTOE].enable = 1;

    gRxErr.err_list[GimbalPitTOE].err_exist   = 0;
    gRxErr.err_list[GimbalPitTOE].warn_pri    = 8;
    gRxErr.err_list[GimbalPitTOE].beep_mask   = Bi_Bi_Bi;
    gRxErr.err_list[GimbalPitTOE].set_timeout = 200;
    gRxErr.err_list[GimbalPitTOE].delta_time  = 0;
    gRxErr.err_list[GimbalPitTOE].last_time   = 0x00;
    gRxErr.err_list[GimbalPitTOE].enable      = 1;

    gRxErr.err_list[GimbalYawTOE].err_exist   = 0;
    gRxErr.err_list[GimbalYawTOE].warn_pri    = 8;
    gRxErr.err_list[GimbalYawTOE].beep_mask   = Bi_Bi_Bi;
    gRxErr.err_list[GimbalYawTOE].set_timeout = 200;
    gRxErr.err_list[GimbalYawTOE].delta_time  = 0;
    gRxErr.err_list[GimbalYawTOE].last_time   = 0x00;
    gRxErr.err_list[GimbalYawTOE].enable      = 1;

    gRxErr.err_list[PokeMotoTOE].err_exist   = 0;
    gRxErr.err_list[PokeMotoTOE].warn_pri    = 2;
    gRxErr.err_list[PokeMotoTOE].beep_mask   = Bi_Bi_Bi;
    gRxErr.err_list[PokeMotoTOE].set_timeout = 200;
    gRxErr.err_list[PokeMotoTOE].delta_time  = 0;
    gRxErr.err_list[PokeMotoTOE].last_time   = 0x00;
    gRxErr.err_list[PokeMotoTOE].enable      = 1;

    gRxErr.str = err_str[NoTimeOutErr];
}

//把这个函数插入到中断回调或者触发式中断同步函数中如CAN rx callback， usart。。。
void err_detector_hook(int err_id)
{

    if (gRxErr.err_list[err_id].enable)
        gRxErr.err_list[err_id].last_time = HAL_GetTick();
}

void power_up_music()
{

    sing(Fa4M);
    osDelay(250);
    sing(Mi3M);
    osDelay(250);
    sing(Re2M);
    osDelay(250);
    sing(Do1M);
    osDelay(250);
    sing(Silent);
    osDelay(100);

    sing(Do1M);
    osDelay(100);
    sing(Silent);
    osDelay(50);
    sing(Do1M);
    osDelay(100);
    sing(Re2M);
    osDelay(100);
    sing(La6L);
    osDelay(200);
    sing(So5L);
    osDelay(200);
    sing(La6L);
    osDelay(200);
    sing(Silent);
}

//不能这样写！
void err_music()
{

    sing(Re2M);
    osDelay(400);
    sing(Fa4M);
    osDelay(400);
    sing(So5M);
    osDelay(300);
    sing(Silent);
    osDelay(100);
    sing(So5M);
    osDelay(300);
    sing(La6M);
    osDelay(300);
    sing(Silent);
    osDelay(100);
    sing(La6M);
    osDelay(300);
    sing(Silent);
    osDelay(500);
}

void err_music2()
{

    sing(La6L);
    osDelay(200);
    sing(Do1M);
    osDelay(200);
    sing(Re2M);
    osDelay(200);
    sing(Silent);
    osDelay(100);

    sing(So5M);
    osDelay(500);
    sing(Mi3M);
    osDelay(500);
    sing(Fa4M);
    osDelay(200);

    sing(Mi3M);
    osDelay(200);
    sing(Do1M);
    osDelay(200);
    sing(Re2M);
    osDelay(400);
}

void StartErrDecetorTask(void const* argument)
{
    global_err_detector_init();
    osDelay(100);
    //power_up_music();
    //	err_music2();
    for (;;)
    {
        u32 systime      = HAL_GetTick();
        int max_priority = 0;
        int err_cnt      = 0;
        for (int id = 0; id < ErrorListLength; id++)
        {
            gRxErr.err_list[id].delta_time = HAL_GetTick() - gRxErr.err_list[id].last_time;
            if (gRxErr.err_list[id].enable && gRxErr.err_list[id].delta_time > gRxErr.err_list[id].set_timeout)
            {
                gRxErr.err_list[id].err_exist = 1; //now err exist!!
                err_cnt++;
                gRxErr.id = (TOE_ID_e)id;
                if (gRxErr.err_list[id].warn_pri > max_priority)
                {
                    max_priority   = gRxErr.err_list[id].warn_pri;
                    gRxErr.err_now = &(gRxErr.err_list[id]);
                    gRxErr.str     = err_str[id];
                }
                //update max
            }
            else
            {
                gRxErr.err_list[id].err_exist = 0;
            }
        }

        if (!err_cnt) //all scan no error, should clear err pointer!!!
            gRxErr.err_now = NULL;

        int beep_seq = 0;
        if (gRxErr.err_now != NULL)
        {
            beep_seq = gRxErr.err_now->beep_mask;
            for (int i = 0; i < 32; i++)
            {
                beep_sing(beep_seq << i);
                osDelay(100);
            }
            LED_G_OFF;
        }
        else
        {
            beep_sing(0); //必须写！！不然导致有时候一直长响
            LED_G_ON;
        }

        //clear err_now every 100ms

        //clr_err_cnt = 0;
        gRxErr.str = err_str[NoTimeOutErr];
        osDelay(50);
    }
}

//C1 C2 C3
void set_led(int led)
{
    LED_PORT->ODR &= ~(7 << 1); //因为1是灭 所以取反
    LED_PORT->ODR |= ~(led << 1);
}

void err_deadloop()
{

    set_led(LED_R | LED_B);
    while (1)
    {
        set_led(LED_R | LED_B);
        HAL_Delay(300);
        set_led(LED_R | LED_B);
        HAL_Delay(1000);
    }
}
