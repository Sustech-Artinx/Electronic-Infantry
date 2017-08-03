
/**
  * @update 2016年5月24日22:50:28
  * @note  更新了串口接收的方式。
        *				//使用DMA来接收，每当空闲中断的时候，也就是不收数据了的时候处理接收到的数据。
        *				//DMA中断正常情况下是不触发的，
  *要保证MAX-RX-SIZE大于每次的一帧数据长度
        *				//所以DMA的作用就是加快收数据的速率，然后其他没了
        *				//################### 状态机：
  *########################
        *				进入空闲中断--处理收到的数据--清除中断标记
        *				//--重新初始化DMA--等待接收数据直到空闲--进入空闲中断
  *       //################### 状态机： ########################
  * @after	测试过一次，成功！ 后面可能还要测一下
  */

#include "bsp_uart.h"
#include "error_task.h"
#include "judge_sys.h"
#include "mytype.h"
#include "pid.h"
#include "sys.h"
#include "usart.h"

// NOT USE
#if defined STM32F4
u32         JumpIapFlag __attribute__((at(0x40024000)));
#endif

#define MAX_DMA_COUNT 100
#define DBUS_RX_MAX_BUFLEN 20
#define BT_RX_MAX_BUFLEN 100 //这个只要比一帧大就行拉

#define ARMAPI extern "C" // add this before hal callback

RC_Type          rc;
CV_BigBuff_t     cv_big_buff;
u8               cv_num = 0;
u8               bt_rx_buff[BT_RX_MAX_BUFLEN];
u8               dbus_buff[DBUS_RX_MAX_BUFLEN];
u8               cv_buff[20];
FloatConvertType rcDataTable[RcTableLength];
float            easyTable[RcTableLength];
uint8_t          transmit_frame[FrameLength];
u32              frame_cnt;

// enable global uart it, do not use DMA transfer done it!!!
static int UART_Receive_DMA_No_IT(UART_HandleTypeDef* huart, u8* pData, u32 Size)
{
    uint32_t tmp1 = 0;

    tmp1 = huart->RxState;
    if (tmp1 == HAL_UART_STATE_READY)
    {
        if ((pData == NULL) || (Size == 0))
        {
            return HAL_ERROR;
        }

        /* Process Locked */
        __HAL_LOCK(huart);

        huart->pRxBuffPtr = pData;
        huart->RxXferSize = Size;
        huart->ErrorCode  = HAL_UART_ERROR_NONE;

        /* Enable the DMA Stream */
        HAL_DMA_Start(huart->hdmarx, (uint32_t)&huart->Instance->DR,
                      (uint32_t)pData, Size);

        /* Enable the DMA transfer for the receiver request by setting the DMAR bit
    in the UART CR3 register */
        huart->Instance->CR3 |= USART_CR3_DMAR;

        /* Process Unlocked */
        __HAL_UNLOCK(huart);

        return HAL_OK;
    }
    else
    {
        return HAL_BUSY;
    }
}

void judge_sys_init()
{
#ifdef JUDGE_HUART
    __HAL_UART_CLEAR_IDLEFLAG(&JUDGE_HUART);
    __HAL_UART_ENABLE_IT(&JUDGE_HUART, UART_IT_IDLE);
    UART_Receive_DMA_No_IT(&JUDGE_HUART, judge_buf, FRAME_BUFLEN);
#endif
}

void manifold_uart_init()
{
#ifdef CV_HUART
    __HAL_UART_CLEAR_IDLEFLAG(&CV_HUART);
    __HAL_UART_ENABLE_IT(&CV_HUART, UART_IT_IDLE);
    UART_Receive_DMA_No_IT(&CV_HUART, cv_buff, 20);
#endif
}
void dbus_init()
{
    __HAL_UART_CLEAR_IDLEFLAG(&DBUS_HUART);
    __HAL_UART_ENABLE_IT(&DBUS_HUART, UART_IT_IDLE);
    // clear idle it flag & open idle it
    UART_Receive_DMA_No_IT(&DBUS_HUART, dbus_buff, DBUS_RX_MAX_BUFLEN);
    //下面一句使用联合体来代替解码，效率高很多
}

void bt_init()
{
    __HAL_UART_CLEAR_IDLEFLAG(&BT_HUART);
    __HAL_UART_ENABLE_IT(&BT_HUART, UART_IT_IDLE);
    UART_Receive_DMA_No_IT(&BT_HUART, bt_rx_buff, BT_RX_MAX_BUFLEN);
    //状态机：进入空闲中断--处理收到的数据--清除中断标记
    //--重新初始化DMA--等待接收数据直到空闲--进入空闲中断
}

