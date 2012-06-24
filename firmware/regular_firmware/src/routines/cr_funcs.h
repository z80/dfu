
#ifndef __CR_FUNCS_H_
#define __CR_FUNCS_H_

#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "croutine.h"
#include "queue.h"

void invokeFunc( uint8_t funcId );

void crFuncs( xCoRoutineHandle xHandle, 
              unsigned portBASE_TYPE uxIndex );


#endif


