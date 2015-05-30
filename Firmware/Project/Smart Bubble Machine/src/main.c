/**
  ******************************************************************************
  * @file    main.c 
  * @author  Ivan Spasic
  * @version V1.0.0
  * @date    22-01-2014
  * @brief   Main program body
  ******************************************************************************/

/* Includes ********************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#include "stm32f4_discovery_sdio_sd.h"
#include "ff.h"
#include "functions.h"
#include "rtc.h"
#include "fsm.h"
#include "i2c.h"

#include "taskovi.h"


static void fault_err (FRESULT rc);
static void Delay(__IO uint32_t nCount);

xQueueHandle message = NULL;	  
xQueueHandle pwmQueue = NULL;
xQueueHandle relayQueue = NULL;


xSemaphoreHandle clockSemaphore = NULL;
xSemaphoreHandle dateSemaphore = NULL;
xSemaphoreHandle SDSemaphore = NULL;



//Pritiskom na user_button zapisi fajl na SD karticu + na terminal ispisi vrijeme i datum (debugging).
//Posto SD kartica nije u funkciji nema potrebe za njim
void SD_Task(){
	SD_Error Status = SD_OK;
	FATFS filesystem;		/* volume lable */
	FRESULT ret;			  /* Result code */
	FIL file;				    /* File object */
	DIR dir;				    /* Directory object */
	FILINFO fno;			  /* File information object */
	UINT bw, br;
	uint8_t buff[128];
	char help;
	
	xSemaphoreTake(SDSemaphore, 0);  ////
	////////////////////////////////
	if (f_mount(0, &filesystem) == FR_OK) {
		STM_EVAL_LEDToggle(LED6);
  }
  Delay(50);	
////////////////////////////////	
	while(1){
		if(STM_EVAL_PBGetState(BUTTON_USER)){
			STM_EVAL_LEDOn(LED6);
			ret = f_open(&file, "Goodbye.TXT", FA_WRITE | FA_CREATE_ALWAYS);
			if (ret) {
				USART_putsn(USART2, "Greska pri otvaranju");
				fault_err(ret);
			} 
			else {
				USART_putsn(USART2, "Mirko");
				ret = f_write(&file, "Goodbye cruel world", sizeof("Goodbye cruel world"), &bw);
				Delay(100);
				ret = f_close(&file);
				if (ret) {
				USART_putsn(USART2, "Close the file error");
				}
			}
			STM_EVAL_LEDOff(LED6);
			Delay(100);

			USART_puts_time(USART2); /////

			if(xSemaphoreTake(SDSemaphore, 100) == pdTRUE){
				ret = f_open(&file, file_name, FA_WRITE | FA_CREATE_ALWAYS);
				if(ret){
					fault_err(ret);
				}
				else{
					USART_putsn(USART2, "Pisem na karticu");
					USART_putsn(USART2, file_data);
					f_write(&file, file_data, file_size, &bw);
					ret = f_close(&file);
					if (ret) {
					USART_putsn(USART2, "Close the file error");

					}
				}
			}
			STM_EVAL_LEDOff(LED6);
		}
	}
}




//Parsira podatke iz commandQueue-a, koji su dosli sa Bluetooth USART-a.
void Parser_Task(){
	char znak, znak2;
	portBASE_TYPE xStatus;
	while(1){
		xStatus=xQueueReceive(commandQueue, &znak, 50);
		if(xStatus==pdTRUE){
			bluetooth_fsm(znak);
		}
	}
}

//Parsira podatke iz WiFly_commandQueue-a, koji su dosli sa WiFly USART-a.
void WiFly_Parser_Task(){
	char znak, znak2;
	portBASE_TYPE xStatus;
	while(1){
		xStatus=xQueueReceive(WiFly_commandQueue, &znak2, 50);
		if(xStatus==pdTRUE){
			wifly_fsm(znak2);
		}
	}
}

