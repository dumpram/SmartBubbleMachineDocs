#include "fsm.h"

file_state file_faza;
state faza;

char cnt;	
char fsm_buffer[30];

char file_name[30];
char file_data[2048];
int file_cnt;
int file_size;

char clock_buffer[9]; 
char date_buffer[9];


//fsm funkcija za bluetooth USART
void bluetooth_fsm(char znak){
	char condition;
	char PWM_value;
	char relay_value;
	
	char* clk_ptr=clock_buffer;
	char* date_ptr=date_buffer;

	
	switch (faza){
		case sync:
			if(znak=='!'){
				faza=stage1;
				USART_putsn(USART2, "Sync"); //
				cnt=0;}
			if(znak!='!')faza=sync;
			break;
		
		case stage1:
			if((znak>='0' && znak<='9')||(znak>='A' && znak<='F')){
				fsm_buffer[cnt]=znak;
				USART_putsn(USART2, "stage1"); //
				cnt++;}
			if(znak=='$'){
				fsm_buffer[cnt]='\0';
				condition=(char)atoi(fsm_buffer);
				cnt=0;
				switch (condition){
					case 01: //podatak je stanje releja
						faza=relay;
						USART_putsn(USART2, "slijedi relay"); //
						break;
					case 02: //podatak je PWM intenzitet
						faza=pwm;
						USART_putsn(USART2, "slijedi pwm"); //
						break;
					case 03: //Narihtavanje cukice
						faza=clock;
						USART_putsn(USART2, "slijedi clock setting");
						break;
					case 04: //narihtavanje datuma
						faza=date;
					  USART_putsn(USART2, "slijedi date setting");
						break;
					case 05:     //
						faza = file;
						file_faza = name;
						file_cnt=0;
						USART_putsn(USART2, "slijedi file");
						break;
					default:
						faza=sync;
				}
			}
			break;
			//	!02$99# !02$85# !02$0&9#  !01$1# !01$a# !02$059# !04$12/12/13# !03$12:37:30# !05$Test.txt$Ovo je fajl\r\nZapis 1\r\nZapis 2#
			//   !05$BullHit.txt$Ovo je fajl\r\nZapis 1   jjdssaassdjdsfjdkjsfksjflsdfjdlskjflksdjhklshfklsjfdkgkljkjghls#
		case pwm:
			if((znak>='0' && znak<='9')||(znak>='A' && znak<='F')){
				fsm_buffer[cnt]=znak;
				cnt++;
			}
			else if(znak=='#'){
				fsm_buffer[cnt]='\0';
				PWM_value=(char)atoi(fsm_buffer);
				xQueueSend(pwmQueue,&PWM_value,100);
				USART_putsn(USART2,"pwmQueue obavjesten"); //
				faza=sync;				
			}
			else{
				faza=sync;
				USART_putsn(USART2,"pogreska u pwm nizu"); //
			}
			break;
			
		case relay:
			if(znak!='0' && znak!='1'){
				USART_putsn(USART2,"pogresna relay poruka");
				faza=sync;}
			else{
				relay_value=znak-'0';
				xQueueSend(relayQueue,&relay_value,100);
				USART_putsn(USART2,"relayQueue obavjesten"); //
				faza=sync;
			}
			break;
			
		case clock:
			if((znak>='0' && znak<='9')|| znak == ':'){
				clock_buffer[cnt]=znak;
				cnt++;
			}
			else if(znak=='#'){
				clock_buffer[cnt]='\0'; //ne treba
				xSemaphoreGive(clockSemaphore);
				USART_putsn(USART2,"clockQueue obavjesten"); //
				faza=sync;				
			}
			else{
				faza=sync;
				USART_putsn(USART2,"pogreska u clock nizu"); //
			}
			break;
			
		case date:
			if((znak>='0' && znak<='9')|| znak == '/'){
				date_buffer[cnt]=znak;
				cnt++;
			}
			else if(znak=='#'){
				date_buffer[cnt]='\0'; //ne treba
				xSemaphoreGive(dateSemaphore);
				USART_putsn(USART2,"dateQueue obavjesten"); //
				faza=sync;				
			}
			else{
				faza=sync;
				USART_putsn(USART2,"pogreska u date nizu"); //
			}
			break;
		
		case file:     ///
			switch (file_faza){
				case name:
					if((znak >='a' && znak <='z')||(znak >='A' && znak <='Z')||(znak >='0' && znak <='9')||znak == '.'){
						file_name[file_cnt] = znak;
						file_cnt++;
					}
					else if(znak == '$'){
						file_name[file_cnt] = '\0';
						USART_putsn(USART2, "Gotovo ime fajla");
						USART_putsn(USART2, file_name);
						file_faza = data;
						file_cnt = 0;
					}
					else{
						faza = sync;
						file_faza = name;
						file_cnt = 0;
						USART_putsn(USART2, "Neispravan znak");
					}
					break;
				
				case data:
					file_data[file_cnt] = znak;
					file_cnt++;
					if(znak == '#'){
						file_faza = name;
						file_size = file_cnt-1;
						faza = sync;
						USART_putsn(USART2, "Fajl dejta");
						USART_putsn(USART2, file_data); //////Poaalji i counter da bi znali kolko znakova smo upisali od max 2048
						xSemaphoreGive(SDSemaphore);
						USART_putsn(USART2, "SDSemaphore obavjesten");
					}
					break;
			}
			break;
		default:
			faza=sync;
		}		
}


file_state file_faza2;
state faza2;

char cnt2;	
char fsm_buffer2[30];

