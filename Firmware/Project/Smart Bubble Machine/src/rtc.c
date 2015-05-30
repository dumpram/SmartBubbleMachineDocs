#include "rtc.h"


RTC_TimeTypeDef RTC_TimeStampStructure;
RTC_DateTypeDef RTC_TimeStampDateStructure;
RTC_TimeTypeDef RTC_TimeStructure;
RTC_DateTypeDef RTC_DateStructure;



void RTC_Config(void){
	uint32_t uwAsynchPrediv = 0;
	uint32_t uwSynchPrediv = 0;
	RTC_InitTypeDef RTC_InitStructure;
	
	/* Enable the PWR clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	/* Allow access to RTC */
  PWR_BackupAccessCmd(ENABLE);
	
  /* Enable the LSE OSC */
  RCC_LSEConfig(RCC_LSE_ON);

  /* Wait till LSE is ready */  
  while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {
  }

  /* Select the RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
  /* ck_spre(1Hz) = RTCCLK(LSE) /(uwAsynchPrediv + 1)*(uwSynchPrediv + 1)*/
  uwSynchPrediv = 0xFF;
  uwAsynchPrediv = 0x7F;
	
	RTC_InitStructure.RTC_AsynchPrediv = uwAsynchPrediv;
  RTC_InitStructure.RTC_SynchPrediv = uwSynchPrediv;
  RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
	RTC_Init(&RTC_InitStructure);
	
	/* Enable the RTC Clock */
	RCC_RTCCLKCmd(ENABLE);
	/* Wait for RTC APB registers synchronisation */
	RTC_WaitForSynchro();
	
	RTC_WriteBackupRegister(RTC_BKP_DR0, 0x32F2);

}

void RTC_TimeRegulate(void){
  /* Set Time hh:mm:ss */
  RTC_TimeStructure.RTC_H12     = RTC_H12_AM;
  RTC_TimeStructure.RTC_Hours   = 0x08;  
  RTC_TimeStructure.RTC_Minutes = 0x10;
  RTC_TimeStructure.RTC_Seconds = 0x00;
  RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);

  /* Set Date Week/Date/Month/Year */
  RTC_DateStructure.RTC_WeekDay = 01;
  RTC_DateStructure.RTC_Date = 0x31;
  RTC_DateStructure.RTC_Month = 0x12;
  RTC_DateStructure.RTC_Year = 0x12;
  RTC_SetDate(RTC_Format_BCD, &RTC_DateStructure);
  
  /* Write BkUp DR0 */
  RTC_WriteBackupRegister(RTC_BKP_DR0, 0x32F2);
}

void RTC_setTime(char* time){
	RTC_TimeStructure.RTC_H12     = RTC_H12_AM;
  RTC_TimeStructure.RTC_Hours   = (((*(time)-48)<<4)&0xF0)|(((*(time+1)-48))&0x0F);  
  RTC_TimeStructure.RTC_Minutes = (((*(time+3)-48)<<4)&0xF0)|(((*(time+4)-48))&0x0F); 
  RTC_TimeStructure.RTC_Seconds = (((*(time+6)-48)<<4)&0xF0)|(((*(time+7)-48))&0x0F); 
  RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);
}

void RTC_setDate(char* date){
	RTC_DateStructure.RTC_WeekDay = 01;
  RTC_DateStructure.RTC_Date = (((*(date)-48)<<4)&0xF0)|(((*(date+1)-48))&0x0F);  
  RTC_DateStructure.RTC_Month = (((*(date+3)-48)<<4)&0xF0)|(((*(date+4)-48))&0x0F); 
  RTC_DateStructure.RTC_Year = (((*(date+6)-48)<<4)&0xF0)|(((*(date+7)-48))&0x0F); 
  RTC_SetDate(RTC_Format_BCD, &RTC_DateStructure);
}