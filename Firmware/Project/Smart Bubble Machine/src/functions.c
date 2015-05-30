#include "functions.h"
#include "i2c.h"

uint32_t pwmIntensity = 0;
uint8_t relayState = 0;
char current_time[9];

///////////////////////////
void PWM_Init(){  //Treba ju srediti
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
	


  /* LEDs are on GPIOD */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
  
	
	GPIO_InitStructure.GPIO_Pin = PWM_PIN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN; //
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 32000;
  TIM_TimeBaseStructure.TIM_Prescaler = 41;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	  /* TIM4 enable counter */
  TIM_Cmd(TIM4, ENABLE);
	
	
  /* PWM1 Mode configuration: Channel3 */
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
  TIM_OC3Init(TIM4, &TIM_OCInitStructure);

  TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_TIM4);


 // TIM_ARRPreloadConfig(TIM4, ENABLE);

}

void set_PWM(uint8_t intesity){
	TIM4->CCR3 = 32000 - (intesity*32000)/100
	;
}

void Relay_Init(){
	GPIO_InitTypeDef init_Struct;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	init_Struct.GPIO_Mode = GPIO_Mode_OUT;
	init_Struct.GPIO_OType = GPIO_OType_PP;
	init_Struct.GPIO_Pin = RELAY_PIN;
	init_Struct.GPIO_PuPd = GPIO_PuPd_UP;
	init_Struct.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(RELAY_PORT, &init_Struct);
}

//Pali relej
void Relay_On(){
	GPIO_SetBits(RELAY_PORT, RELAY_PIN);
}

//Gasi relej
void Relay_Off(){
	GPIO_ResetBits(RELAY_PORT, RELAY_PIN);
}

//Inicijalizacija USART-a za bluetooth
// RXD	PA2	   
// TXD	PA3	 
// key  PA5
// State PA4
// RESET PA8
void BluetoothUSART_Init(){
	//Inicijalizacija Tx i Rx
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
	
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	
	USART_Init(USART2, &USART_InitStructure);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART2, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
	//Inicijalizacija kontrolnih pinova
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_8;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);
//	GPIO_SetBits(GPIOA, GPIO_PinSource8);
//	GPIO_ResetBits(GPIOA, GPIO_PinSource5);
//	
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);
}
//Inicijalizacija WiFly USART-a
void WiFlyUSART_Init(){
	//Inicijalizacija Tx i Rx
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_USART3);
	
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	
	USART_Init(USART3, &USART_InitStructure);
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART3, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );

}


//Salji na bluetooth USART string sa \n
void USART_putsn(USART_TypeDef* USARTx, char* Data){
	while(*Data){
		while( !(USARTx->SR & 0x00000040) );
		USART_SendData(USARTx, *Data);
		*Data++;
   }
	while( !(USARTx->SR & 0x00000040) );
	USART_SendData(USARTx, '\r');
	while( !(USARTx->SR & 0x00000040) );
	USART_SendData(USARTx, '\n');		 
}

//Salji na bluetooth USART string bez \n
void USART_puts(USART_TypeDef* USARTx, char* Data){
	while(*Data){
		while( !(USARTx->SR & 0x00000040) );
		USART_SendData(USARTx, *Data);
		*Data++;
   }
 
}
/** Funkcija za dohvacanje podatka sa senzora **/
int getSensorData() {
	
		return current_value;

}

void getTime() {
	char bufer[9];
	RTC_GetTime(RTC_Format_BCD, &RTC_TimeStructure);
	current_time[0]  = (((uint8_t)(RTC_TimeStructure.RTC_Hours & 0xF0) >> 0x04) + 0x30); //Pretvorba iz BCD brojeva u ASCII kod
	current_time[1]  = (((uint8_t)(RTC_TimeStructure.RTC_Hours & 0x0F))+ 0x30);
	current_time[2]  = 0x74;
  
	current_time[3]  = (((uint8_t)(RTC_TimeStructure.RTC_Minutes & 0xF0) >> 0x04) + 0x30);
	current_time[4]  =(((uint8_t)(RTC_TimeStructure.RTC_Minutes & 0x0F))+ (uint8_t)0x30);
	current_time[5]  = 0x74;

	current_time[6]  = (((uint8_t)(RTC_TimeStructure.RTC_Seconds & 0xF0) >> 0x04)+ 0x30);
	current_time[7]  = (((uint8_t)(RTC_TimeStructure.RTC_Seconds & 0x0F)) + 0x30);
	current_time[8]  = '\0';
}

