#ifndef __COMMUNICATION_H
#define __COMMUNICATION_H

#include <stdint.h>
#include "uart_dma_queue.h"

typedef struct{
	enum{
		RECOP,		//head code
		RECLEN1,	//low bits of data length
		RECLEN2,	//hight bits of data length
		RECSEL,		//data part
		RECCHECK,	//check part
		RECEND		//end code
	} recState;
	uint16_t dataRecProgress; //receive progress of data part
}UARTProtocol_StateTypeDef;

typedef struct __UARTProtocol_HandleTypeDef{
	UARTProtocol_StateTypeDef state;
	UART_RX_Queue *source;	//uart data source
	uint8_t *buffer;	//store data part
	uint16_t bufferSize;
	uint16_t dataLen;	//length of data
	bool isFrameReceived; //one frame received, need to be cleared manually
}UARTProtocol_HandleTypeDef;

void CommunicationInit(void);
void UART_Protocol_Unpacker(UARTProtocol_HandleTypeDef *handler);

#endif