char file_name2[30];
char file_data2[2048];
int file_cnt2;
int file_size2;

char clock_buffer2[9]; 
char date_buffer2[9];

////////////////////////////////////////////////////////////////////

//fsm funkcija za wifly USART
void wifly_fsm(char znak){
	char condition;
	char PWM_value;
	char relay_value;
	
	char* clk_ptr=clock_buffer2;
	char* date_ptr=date_buffer2;

	//USART_puts(USART2, &znak);
	switch (faza2){
		case sync:
			if(znak=='!'){
				faza2=stage1;
		//		USART_putsn(USART2, "Sync"); //
				cnt2=0;}
			if(znak!='!')faza2=sync;
			break;
		
		case stage1:
			if((znak>='0' && znak<='9')||(znak>='A' && znak<='F')){
				fsm_buffer2[cnt2]=znak;
			//	USART_putsn(USART2, "stage1"); //
				cnt2++;}
			if(znak=='$'){
				fsm_buffer2[cnt2]='\0';
				condition=(char)atoi(fsm_buffer2);
				cnt2=0;
				switch (condition){
					case 01: //podatak je stanje releja
						faza2=relay;
				//		USART_putsn(USART2, "slijedi relay"); //
						break;
					case 02: //podatak je PWM intenzitet
						faza2=pwm;
					//	USART_putsn(USART2, "slijedi pwm"); //
						break;
					case 03: //Narihtavanje cukice
						faza2=clock;
						//USART_putsn(USART2, "slijedi clock setting");
						break;
					case 04: //narihtavanje datuma
						faza2=date;
					  //USART_putsn(USART2, "slijedi date setting");
						break;
					case 05:     //
						faza2 = file;
						file_faza2 = name;
						file_cnt2=0;
						//USART_putsn(USART2, "slijedi file");
						break;
					default:
						faza2=sync;
				}
			}
			break;
			//	!02$99# !02$85# !02$0&9#  !01$1# !01$a# !02$059# !04$12/12/13# !03$12:37:30# !05$Test.txt$Ovo je fajl\r\nZapis 1\r\nZapis 2#
			//   !05$BullHit.txt$Ovo je fajl\r\nZapis 1   jjdssaassdjdsfjdkjsfksjflsdfjdlskjflksdjhklshfklsjfdkgkljkjghls#
		case pwm:
			if((znak>='0' && znak<='9')||(znak>='A' && znak<='F')){
				fsm_buffer2[cnt2]=znak;
				cnt2++;
			}
			else if(znak=='#'){
				fsm_buffer2[cnt2]='\0';
				PWM_value=(char)atoi(fsm_buffer2);
				xQueueSend(pwmQueue,&PWM_value,100);
				//USART_putsn(USART2,"pwmQueue obavjesten"); //
				faza2=sync;				
			}
			else{
				faza2=sync;
				//USART_putsn(USART2,"pogreska u pwm nizu"); //
			}
			break;
			
		case relay:
			if(znak!='0' && znak!='1'){
				//USART_putsn(USART2,"pogresna relay poruka");
				faza2=sync;}
			else{
				relay_value=znak-'0';
				xQueueSend(relayQueue,&relay_value,100);
				//USART_putsn(USART2,"relayQueue obavjesten"); //
				faza2=sync;
			}
			break;
			
		case clock:
			if((znak>='0' && znak<='9')|| znak == ':'){
				clock_buffer2[cnt2]=znak;
				cnt2++;
			}
			else if(znak=='#'){
				clock_buffer2[cnt2]='\0'; //ne treba
				xSemaphoreGive(clockSemaphore);
				//USART_putsn(USART2,"clockQueue obavjesten"); //
				faza2=sync;				
			}
			else{
				faza2=sync;
				//USART_putsn(USART2,"pogreska u clock nizu"); //
			}
			break;
			
		case date:
			if((znak>='0' && znak<='9')|| znak == '/'){
				date_buffer2[cnt2]=znak;
				cnt2++;
			}
			else if(znak=='#'){
				date_buffer2[cnt2]='\0'; //ne treba
				xSemaphoreGive(dateSemaphore);
				//USART_putsn(USART2,"dateQueue obavjesten"); //
				faza2=sync;				
			}
			else{
				faza2=sync;
				//USART_putsn(USART2,"pogreska u date nizu"); //
			}
			break;
		
		case file:     ///
			switch (file_faza2){
				case name:
					if((znak >='a' && znak <='z')||(znak >='A' && znak <='Z')||(znak >='0' && znak <='9')||znak == '.'){
						file_name2[file_cnt2] = znak;
						file_cnt2++;
					}
					else if(znak == '$'){
						file_name2[file_cnt2] = '\0';
						USART_putsn(USART2, "Gotovo ime fajla");
						USART_putsn(USART2, file_name2);
						file_faza2 = data;
						file_cnt2 = 0;
					}
					else{
						faza2 = sync;
						file_faza2 = name;
						file_cnt2 = 0;
						USART_putsn(USART2, "Neispravan znak");
					}
					break;
				
				case data:
					file_data2[file_cnt2] = znak;
					file_cnt2++;
					if(znak == '#'){
						file_faza2 = name;
						file_size2 = file_cnt2-1;
						faza2 = sync;
						USART_putsn(USART2, "Fajl dejta");
						USART_putsn(USART2, file_data2); //////Poaalji i counter da bi znali kolko znakova smo upisali od max 2048
						xSemaphoreGive(SDSemaphore);
						USART_putsn(USART2, "SDSemaphore obavjesten");
					}
					break;
			}
			break;
		default:
			faza2=sync;
		}		
}