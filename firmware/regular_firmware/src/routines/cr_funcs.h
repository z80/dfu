
#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "croutine.h"
#include "queue.h"

xQueueHandle invokeFunc( uint8_t funcId );

void crFuncs( xCoRoutineHandle xHandle, 
              unsigned portBASE_TYPE uxIndex );


