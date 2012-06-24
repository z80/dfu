
#include "cr_usbio.h"
#include "cr_funcs.h"
#include "config.h"
#include "hw_config.h"

#define STATE_CMD  0  // State wait for cmd.
#define STATE_SIZE 1  // State wait for data cnt.
#define STATE_DATA 2  // State wait for data.
#define STATE_FUNC 3  // State wait for func.

xQueueHandle g_toMcu;
xQueueHandle g_fromMcu;
uint8_t      g_buffer[ CMD_BUFFER_SIZE ];

static void initUsbIo( void )

{
    g_toMcu   = xQueueCreate( IN_QUEUE_SIZE,  1 );
    g_fromMcu = xQueueCreate( OUT_QUEUE_SIZE, 1 );
}

/*uint8_t putToMcuIsr( uint8_t value )
{
    portBASE_TYPE res = crQUEUE_SEND_FROM_ISR( g_toMcu, &value, pdFALSE );
    return ( res == pfTRUE ) ? 0 : 1;
}*/

xQueueHandle toMcu( void )
{
    return g_toMcu;
}

xQueueHandle fromMcu( void )
{
    return g_fromMcu;
}

uint8_t    * buffer( void )
{
    return g_buffer;
}
    
void crUsbIo( xCoRoutineHandle xHandle, 
              unsigned portBASE_TYPE uxIndex )
{
    static uint8_t data;
    static portBASE_TYPE rcTo, rcFrom;
    static uint8_t state = STATE_CMD;
    static uint8_t size = 0;
    static uint8_t bufferIndex = 0;
    static uint32_t stateResetTimeout = 0;

    initUsbIo();

    crSTART( xHandle );

    for ( ;; )
    {
        // Receive data from USB and place to an execution buffer.
        crQUEUE_RECEIVE( xHandle, g_toMcu, &data, 0, &rcTo );
        if ( rcTo == pdPASS )
        {
            // Analyze data and put it into execution buffer.
            switch ( state )
            {
                case STATE_CMD:
                    // Reset timer.
                    stateResetTimeout = STATE_RESET_TIMEOUT;
                    if ( data == CMD_DATA )
                        state = STATE_SIZE;
                    else if ( data == CMD_FUNC )
                        state = STATE_FUNC;
                    break;
                case STATE_SIZE:
                    size = data;
                    state = STATE_DATA;
                    break;
                case STATE_DATA:
                    g_buffer[ bufferIndex++ ] = data;
                    size--;
                    if ( size == 0 )
                        state = STATE_CMD;
                    break;
                case STATE_FUNC:
                    // Function invocation.
                    invokeFunc( data );
                    crDELAY( xHandle, 1 );
                    // And state back to STATE_CMD
                    state = STATE_CMD;
                    bufferIndex = 0;
                    break;
            }
        }
        // Receive data to USB and send it.
        crQUEUE_RECEIVE( xHandle, g_fromMcu, &data, 0, &rcFrom );
        if ( rcFrom == pdPASS )
        {
            USB_Send_Data( data );
        }
        
        if ( (rcTo != pdPASS) && (rcFrom != pdPASS) )
        {
            crDELAY( xHandle, 1 );
            if ( stateResetTimeout > 0 )
                stateResetTimeout--;
            else
            {
                state = STATE_CMD;
                bufferIndex = 0;
            }
        }
    }

    crEND();
}



