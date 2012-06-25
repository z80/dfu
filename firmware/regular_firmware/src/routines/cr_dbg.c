
#include "cr_dbg.h"

#define RED_PIN      GPIO_Pin_10
#define GREEN_PIN    GPIO_Pin_11
#define RCC_LED_PORT RCC_APB2Periph_GPIOB
#define LED_PORT     GPIOB

uint8_t g_initialized = 0;

static void init( void )
{
    RCC_APB2PeriphClockCmd( RCC_LED_PORT, ENABLE );
    GPIO_InitTypeDef GPIO_InitStructure;
  
    GPIO_InitStructure.GPIO_Pin = GREEN_PIN | RED_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( LED_PORT, &GPIO_InitStructure );
}

void setRed( uint8_t val )
{
    GPIO_WriteBit( LED_PORT, RED_PIN, (val) ? Bit_SET : Bit_RESET );
}

uint8_t red( void )
{
    uint8_t res = GPIO_ReadOutputDataBit( LED_PORT, RED_PIN );
    return res;
}

void setGreen( uint8_t val )
{
    GPIO_WriteBit( LED_PORT, GREEN_PIN, (val) ? Bit_SET : Bit_RESET );
}

uint8_t green( void )
{
    uint8_t res = GPIO_ReadOutputDataBit( LED_PORT, GREEN_PIN );
    return res;
}


void crDbg( xCoRoutineHandle xHandle, 
            unsigned portBASE_TYPE uxIndex )
{
    if ( !g_initialized )
    {
        init();
        g_initialized = 255;
    }
    crSTART( xHandle );
    for ( ;; )
    {
        //setRed( 0 );
        setGreen( 1 );
        crDELAY( xHandle, 25 );

        setGreen( 0 );
        //setRed( 1 );
        crDELAY( xHandle, 25 );
    }
    crEND();
}


