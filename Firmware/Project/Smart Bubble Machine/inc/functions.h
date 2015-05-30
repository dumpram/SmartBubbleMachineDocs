#ifndef __FUNCTIONS_H
#define __FUNCTIONS_H

#include "stm32f4xx.h"
#include "rtc.h"

#define RELAY_PIN		GPIO_Pin_13 
#define RELAY_PORT	 GPIOD	
#define PWM_PIN		GPIO_Pin_14	
#define PWM_PORT		GPIOD	

extern uint32_t pwmIntensity;
extern uint8_t relayState;
extern char current_time[9];

void PWM_Init();
void set_PWM(uint8_t intesity);
void Relay_Init();
void Relay_On();
void Relay_Off();
void getTime();
int getSensorData();
void BluetoothUSART_Init();
void WiFlyUSART_Init();
void USART_putsn(USART_TypeDef* USARTx, char* Data);
void USART_puts(USART_TypeDef* USARTx, char* Data);
void USART_puts_time(USART_TypeDef* USARTx);
void UpdateTime(USART_TypeDef* USARTx);
void SendLog(USART_TypeDef* USARTx);
//void WiFi_Init(); //X
//void I2C_Init(); //X

#endif