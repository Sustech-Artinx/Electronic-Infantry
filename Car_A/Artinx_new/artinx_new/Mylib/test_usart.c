#include "test_usart.h"
#include "usart.h"
#include "dbus.h"
  
uint8_t uart3_rx_buff[50];
uint8_t uart6_rx_buff[50];
uint8_t dbus_buf[DBUS_BUF_SIZE];
uint8_t p=0;
//it will be auto callback when usart receive msg completely
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{ 
  if(huart == &huart3)
  {
    __HAL_UART_CLEAR_PEFLAG(&huart3);
    HAL_UART_Receive_IT(&huart3, uart3_rx_buff, 1);
		HAL_UART_Transmit(&huart3, uart3_rx_buff, 1, 10);
  }
	if(huart == &huart6)
  {
    __HAL_UART_CLEAR_PEFLAG(&huart6);
    HAL_UART_Receive_IT(&huart6, uart6_rx_buff, 1);
		HAL_UART_Transmit(&huart6, uart6_rx_buff, 1, 10);
  }
  
}