/**
        *@bref use uart idle it + dma(no it) to receive a frame data.
        */
void uart_idle_dma_rx_init(UART_HandleTypeDef* huart, u8* rx_buff, u16 buff_len)
{
    __HAL_UART_CLEAR_IDLEFLAG(huart);
    __HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);
    UART_Receive_DMA_No_IT(huart, rx_buff, buff_len);
}

void uart_reset_idle_rx_callback(UART_HandleTypeDef* huart)
{
    if (__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE))
    {
        __HAL_UART_CLEAR_IDLEFLAG(huart);
        // clear idle it flag
        //重启DMA
        uint32_t DMA_FLAGS = __HAL_DMA_GET_TC_FLAG_INDEX(
            huart->hdmarx); //根据串口的不同来选择清除不同的DMA标志位

        __HAL_DMA_DISABLE(huart->hdmarx);
        __HAL_DMA_CLEAR_FLAG(huart->hdmarx, DMA_FLAGS);
        __HAL_DMA_SET_COUNTER(huart->hdmarx, MAX_DMA_COUNT);
        __HAL_DMA_ENABLE(huart->hdmarx);
    }
}

void Dma_Callback_RC_Handle(RC_Type* rc, uint8_t* buff);
//这个函数放在BT对应的 Usart-IRQHandler 里面
void LanggoUartFrameIRQHandler(UART_HandleTypeDef* huart)
{

    FloatConvertType tmp;
    // confirm uart idle it set!

    if (huart == &DBUS_HUART)
    {
        // HAL_UART_Receive_DMA(&huart4, rcUnion.buff, RC_Frame_Lentgh);
        Dma_Callback_RC_Handle(&rc, dbus_buff);
        err_detector_hook(DbusTOE);
        //使用DMA来接收，每当空闲中断的时候，也就是不收数据了的时候处理接收到的数据。
    }
#ifdef JUDGE_HUART
    else if (huart == &JUDGE_HUART)
    {
        judgementDataHandler();
    }
#endif
#ifdef CV_HUART
    else if (huart == &CV_HUART)
    {
			
    }
#endif

    else if (huart == &BT_HUART)
    {

        if (bt_rx_buff[Head1] == 0x55 && bt_rx_buff[Head2] == 0xAA)
        {
            //&& bt_rx_buff[Tail]  == 0xFF)
            frame_cnt++;

            uint8_t id  = bt_rx_buff[DataID];
            uint8_t sum = (uint8_t)(bt_rx_buff[DataID] + bt_rx_buff[Byte0] + bt_rx_buff[Byte1] + bt_rx_buff[Byte2] + bt_rx_buff[Byte3]);
            if (sum == bt_rx_buff[SumCheck])
            {

                tmp.U8type[0] = bt_rx_buff[Byte0];
                tmp.U8type[1] = bt_rx_buff[Byte1];
                tmp.U8type[2] = bt_rx_buff[Byte2];
                tmp.U8type[3] = bt_rx_buff[Byte3];

                if (id < RcTableLength)
                {
                    rcDataTable[id].F32type = tmp.F32type; //
                    easyTable[id]           = rcDataTable[id].F32type;
                }
                rcDataTable[CmdMcuGotParamOK].F32type = 1;
                uart_send_frame(&BT_HUART, CmdMcuGotParamOK);

                // receive success return OK
                if (id <= YawOmgKd)
                { // 0 - yawOmg kd
                    pid_rol.f_pid_reset(&pid_rol, rcDataTable[RolKp].F32type,
                                        rcDataTable[RolKi].F32type,
                                        rcDataTable[RolKd].F32type);
                    pid_pit.f_pid_reset(&pid_pit, rcDataTable[PitKp].F32type,
                                        rcDataTable[PitKi].F32type,
                                        rcDataTable[PitKd].F32type);
                }

                switch (id)
                {
                case CmdResetMCU:
                    if (rcDataTable[CmdResetMCU].F32type)
                        HAL_NVIC_SystemReset();
                    break;
                case CmdReadAllFromMcu:
                    for (int k = 0; k < RcTableLength; k++)
                    {
                        uart_send_frame(&BT_HUART, (RcTableType)id);
                    }
                    break;
                case CmdJump2IAP:
                    if (rcDataTable[CmdJump2IAP].F32type)
                    {
#if defined STM32F4
                        JumpIapFlag = 0x12345678;
                        NVIC_SystemReset();
#else
                        ResetAndJumpIAP_Menu();
#endif
                    }
                    break;
                case CmdSaveAllToFlash:

                    break;
                }
            }
        }
    }

    uart_reset_idle_rx_callback(huart);
}

