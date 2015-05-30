#include "i2c.h"

int current_value;

void I2C1_Init()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	I2C_InitTypeDef I2C_InitStruct;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_I2C1);
	
	I2C_InitStruct.I2C_ClockSpeed = 50000;
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStruct.I2C_OwnAddress1 = 0x00;
	I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_Init(I2C1, &I2C_InitStruct);
	
	I2C_Cmd(I2C1, ENABLE);
}

void I2C_Start(I2C_TypeDef* I2Cx, uint8_t address, uint8_t direction)
{
	while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY));
	I2C_GenerateSTART(I2Cx, ENABLE);
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(I2Cx, address, direction);
	if(direction == I2C_Direction_Transmitter)
	{
		while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	}
	if(direction == I2C_Direction_Receiver)
	{
		while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	}
}

void I2C_Write(I2C_TypeDef* I2Cx, uint8_t data)
{
	I2C_SendData(I2Cx, data);
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
}

uint8_t I2C_Read_Ack(I2C_TypeDef* I2Cx)
{
	uint8_t data;
	I2C_AcknowledgeConfig(I2Cx, ENABLE);
	while( !I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED) );
	data = I2C_ReceiveData(I2Cx);
	return data;
}


uint8_t I2C_Read_NAck(I2C_TypeDef* I2Cx)
{
	uint8_t data;
	I2C_AcknowledgeConfig(I2Cx, DISABLE);
	I2C_GenerateSTOP(I2Cx, ENABLE);
	while( !I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED) );
	data = I2C_ReceiveData(I2Cx);
	return data;
}

void I2C_stop(I2C_TypeDef* I2Cx)
{
	I2C_GenerateSTOP(I2Cx, ENABLE);
}

uint16_t GetReg(uint16_t Addr) // promijenjeno unsigned short int u uint16_t - isto je ali je ovako preglednije
{
		uint16_t Ret_Value = 0x0000;
		uint8_t NumByteToRead = 2;
		uint8_t writeAddr = 0x58;
		uint8_t readAddr = 0x59;
	
		
			/* Wait while I2C line is busy */
		while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
		
			/* Generate start condition */
		I2C_GenerateSTART(I2C1, ENABLE);	
	
			/* Test on EV5 and clear it */
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	
			/* Send address */
			/* 3 LSB
				 ADD0 --> 0
				 ADD1 --> 0
				 Write operation --> 0 */
		I2C_Send7bitAddress(I2C1, writeAddr ,I2C_Direction_Transmitter);
			
			/* Test on EV6 and clear it */
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	
			/* Clear EV6 by setting again the PE bit */
		I2C_Cmd(I2C1, ENABLE);
			
			/* Send register address (MSB)*/
		I2C_SendData(I2C1, Addr>>8);
		
			/* Test on EV8 and clear it */
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
		
			/* Send register address (LSB)*/
		I2C_SendData(I2C1, Addr);
		
			/* Test on EV8 and clear it */
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
		
			/* Send STRAT condition a second time */
		I2C_GenerateSTART(I2C1, ENABLE);
		
			/* Test on EV5 and clear it */
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
			
			
			/* Send address */
			/* 3 LSB
				 ADD0 --> 0
				 ADD1 --> 0
				 Read operation --> 1 */
		I2C_Send7bitAddress(I2C1, readAddr, I2C_Direction_Receiver);
		
			/* Test on EV6 and clear it */
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
		
			/* Test on EV7 and clear it */
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
// ack
			/* Read a upper byte from the AD7147 */
		Ret_Value |= I2C_ReceiveData(I2C1);
		
			/* Shift upper byte left to make place for lower byte */
		Ret_Value = Ret_Value<<8;
		
		//delay();
			/* Disable Acknowledgement */
		
			
		I2C_AcknowledgeConfig(I2C1, DISABLE);
 	
		/* Test on EV7 and clear it */
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
		
		I2C_AcknowledgeConfig(I2C1, DISABLE);
			
		/* Read a lower byte from the AD7147 */
   	Ret_Value |= I2C_ReceiveData(I2C1);
		
			/* Disable Acknowledgement */
		I2C_AcknowledgeConfig(I2C1, DISABLE);
		
			/* Generate STOP Condition */
		I2C_GenerateSTOP(I2C1, ENABLE);   
				
			/* Enable Acknowledgement to be ready for another reception */
		I2C_AcknowledgeConfig(I2C1, ENABLE);
		
		return Ret_Value;
}

void SetReg (unsigned short int Addr, unsigned short int Data)
{
		uint8_t writeAddr = 0x58;
	
			/* Wait while I2C line is busy */
		while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
		
			/* Generate start condition */
		I2C_GenerateSTART(I2C1, ENABLE);	
	
			/* Test on EV5 and clear it */
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	
		/* Send address */
			/* 3 LSB
				 ADD0 --> 0
				 ADD1 --> 0
				 Write operation --> 0 */
		I2C_Send7bitAddress(I2C1, writeAddr, I2C_Direction_Transmitter);
			
			/* Test on EV6 and clear it */
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	
			/* Clear EV6 by setting again the PE bit */
		I2C_Cmd(I2C1, ENABLE);
			
			/* Send register address (MSB)*/
		I2C_SendData(I2C1, Addr>>8);
		
			/* Test on EV8 and clear it */
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
		
			/* Send register address (LSB)*/
		I2C_SendData(I2C1, Addr);
		
			/* Test on EV8 and clear it */
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
				
			/* Send the data to write in (MSB)*/
		I2C_SendData(I2C1, Data>>8);
	
			/* Test on EV8 and clear it */
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
		
			/* Send the data to write in (LSB) */
		I2C_SendData(I2C1, Data);
	
			/* Test on EV8 and clear it */
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
			
			/* Generate STOP Condition */
		I2C_GenerateSTOP(I2C1, ENABLE);
}

