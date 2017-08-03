#include "test_usart.h"
#include "usart.h"
#include "dbus.h"
  
uint8_t uart3_rx_buff[50];
uint8_t uart6_rx_buff[50];
uint8_t dbus_buf[DBUS_BUF_SIZE];
char data[4];
char old_data;
uint8_t p=0;
//it will be auto callback when usart receive msg completely
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{ 
  if(huart == &huart3)
  {
    __HAL_UART_CLEAR_PEFLAG(&huart3);
    HAL_UART_Receive_IT(&huart3, uart3_rx_buff, 1);
		//HAL_UART_Transmit(&huart3, uart3_rx_buff, 1, 10);
		printf("1 %c%c%c%c%c\n",uart3_rx_buff[0], data[0], data[1], data[2], data[3]);
		
		if((uart3_rx_buff[0]=='a')||(data[0]=='a')){
			data[p]=uart3_rx_buff[0];
			p++;
		if(p==4){
			printf("t\n");
			if(old_data==data[3]){
				printf("%c%c%c%c", data[0], data[1], data[2], data[3]);
				p=0;
				data[0]='p';
			}
			old_data=data[3];
			printf("o%d\n", old_data);
		}
		
		}
  }
	if(huart == &huart6)
  {
    __HAL_UART_CLEAR_PEFLAG(&huart6);
    HAL_UART_Receive_IT(&huart6, uart6_rx_buff, 1);
		HAL_UART_Transmit(&huart6, uart6_rx_buff, 1, 10);
  }
  
}
