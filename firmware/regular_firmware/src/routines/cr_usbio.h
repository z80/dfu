
#ifndef __CR_USBIO_H_
#define __CR_USBIO_H_

#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "croutine.h"
#include "queue.h"

xQueueHandle toMcu( void );
xQueueHandle fromMcu( void );
uint8_t     * buffer( void );

void crUsbIo( xCoRoutineHandle xHandle, 
              unsigned portBASE_TYPE uxIndex );


#endif


