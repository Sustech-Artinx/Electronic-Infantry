#include "stm32f4xx_hal.h"
#include "communication.h"

//#define CRC_Divider  ？  //CRC校验除数

#define UART6_RECEIVEBUF_SIZE 1024

UARTProtocol_HandleTypeDef hprotocol_uart6;
static uint8_t uart6_receive_buffer[UART6_RECEIVEBUF_SIZE];

void CommunicationInit(void){
	extern UART_RX_Queue USART6_RX;
	hprotocol_uart6.buffer = uart6_receive_buffer;
	hprotocol_uart6.bufferSize = UART6_RECEIVEBUF_SIZE;
	hprotocol_uart6.source = &USART6_RX;
	hprotocol_uart6.state.recState = RECOP;
	hprotocol_uart6.isFrameReceived = false;
}

void UART_Protocol_Unpacker(UARTProtocol_HandleTypeDef *handler){
	while(!handler->source->isEmpty(handler->source)){
		uint8_t data = handler->source->deQueue(handler->source);
		switch(handler->state.recState){
			case RECOP:{
				if(data==0xFA){
					handler->state.recState=RECLEN1;
				}
				break;
			}
			case RECLEN1:{
				handler->dataLen = (uint16_t)data;
				handler->state.recState=RECLEN2;
				handler->isFrameReceived = false;
				break;
			}
			case RECLEN2:{
				handler->dataLen |= data<<8;
				handler->state.recState=RECSEL;
				handler->state.dataRecProgress = 0;
				//receive error, buffer is too small
				if(handler->dataLen > handler->bufferSize){
					handler->state.recState=RECOP;
				}
				break;
			}
			case RECSEL:{
				handler->buffer[handler->state.dataRecProgress] = data;
				handler->state.dataRecProgress++;
				//data receive complete
				if(handler->state.dataRecProgress >= handler->dataLen){
					handler->state.recState=RECCHECK;
				}
				break;
			}
			case RECCHECK:{
				//CRC check
				handler->state.recState=RECEND;
	//			if(  ？^CRC_Divider==0)//如何把前面的数据合并
	//			{
	//				handler->state.recState=RECEND;//校验成功
	//			}
	//			else
	//			{
	//				//数据校验错误，丢弃
	//				handler->state.recState=RECOP;
	//				UART_RX_STA[0]=0;
	//			}
				break;
			}
			case RECEND:{
				if(data==0xFB){
					//frame complete
					handler->isFrameReceived = true;
					handler->state.recState=RECOP;
				}else {
					//RECEND error
					handler->state.recState=RECOP;
				}
			}
		}
	}
}
