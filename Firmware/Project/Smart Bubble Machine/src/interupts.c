#include "taskovi.h"


xQueueHandle commandQueue = NULL; 
xQueueHandle WiFly_commandQueue = NULL;

char flag = 0;

//Interupt handler za Bluetooth USART
void USART2_IRQHandler(){
	uint8_t buffer;
	static portBASE_TYPE xHigherPriorityTaskWoken;
	xHigherPriorityTaskWoken = pdFALSE;
  NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 ); //Na neku foru se bitovi za subriority poremete. Treba naci alternativu
	buffer= USART2->DR;
	USART_SendData(USART2, buffer); //Provjera
	xQueueSendToBackFromISR(commandQueue, &buffer , &xHigherPriorityTaskWoken);

	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

void USART3_IRQHandler(){
	uint8_t buffer;
	static portBASE_TYPE xHigherPriorityTaskWoken;
	xHigherPriorityTaskWoken = pdFALSE;
  NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 ); //Na neku foru se bitovi za subriority poremete. Treba naci alternativu
	buffer= USART3->DR;
	if(buffer == '!') {
		flag = 1;
	} else if (buffer == '#') {
		flag = 0;
			xQueueSendToBackFromISR(WiFly_commandQueue, &buffer , &xHigherPriorityTaskWoken);
			USART_SendData(USART2, buffer);
			portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
	}
	if(flag == 1) {
			xQueueSendToBackFromISR(WiFly_commandQueue, &buffer , &xHigherPriorityTaskWoken);
			USART_SendData(USART2, buffer);
			portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
	}
	
}