// Warning！ ARMAPI shoule not be remove？！
// ARMAPI void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
//	//
void Dma_Callback_RC_Handle(RC_Type* rc, uint8_t* buff)
{
    //	int six_total = buff[0] + buff[1] + buff[2]
    //								+ buff[3] +
    // buff[4]
    //+
    // buff[5];
    //	if(0 == six_total)return;	//可能出现偶然情况么 - -。。。
    if (buff[0] == 0 && buff[1] == 0 && buff[2] == 0 && buff[3] == 0 && buff[4] == 0 && buff[5] == 0)
        return;
    // err_detector_callback(DbusTOE);	//a hook of dbus rx monitor

    //如果数组前面表示通道的全部为0，说明是错误数据则return;
    //	rc->ch1 = (*buff | *(buff+1) << 8) & 0x07FF;	offset  = 1024
    rc->ch1 = (buff[0] | buff[1] << 8) & 0x07FF;
    rc->ch1 -= 1024;
    rc->ch2 = (buff[1] >> 3 | buff[2] << 5) & 0x07FF;
    rc->ch2 -= 1024;
    rc->ch3 = (buff[2] >> 6 | buff[3] << 2 | buff[4] << 10) & 0x07FF;
    rc->ch3 -= 1024;
    rc->ch4 = (buff[4] >> 1 | buff[5] << 7) & 0x07FF;
    rc->ch4 -= 1024;

    rc->sw1 = ((buff[5] >> 4) & 0x000C) >> 2;
    rc->sw2 = (buff[5] >> 4) & 0x0003;

    rc->mouse.x = buff[6] | (buff[7] << 8); // x axis
    rc->mouse.y = buff[8] | (buff[9] << 8);
    rc->mouse.z = buff[10] | (buff[11] << 8);

    rc->mouse.l = buff[12]; // is pressed?
    rc->mouse.r = buff[13];

    rc->kb.key_code = buff[14] | buff[15] << 8; // key borad code
}

void uart_send_frame(UART_HandleTypeDef* huart, RcTableType id)
{
    FloatConvertType tmp;
    transmit_frame[Head1]  = 0x55;
    transmit_frame[Head2]  = 0xAA;
    transmit_frame[DataID] = id;

    tmp.F32type = rcDataTable[id].F32type;

    transmit_frame[Byte0] = tmp.U8type[0];
    transmit_frame[Byte1] = tmp.U8type[1];
    transmit_frame[Byte2] = tmp.U8type[2];
    transmit_frame[Byte3] = tmp.U8type[3];

    transmit_frame[SumCheck] = (uint8_t)(
        transmit_frame[DataID] + transmit_frame[Byte0] + transmit_frame[Byte1] + transmit_frame[Byte2] + transmit_frame[Byte3]);
    transmit_frame[Tail] = 0xFF;

    HAL_UART_Transmit(huart, transmit_frame, FrameLength, 100);
    // use blocking mode transmit
}

void uart_send2pc(UART_HandleTypeDef* huart, RcTableType id, float value)
{
    rcDataTable[id].F32type = value;
    uart_send_frame(huart, id);
}

void uart_send4byte(UART_HandleTypeDef* huart, RcTableType id, void* buff)
{

    u8* p                     = buff;
    rcDataTable[id].U8type[0] = *p;
    rcDataTable[id].U8type[1] = *(p + 1);
    rcDataTable[id].U8type[2] = *(p + 2);
    rcDataTable[id].U8type[3] = *(p + 3);

    uart_send_frame(huart, id);
}

void bt_printf(const char* fmt, ...)
{
    char    buff[128] = { 0 };
    char*   p         = buff;
    va_list ap;
    //	__va_list ap;
    va_start(ap, fmt);
    vsprintf(buff, fmt, ap);

    int size = 0;
    while (*p++)
    {
        size++;
    }

    HAL_UART_Transmit(&BT_HUART, (u8*)buff, size, 1000);
    va_end(ap);
}
