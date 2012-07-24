
#include "cr_pawn.h"
//#include "pawn_loader.h"
#include "config.h"


uint8_t invokePawn( void )
{
    RCC_APB2PeriphClockCmd( PIN_CLK, ENABLE );
    GPIO_InitTypeDef s;
    s.GPIO_Pin   = PIN_FLASH_MODE | PIN_OP_MODE;
    s.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    s.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init( PIN_PORT, &s );
    if ( GPIO_ReadInputDataBit( PIN_PORT, PIN_OP_MODE ) )
        return 0;
    return 1;
}


void crPawn( xCoRoutineHandle xHandle,
             unsigned portBASE_TYPE uxIndex )
{

}

