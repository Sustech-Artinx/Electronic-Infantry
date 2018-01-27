#include "stm32f4xx_hal.h"
#include "uart_dma_queue.h"

static uint8_t UART_DMA_RX_deQueue(UART_RX_Queue *self);
inline static bool UART_DMA_RX_isEmpty(UART_RX_Queue *self);
inline static bool UART_DMA_RX_isFull(UART_RX_Queue *self);

/* This queue is used for receiving uart_rx data through DMA */

void UART_DMA_RX_Queue_Init(UART_RX_Queue *queue, UART_HandleTypeDef *huart, DMA_HandleTypeDef *hdma_usart_rx){
	//init for USART6_RX
	queue->head = 0;
	queue->dma_counter = (uint32_t*)&__HAL_DMA_GET_COUNTER(hdma_usart_rx);
	queue->deQueue = UART_DMA_RX_deQueue;
	queue->isEmpty = UART_DMA_RX_isEmpty;
	queue->isFull = UART_DMA_RX_isFull;
	HAL_DMA_Start(hdma_usart_rx, (uint32_t)&huart->Instance->DR, (uint32_t)queue->arr, UART_RX_QUEUE_SIZE);
	SET_BIT(huart->Instance->CR3, USART_CR3_DMAR);
}

static uint8_t UART_DMA_RX_deQueue(UART_RX_Queue *self){
	if(UART_DMA_RX_isEmpty(self)){return 0;}
	uint8_t data = self->arr[self->head];
	self->head = (self->head + 1) % UART_RX_QUEUE_SIZE;
	return data;
}

inline static bool UART_DMA_RX_isEmpty(UART_RX_Queue *self){
	if(self->head == (UART_RX_QUEUE_SIZE-*(self->dma_counter))){
		return true;
	}else {
		return false;
	}
}

inline static bool UART_DMA_RX_isFull(UART_RX_Queue *self){
	if(self->head == ((UART_RX_QUEUE_SIZE-*(self->dma_counter))+1)%UART_RX_QUEUE_SIZE){
		return true;
	}else {
		return false;
	}
}
