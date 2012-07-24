
#include "cr_dbg.h"
#include "gpio.h"

#define RED_PIN      GPIO_Pin_10
#define GREEN_PIN    GPIO_Pin_11
#define RCC_LED_PORT RCC_APB2Periph_GPIOB
#define LED_PORT     GPIOB

uint8_t g_initialized = 0;
uint8_t g_high = 0;
uint8_t g_low  = 0;
uint8_t g_pulses = 0;

void dbgSetLow( void )
{
	GPIO_WriteBit( LED_PORT, GREEN_PIN, Bit_RESET );
}

void dbgSetHigh( void )
{
	GPIO_WriteBit( LED_PORT, GREEN_PIN, Bit_SET );
}

void dbgSetPulses( uint8_t low, uint8_t high, uint32_t cnt )
{
	g_low    = low;
	g_high   = high;
	g_pulses = cnt;
}

void dbgStop( void )
{
	g_pulses = 0;
}



static void init( void )
{
    RCC_APB2PeriphClockCmd( RCC_LED_PORT, ENABLE );
    GPIO_InitTypeDef GPIO_InitStructure;
  
    GPIO_InitStructure.GPIO_Pin = GREEN_PIN/* | RED_PIN*/;
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
    crSTART( xHandle );
    for ( ;; )
    {
        if ( !g_initialized )
        {
            init();
            g_initialized = 255;
        }
        while ( g_pulses > 0 )
        {
        	dbgSetHigh();
        	static uint8_t i;
        	for ( i=0; i<g_high; i++ )
        	{
        	    crDELAY( xHandle, 1 );
        	    if ( !g_pulses )
        	    	goto DBG_END;
        	}
        	dbgSetLow();
        	for ( i=0; i<g_low; i++ )
        	{
        		crDELAY( xHandle, 1 );
        	    if ( !g_pulses )
        	    	goto DBG_END;
        	}
        	g_pulses--;
        }
DBG_END:
        // If not busy give other tasks time to live.
		crDELAY( xHandle, 128 );
    }
    crEND();
}