//Task za PWM funkcionalnost. Provjerava pwmQueue.
void PWM_Task(){
	uint32_t pwm;
	char buff[4];
	PWM_Init();
	while(1){
		if(xQueueReceive(pwmQueue, &pwm, 100)){
		//pwm upisi u compare registar !!!!!!!!!!!!!!!!!!!!!!!
		USART_putsn(USART2, "Narihtavam USART");
		buff[0]=((char)pwm/100)+'0';
		buff[1]=(((char)pwm%100)/10)+'0';
		buff[2]=((char)pwm%10)+'0';
		buff[3]='\0';
		set_PWM((uint8_t)pwm); 
		USART_putsn(USART2, buff);
		pwmIntensity = pwm;
		vTaskDelay(100);
		}
	}
}

//Task za relay funkcionalnost. Provjerava relayQueue.
void Relay_Task(){
	uint8_t relay;
	Relay_Init();
	while(1){
		if(xQueueReceive(relayQueue, &relay, 100)){
			if(relay==0){
				Relay_Off();
				USART_putsn(USART2,"gasim AC motor");
				relayState = 0;
			}
			if(relay==1){
				Relay_On();
				USART_putsn(USART2,"palim AC motor");
				relayState = 1;
			}
		}
		vTaskDelay(100);
	}
}

//Task koji inicijalizira RTC na mikrokontroleru, te ovisno o tome da li postoje zahtjevi 
//mijenja trenutni datum i vrijeme RTC-a
void RTC_Task(){
	xSemaphoreTake(clockSemaphore, 100); //Zanimljivo ???
	xSemaphoreTake(dateSemaphore, 100); //Zanimljivo ??? 
	if (RTC_ReadBackupRegister(RTC_BKP_DR0) != 0x32F2){
    /* RTC configuration  */
    RTC_Config();

    /* Configure the time&date register */
    RTC_TimeRegulate(); 
  }
	else{
		/* Enable the PWR clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
		/* Allow access to RTC */
    PWR_BackupAccessCmd(ENABLE);
		RCC_RTCCLKCmd(ENABLE);
		/* Wait for RTC APB registers synchronisation */
    RTC_WaitForSynchro();
	}
	while(1){
		if(xSemaphoreTake(clockSemaphore, 100) == pdTRUE){
		USART_putsn(USART2, "Narihtavam sat");
		USART_putsn(USART2, clock_buffer);
		RTC_setTime(clock_buffer);
		}	
		if(xSemaphoreTake(dateSemaphore, 100) == pdTRUE){
		USART_putsn(USART2, "Narihtavam datum");
		USART_putsn(USART2, date_buffer);
		RTC_setDate(date_buffer);
		}		
	}
}


//Inicijalizacija WiFly modula i komunikacija sa serverom preko WiFly-a
void WiFi_Task(){
	char buffer[16];
	char buffer2[16];
	/** Inicijalizacija WiFly modula **/
	char a[1024];
	Delay(100);
	USART_puts(USART3, "$$$");
	Delay(100);
//	USART_puts(USART3, "reboot\r");
//	Delay(500);
//	USART_puts(USART3, "$$$");
//	Delay(100);
	USART_puts(USART3, "join\r");
	//USART_SendData(USART3, '\r');
	Delay(1000);
	USART_puts(USART3, "open\r");
	//USART_SendData(USART3, '\r');
	Delay(500);
	xQueueReceive(WiFly_commandQueue, &a, 100); //prazni queue jer WiFly vraca na USART
	/** Kraj inicijalizacije **/
	
	while(1){
		getTime();
		sprintf(buffer, "%di%s", getSensorData(), current_time);		
		USART_puts(USART3, buffer);
		sprintf(buffer2, "!%s#", buffer);
		USART_puts(USART2, buffer2);
		vTaskDelay(700);
//		wifly_fsm();    
	}
}

