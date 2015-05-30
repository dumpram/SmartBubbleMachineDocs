#ifndef __TASK_H
#define __TASK_H

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "croutine.h"
#include "semphr.h"

xQueueHandle message=NULL;
xQueueHandle controlQueue=NULL;
xSemaphoreHandle read_enable = NULL; //creat-a se u parser_tasku
 
#endif