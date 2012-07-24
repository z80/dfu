
#ifndef __CR_PAWN_H_
#define __CR_PAWN_H_

#include "stm32f10x.h"
#include "config.h"
#include "FreeRTOS.h"
#include "task.h"
#include "croutine.h"
#include "queue.h"

uint8_t invokePawn( void );
void crPawn( xCoRoutineHandle xHandle,
             unsigned portBASE_TYPE uxIndex );

#endif





