
#ifndef __CR_I2C_H_
#define __CR_I2C_H_

#include "config.h"
#include "FreeRTOS.h"
#include "task.h"
#include "croutine.h"
#include "queue.h"

xQueueHandle i2cSendQueue();
xQueueHandle i2cReceiveQueue();
uint8_t      i2cStatus();

void crI2c( xCoRoutineHandle xHandle, 
            unsigned portBASE_TYPE uxIndex );

#endif


