#ifndef __UART_DMA_QUEUE_H
#define __UART_DMA_QUEUE_H

#include <stdint.h>
#include <stdbool.h>

#define UART_RX_QUEUE_SIZE 100

typedef struct __UART_RX_Queue{
	uint8_t arr[UART_RX_QUEUE_SIZE];
	uint16_t head;
	uint32_t *dma_counter; //number of tasks remained
	
	uint8_t (*deQueue)(struct __UART_RX_Queue *self);
	bool (*isEmpty)(struct __UART_RX_Queue *self);
	bool (*isFull)(struct __UART_RX_Queue *self);
}UART_RX_Queue;

extern void UART_RX_Queue_Init(void);

#endif
