#ifndef I2C_H
#define I2C_H

#include <stm32f4xx.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_i2c.h>

#define AD7147GG 0x2C /* Address of AD7147 with two LS bits tied to GND */
//#define STAGE0_CONNECTION_60	0x3FFE		// 0011 1111 1111 1110  
//#define STAGE0_CONNECTION_127	0x1FFF   	// 0001 1111 1111 1111
#define FULL_CONTAINER		39600
#define	EMPTY_CONTAINER		100
#define EMPTY_CONTAINER_MESSAGE 	"No more detergent\r\n"
#define FULL_CONTAINER_MESSAGE		"Container is full\r\n"
#define	CONTAINER_MESSAGE					"Container is at %d % \r\n"
#define CONTAINER_VALUE						(1-((float)(registerValue-FULL_CONTAINER)/(float)(EMPTY_CONTAINER - FULL_CONTAINER)))*100


extern int current_value;
void I2C1_Init(void);
void I2C_Start(I2C_TypeDef* I2Cx, uint8_t address, uint8_t direction);
void I2C_Write(I2C_TypeDef* I2Cx, uint8_t data);
uint8_t I2C_Read_Ack(I2C_TypeDef* I2Cx);
uint8_t I2C_Read_NAck(I2C_TypeDef* I2Cx);
void I2C_stop(I2C_TypeDef* I2Cx);
unsigned short int GetReg(unsigned short int Addr);
void SetReg (unsigned short int Addr, unsigned short int Data);
#endif