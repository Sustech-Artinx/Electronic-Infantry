#include "test_can1.h"
#include "test_can2.h"
#include "can.h"
#include "chassis_control.h"
#include "gimbal_control.h"
#include "delay.h"
#include "shoot_control.h"
/********************************************************************************
Name：          void Cmd_ESC(int16_t current_205,int16_t current_206)

Description：  	给电调板发送指令电流环控制电机，ID号为0x1FF，只用两个电调板，
                  数据回传ID为0x205和0x206
*********************************************************************************/
void Cmd_ESC(int16_t current_205,int16_t current_206)
{
    CAN_HandleTypeDef* hcan=&hcan2;
    hcan->pTxMsg->StdId = TEST_CAN2_ID ;
		hcan->pTxMsg->IDE = CAN_ID_STD;
		hcan->pTxMsg->RTR = CAN_RTR_DATA;
		hcan->pTxMsg->DLC = 0x08;
	
    hcan->pTxMsg->Data[0] = (unsigned char)(current_205 >> 8);
    hcan->pTxMsg->Data[1] = (unsigned char)current_205;
    hcan->pTxMsg->Data[2] = (unsigned char)(current_206 >> 8);
    hcan->pTxMsg->Data[3] = (unsigned char)current_206;
	  hcan->pTxMsg->Data[4] = (unsigned char)(shoot_speed_new >> 8);
		hcan->pTxMsg->Data[5] = (unsigned char)shoot_speed_new;
    hcan->pTxMsg->Data[6] = 0x00;
    hcan->pTxMsg->Data[7] = NULL;
    if(HAL_CAN_Transmit(hcan, 100)!=HAL_OK){
		printf("error_yun");
		}
}

/********************************************************************************
Name：          void Cmd_Reset(void)

Description：  	给电调板发送指令重新校正电调板,ID号为0x1FF,第6个byte为0x04
*********************************************************************************/
void Cmd_Reset(void)
{
    CAN_HandleTypeDef* hcan=&hcan2;
    
    hcan->pTxMsg->StdId = TEST_CAN2_ID ;
		hcan->pTxMsg->IDE = CAN_ID_STD;
		hcan->pTxMsg->RTR = CAN_RTR_DATA;
		hcan->pTxMsg->DLC = 0x08;
    
    hcan->pTxMsg->Data[0] = 0x00;
    hcan->pTxMsg->Data[1] = 0x00;
    hcan->pTxMsg->Data[2] = 0x00;
    hcan->pTxMsg->Data[3] = 0x00;
    hcan->pTxMsg->Data[4] = 0x00;
		hcan->pTxMsg->Data[5] = 0x00;
    hcan->pTxMsg->Data[6] = 0x04;
    hcan->pTxMsg->Data[7] = 0x00;
    if(HAL_CAN_Transmit(hcan, 10)!=HAL_OK){
		printf("error");
		}
}