//Salji vrijeme i datum na USART
void USART_puts_time(USART_TypeDef* USARTx){
	uint8_t bufer[9];
	USART_putsn(USARTx, "\nVrijeme:");
	RTC_GetTime(RTC_Format_BCD, &RTC_TimeStructure);
	bufer[0]  = (((uint8_t)(RTC_TimeStructure.RTC_Hours & 0xF0) >> 0x04) + 0x30); //Pretvorba iz BCD brojeva u ASCII kod
	bufer[1]  = (((uint8_t)(RTC_TimeStructure.RTC_Hours & 0x0F))+ 0x30);
	bufer[2]  = 0x3A;
  
	bufer[3]  = (((uint8_t)(RTC_TimeStructure.RTC_Minutes & 0xF0) >> 0x04) + 0x30);
	bufer[4]  =(((uint8_t)(RTC_TimeStructure.RTC_Minutes & 0x0F))+ (uint8_t)0x30);
	bufer[5]  = 0x3A;

	bufer[6]  = (((uint8_t)(RTC_TimeStructure.RTC_Seconds & 0xF0) >> 0x04)+ 0x30);
	bufer[7]  = (((uint8_t)(RTC_TimeStructure.RTC_Seconds & 0x0F)) + 0x30);
	bufer[8]  = '\0';
	USART_putsn(USART2,(char*) bufer);
			
	USART_putsn(USARTx, "\nDatum (BCD):");
	RTC_GetDate(RTC_Format_BCD, &RTC_DateStructure);
	bufer[0]  = (((uint8_t)(RTC_DateStructure.RTC_Date & 0xF0) >> 0x04) + 0x30);
	bufer[1]  = (((uint8_t)(RTC_DateStructure.RTC_Date & 0x0F))+ 0x30);
	bufer[2]  = 0x2F;
  
	bufer[3]  = (((uint8_t)(RTC_DateStructure.RTC_Month & 0xF0) >> 0x04) + 0x30);
	bufer[4]  =(((uint8_t)(RTC_DateStructure.RTC_Month & 0x0F))+ (uint8_t)0x30);
	bufer[5]  = 0x2F;

	bufer[6]  = (((uint8_t)(RTC_DateStructure.RTC_Year & 0xF0) >> 0x04)+ 0x30);
	bufer[7]  = (((uint8_t)(RTC_DateStructure.RTC_Year & 0x0F)) + 0x30);
	bufer[8]  = '\0';
	USART_putsn(USARTx,(char*) bufer);	
}

//Updateanje vremena. 01 oznacava da saljem vrijeme. Format je: !01$hours:minutes:seconds#
void UpdateTime(USART_TypeDef* USARTx){
	uint8_t bufer[9];
	USART_puts(USARTx, "!01$");
	RTC_GetTime(RTC_Format_BCD, &RTC_TimeStructure);
	bufer[0]  = (((uint8_t)(RTC_TimeStructure.RTC_Hours & 0xF0) >> 0x04) + 0x30); //Pretvorba iz BCD brojeva u ASCII kod
	bufer[1]  = (((uint8_t)(RTC_TimeStructure.RTC_Hours & 0x0F))+ 0x30);
	bufer[2]  = 0x3A;
  
	bufer[3]  = (((uint8_t)(RTC_TimeStructure.RTC_Minutes & 0xF0) >> 0x04) + 0x30);
	bufer[4]  =(((uint8_t)(RTC_TimeStructure.RTC_Minutes & 0x0F))+ (uint8_t)0x30);
	bufer[5]  = 0x3A;

	bufer[6]  = (((uint8_t)(RTC_TimeStructure.RTC_Seconds & 0xF0) >> 0x04)+ 0x30);
	bufer[7]  = (((uint8_t)(RTC_TimeStructure.RTC_Seconds & 0x0F)) + 0x30);
	bufer[8]  = '\0';
	USART_puts(USART2,(char*) bufer);
	USART_putsn(USARTx, "#");
}

//Slanje logga na server. Format je: !02$hours:minutes:seconds$day/month/year&pwm_intensity&relay_state#
void SendLog(USART_TypeDef* USARTx){
	uint8_t bufer[30];
	bufer[0] = '!';
	bufer[1] = '0';
	bufer[2] = '2';
	bufer[3]  = '$'; //separator
	RTC_GetTime(RTC_Format_BCD, &RTC_TimeStructure);
	bufer[4]  = (((uint8_t)(RTC_TimeStructure.RTC_Hours & 0xF0) >> 0x04) + 0x30); //Pretvorba iz BCD brojeva u ASCII kod
	bufer[5]  = (((uint8_t)(RTC_TimeStructure.RTC_Hours & 0x0F))+ 0x30);
	bufer[6]  = 0x3A;
  
	bufer[7]  = (((uint8_t)(RTC_TimeStructure.RTC_Minutes & 0xF0) >> 0x04) + 0x30);
	bufer[8]  =(((uint8_t)(RTC_TimeStructure.RTC_Minutes & 0x0F))+ (uint8_t)0x30);
	bufer[9]  = 0x3A;
	
	bufer[10]  = (((uint8_t)(RTC_TimeStructure.RTC_Seconds & 0xF0) >> 0x04)+ 0x30);
	bufer[11]  = (((uint8_t)(RTC_TimeStructure.RTC_Seconds & 0x0F)) + 0x30);
	bufer[12]  = '$'; //separator
	RTC_GetDate(RTC_Format_BCD, &RTC_DateStructure);
	bufer[13]  = (((uint8_t)(RTC_DateStructure.RTC_Date & 0xF0) >> 0x04) + 0x30);
	bufer[14]  = (((uint8_t)(RTC_DateStructure.RTC_Date & 0x0F))+ 0x30);
	bufer[15]  = 0x2F;
  
	bufer[16]  = (((uint8_t)(RTC_DateStructure.RTC_Month & 0xF0) >> 0x04) + 0x30);
	bufer[17]  =(((uint8_t)(RTC_DateStructure.RTC_Month & 0x0F))+ (uint8_t)0x30);
	bufer[18]  = 0x2F;

	bufer[19]  = (((uint8_t)(RTC_DateStructure.RTC_Year & 0xF0) >> 0x04)+ 0x30);
	bufer[20]  = (((uint8_t)(RTC_DateStructure.RTC_Year & 0x0F)) + 0x30);
	bufer[21]  = '$'; //separator
	bufer[22] = ((char)pwmIntensity/100)+'0';
	bufer[23] = (((char)pwmIntensity%100)/10)+'0';
	bufer[24] = ((char)pwmIntensity%10)+'0';
	bufer[25]  = '$'; //separator
	bufer[26] = (uint8_t)relayState+'0';
	bufer[27] = '#';
	bufer[28] = '\0';
	USART_putsn(USARTx,(char*) bufer);	

}