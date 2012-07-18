
#ifndef __CR_I2C_H_
#define __CR_I2C_H_

#include "stm32f10x.h"
#include "config.h"
#include "FreeRTOS.h"
#include "task.h"
#include "croutine.h"
#include "queue.h"

void crI2c( xCoRoutineHandle xHandle, 
            unsigned portBASE_TYPE uxIndex );

#endif