/** Task za ocitavanje razine sapunice **/
void sensorTask() {
	

		uint16_t registerValue;
		int i;
		I2C1_Init();
	for (i = 0x80; i < 0xDF; i++) {
			SetReg(i, 0x55);
	}
	// Power-up seq //
	SetReg(0, 0x82B2); // 0b 1000 0010 1011 0010
	SetReg(1, 0x0000);
	SetReg(2, 0x3230);
	SetReg(3, 0x0832);
	SetReg(4, 0x0000);
	SetReg(5, 0x0000);
	SetReg(6, 0x0000);
	SetReg(7, 0x0000);
	SetReg(1, 0x0FFF);	

	while (1)
	{
		registerValue = GetReg(0xE0);
		current_value = (int) CONTAINER_VALUE;
	}
}

int main(void)
{ 
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 ); 
	//Inicijalizacija gumbeka i LED-ica
	STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_GPIO);
	STM_EVAL_LEDInit(LED3);
	STM_EVAL_LEDInit(LED4);
	STM_EVAL_LEDInit(LED5);
	STM_EVAL_LEDInit(LED6);
	BluetoothUSART_Init();
	WiFlyUSART_Init();

	/* Inicijalizacija taskova */		

	//xTaskCreate( SD_Task, (const signed char*)"SD", 512, NULL, 1, NULL );
  xTaskCreate( RTC_Task, (const signed char*)"RTC", 512, NULL, 1, NULL);
	xTaskCreate( PWM_Task, (const signed char*)"PWM", 512, NULL, 1, NULL );
	xTaskCreate( Relay_Task, (const signed char*)"Relay", 512, NULL, 1, NULL );
	xTaskCreate( WiFi_Task, (const signed char*)"WiFly", 512, NULL, 1, NULL );
	xTaskCreate( Parser_Task, (const signed char*)"Parser", 512, NULL, 3, NULL ); //Trebao bi imati veci priority
	//xTaskCreate( sensorTask, (const signed char*)"I2C sensor", 512, NULL, 1, NULL );
	xTaskCreate( WiFly_Parser_Task, (const signed char*)"WiFly Parser", 512, NULL, 3, NULL ); //Trebao bi imati veci priority
	
	//Queue-ovi za komunikaciju izmedju taskova
	commandQueue = xQueueCreate(2048, sizeof(uint8_t)); // queue za pohranu podataka sa serije
	WiFly_commandQueue = xQueueCreate(20, sizeof(uint8_t));
	pwmQueue = xQueueCreate(1, sizeof(char));
	relayQueue = xQueueCreate(1, sizeof(char));
	message=xQueueCreate( 1, sizeof( char ) );

	//Semafori za komunikaciju izmedju taskove
	vSemaphoreCreateBinary(clockSemaphore);
	vSemaphoreCreateBinary(dateSemaphore); //
	vSemaphoreCreateBinary(SDSemaphore); //Informira da se moze procitati iz SD buffera
	
	vTaskStartScheduler();
	
	while(1){};
	
}


//Skopirano iz gotovog FATFS primjera, ak dodje do error-a u citanju, pisanju po kartici task se baca u ovu fju
//koja ga vrti u beskonacnoj petlji ne radeci nista
static void fault_err (FRESULT rc)
{
  const char *str =
                    "OK\0" "DISK_ERR\0" "INT_ERR\0" "NOT_READY\0" "NO_FILE\0" "NO_PATH\0"
                    "INVALID_NAME\0" "DENIED\0" "EXIST\0" "INVALID_OBJECT\0" "WRITE_PROTECTED\0"
                    "INVALID_DRIVE\0" "NOT_ENABLED\0" "NO_FILE_SYSTEM\0" "MKFS_ABORTED\0" "TIMEOUT\0"
                    "LOCKED\0" "NOT_ENOUGH_CORE\0" "TOO_MANY_OPEN_FILES\0";
  FRESULT i;
	char help;

  for (i = (FRESULT)0; i != rc && *str; i++) {
    while (*str++) ;
  }
	USART_puts(USART2, "Greska pri otvaranju: ");
	help=rc+'0';
	Delay(10);
	USART_putsn(USART2, &help);
  STM_EVAL_LEDOn(LED6);
}


//samo bezvezni delay
static void Delay(__IO uint32_t nCount)
{
  __IO uint32_t index = 0; 
  for (index = (100000 * nCount); index != 0; index--);
}
