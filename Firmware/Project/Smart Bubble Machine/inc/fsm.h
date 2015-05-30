#ifndef __FSM_H
#define __FSM_H

#include <stdio.h>
#include <stdlib.h>
#include "taskovi.h"
#include "functions.h"

typedef enum {
	sync,
	stage1,
	stage2,
	pwm,
	relay,
	clock,
	date,
	file
} state;

typedef enum {
	name,
	data
} file_state;

extern file_state file_faza;
extern state faza;

extern char cnt;	
extern char fsm_buffer[30];

extern char file_name[30];
extern char file_data[2048];
extern int file_cnt;
extern int file_size;

extern char clock_buffer[9]; //u ova dva buffer-a spremam clock i date value, a u queue saljem adrese njihove
extern char date_buffer[9];

///////////////////////////////////////

extern file_state file_faza2;
extern state faza2;

extern char cnt2;	
extern char fsm_buffer2[30];

extern char file_name2[30];
extern char file_data2[2048];
extern int file_cnt2;
extern int file_size2;

extern char clock_buffer2[9]; 
extern char date_buffer2[9];



void bluetooth_fsm(char znak);
void wifly_fsm(char znak);

#endif