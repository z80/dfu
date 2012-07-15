
#include "cr_i2c.h"
#include "config.h"
#include "stm32f10x.h"

xQueueHandle g_sendQueue;
xQueueHandle g_receiveQueue;
uint8_t      g_state = I2C_IDLE;
uint8_t      g_cmd   = I2C_IDLE;
uint8_t      g_address = 0;

uint8_t g_initialized = 0;

static void init()
{
    g_toSlaves   = xQueueCreate( I2C_QUEUE_SIZE, 1 );
    g_fromSlaves = xQueueHandle( I2C_QUEUE_SIZE, 1 );
}

xQueueHandle i2cSendQueue()
{
    return g_sendQueue;
}

xQueueHandle i2cReceiveQueue()
{
    return g_receiveQueue;
}

uint8_t i2cStatus()
{
    return g_status;
}

void crI2c( xCoRoutineHandle xHandle, 
            unsigned portBASE_TYPE uxIndex )
{
    crSTART( xHandle );
    for ( ;; )
    {
        if ( !g_initialized )
        {
            init();
	    g_initialized = 1;
        }
        if ( g_cmd == I2C_SEND )
	{
	    
	}

    }
    crEND();
}




