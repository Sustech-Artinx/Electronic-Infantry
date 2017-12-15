#include "stm32f4xx_hal.h"
#include "uart_dma_queue.h"

static uint8_t UART_RX_deQueue(UART_RX_Queue *self);
inline static bool UART_RX_isEmpty(UART_RX_Queue *self);
inline static bool UART_RX_isFull(UART_RX_Queue *self);

extern UART_HandleTypeDef huart6;
extern DMA_HandleTypeDef hdma_usart6_rx;

UART_RX_Queue USART6_RX;

void UART_RX_Queue_Init(void){
	//init for USART6_RX
	USART6_RX.head = 0;
	USART6_RX.dma_counter = (uint32_t*)&__HAL_DMA_GET_COUNTER(&hdma_usart6_rx);
	USART6_RX.deQueue = UART_RX_deQueue;
	USART6_RX.isEmpty = UART_RX_isEmpty;
	USART6_RX.isFull = UART_RX_isFull;
	HAL_DMA_Start(&hdma_usart6_rx, (uint32_t)&huart6.Instance->DR, (uint32_t)USART6_RX.arr, UART_RX_QUEUE_SIZE);
	SET_BIT(huart6.Instance->CR3, USART_CR3_DMAR);
}

static uint8_t UART_RX_deQueue(UART_RX_Queue *self){
	if(UART_RX_isEmpty(self)){return 0;}
	uint8_t data = self->arr[self->head];
	self->head = (self->head + 1) % UART_RX_QUEUE_SIZE;
	return data;
}

inline static bool UART_RX_isEmpty(UART_RX_Queue *self){
	if(self->head == (UART_RX_QUEUE_SIZE-*(self->dma_counter))){
		return true;
	}else {
		return false;
	}
}

inline static bool UART_RX_isFull(UART_RX_Queue *self){
	if(self->head == ((UART_RX_QUEUE_SIZE-*(self->dma_counter))+1)%UART_RX_QUEUE_SIZE){
		return true;
	}else {
		return false;
	}
}
