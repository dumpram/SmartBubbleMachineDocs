#ifndef __RTC_H
#define __RTC_H
#include "stm32f4xx.h"

extern RTC_TimeTypeDef RTC_TimeStampStructure;
extern RTC_DateTypeDef RTC_TimeStampDateStructure;
extern RTC_TimeTypeDef RTC_TimeStructure;
extern RTC_DateTypeDef RTC_DateStructure;

void RTC_Config(void);
void RTC_TimeRegulate(void);
void RTC_setTime(char* time);
void RTC_setDate(char* date);

#endif