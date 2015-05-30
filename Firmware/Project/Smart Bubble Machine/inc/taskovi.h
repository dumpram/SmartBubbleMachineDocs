#ifndef __TASKOVI_H
#define __TASKOVI_H

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "croutine.h"
#include "semphr.h"

extern xSemaphoreHandle clockSemaphore;
extern xSemaphoreHandle resetSemaphore;
extern xSemaphoreHandle dateSemaphore;
extern xSemaphoreHandle SDSemaphore;

extern xQueueHandle message;
extern xQueueHandle commandQueue;//queue s interrupt na parser task
extern xQueueHandle WiFly_commandQueue; //queue za WiFly parser
extern xQueueHandle pwmQueue;
extern xQueueHandle relayQueue;


#endif
