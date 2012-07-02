
// USB related includes.
#include "usb_lib.h"
#include "usb_desc.h"
#include "hw_config.h"
#include "usb_pwr.h"
#include "usb_istr.h"

#include "cr_usbio.h"
#include "cr_funcs.h"
#include "config.h"
#include "hw_config.h"
#include "FreeRTOS.h"
#include "task.h"
#include "croutine.h"
#include "queue.h"

#include "cr_dbg.h"

#define STATE_CMD  0  // State wait for cmd.
#define STATE_SIZE 1  // State wait for data cnt.
#define STATE_DATA 2  // State wait for data.
#define STATE_FUNC 3  // State wait for func.

xQueueHandle g_toMcu;
xQueueHandle g_fromMcu;
uint8_t      g_buffer[ CMD_BUFFER_SIZE ];
uint8_t      g_usbInitialized = 0;

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
    static portBASE_TYPE rcTo;
    static uint8_t state = STATE_CMD;
    static uint8_t size = 0;
    static uint8_t bufferIndex = 0;
    static uint32_t stateResetTimeout = 0;
    
    if ( !g_usbInitialized )
    {
        initUsbIo();
        // USB setup.
        Set_USBClock();
        USB_Interrupts_Config();
        USB_Init();

        g_usbInitialized = 1;
    }

    crSTART( xHandle );

    for ( ;; )
    {
        /*if ( !g_usbInitialized )
        {
            // Data for gpioEn.
            data = 0;
            crQUEUE_SEND( xHandle, g_toMcu, &data, 0, &rcTo );
            data = 1;
            crQUEUE_SEND( xHandle, g_toMcu, &data, 0, &rcTo );
            data = 1;
            crQUEUE_SEND( xHandle, g_toMcu, &data, 0, &rcTo );

            data = 0;
            crQUEUE_SEND( xHandle, g_toMcu, &data, 0, &rcTo );
            data = 1;
            crQUEUE_SEND( xHandle, g_toMcu, &data, 0, &rcTo );
            data = 1;
            crQUEUE_SEND( xHandle, g_toMcu, &data, 0, &rcTo );
   
            // Call gpioConfig.
            data = 1;
            crQUEUE_SEND( xHandle, g_toMcu, &data, 0, &rcTo );
            data = 2;
            crQUEUE_SEND( xHandle, g_toMcu, &data, 0, &rcTo );

            // Data for gpioConfig
            data = 0;
            crQUEUE_SEND( xHandle, g_toMcu, &data, 0, &rcTo );
            data = 1;
            crQUEUE_SEND( xHandle, g_toMcu, &data, 0, &rcTo );
            data = 1;
            crQUEUE_SEND( xHandle, g_toMcu, &data, 0, &rcTo );

            data = 0;
            crQUEUE_SEND( xHandle, g_toMcu, &data, 0, &rcTo );
            data = 2;
            crQUEUE_SEND( xHandle, g_toMcu, &data, 0, &rcTo );
            data = 255;
            crQUEUE_SEND( xHandle, g_toMcu, &data, 0, &rcTo );
            data = 255;
            crQUEUE_SEND( xHandle, g_toMcu, &data, 0, &rcTo );

            data = 0;
            crQUEUE_SEND( xHandle, g_toMcu, &data, 0, &rcTo );
            data = 1;
            crQUEUE_SEND( xHandle, g_toMcu, &data, 0, &rcTo );
            data = 0x48;
            crQUEUE_SEND( xHandle, g_toMcu, &data, 0, &rcTo );
   
            // Call gpioConfig.
            data = 1;
            crQUEUE_SEND( xHandle, g_toMcu, &data, 0, &rcTo );
            data = 3;
            crQUEUE_SEND( xHandle, g_toMcu, &data, 0, &rcTo );

            // Data for gpio.
            data = 0;
            crQUEUE_SEND( xHandle, g_toMcu, &data, 0, &rcTo );
            data = 1;
            crQUEUE_SEND( xHandle, g_toMcu, &data, 0, &rcTo );
            data = 1;
            crQUEUE_SEND( xHandle, g_toMcu, &data, 0, &rcTo );

            // Call gpioConfig.
            data = 1;
            crQUEUE_SEND( xHandle, g_toMcu, &data, 0, &rcTo );
            data = 5;
            crQUEUE_SEND( xHandle, g_toMcu, &data, 0, &rcTo );
 
            g_usbInitialized = 1;
        }*/
        // Receive data from USB and place to an execution buffer.
        crQUEUE_RECEIVE( xHandle, g_toMcu, &data, 0, &rcTo );
        if ( rcTo == pdPASS )
        {
            // Analyze data and put it into execution buffer.
            switch ( state )
            {
                case STATE_CMD:
                    //setRed( 1 );
                    //setGreen( 0 );
                    // Reset timer.
                    stateResetTimeout = STATE_RESET_TIMEOUT;
                    if ( data == CMD_DATA )
                    {
                        state = STATE_SIZE;
                    }
                    else if ( data == CMD_FUNC )
                    {
                        state = STATE_FUNC;
                    }
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
                    //setRed( 0 );
                    //setGreen( 1 );
                    // Function invocation.
                    invokeFunc( data );
                    // And state back to STATE_CMD
                    state = STATE_CMD;
                    bufferIndex = 0;
                    break;
            }
        }
        else
        {
            if ( stateResetTimeout > 0 )
                stateResetTimeout--;
            else
            {
                state = STATE_CMD;
                bufferIndex = 0;
            }
        }
        crDELAY( xHandle, 1 );

        // Debugging.
        //static uint8_t a = 'a';
        //crQUEUE_SEND( xHandle, g_fromMcu, &a, 0, &rcFrom );
        //if ( rcFrom == pdPASS )
        //    setRed( ( red() ) ? 0 : 1 );
        /*a = 'b';
        crQUEUE_SEND( xHandle, g_fromMcu, &a, 0, &rcFrom );
        if ( rcFrom == pdPASS )
            setRed( ( red() ) ? 0 : 1 );*/
        //a = '\r';
        //crQUEUE_SEND( xHandle, g_fromMcu, &a, 0, &rcFrom );
        //if ( rcFrom == pdPASS )
        //    setRed( ( red() ) ? 0 : 1 );
        //a = '\n';
        //crQUEUE_SEND( xHandle, g_fromMcu, &a, 0, &rcFrom );
        //if ( rcFrom == pdPASS )
        //    setRed( ( red() ) ? 0 : 1 );
    
        /*taskENTER_CRITICAL();
        USB_Send_Data( 'a' );
        USB_Send_Data( '\r' );
        USB_Send_Data( '\n' );
        taskEXIT_CRITICAL();*/
        //crDELAY( xHandle, 20 );

    }

    crEND();
}



