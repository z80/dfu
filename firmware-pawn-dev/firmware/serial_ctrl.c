
#include "serial_ctrl.h"
#include "conv_ctrl.h"
#include "light_ctrl.h"
#include "moto_ctrl.h"

#include "hal.h"
#include "shell.h"
#include "chprintf.h"

#define SERIAL_UART       SD3
#define SERIAL_BAUD       9600

static SerialConfig config =
{
    SERIAL_BAUD,
    0,
    0,
    0
};


static void cmd_mem( BaseChannel * chp, int argc, char * argv[] );
static void cmd_temperature( BaseChannel * chp, int argc, char * argv[] );
static void cmd_current( BaseChannel * chp, int argc, char * argv[] );
static void cmd_light( BaseChannel * chp, int argc, char * argv[] );
static void cmd_motoEn( BaseChannel * chp, int argc, char * argv[] );
static void cmd_moto( BaseChannel * chp, int argc, char * argv[] );

#define SHELL_WA_SIZE   THD_WA_SIZE( 2048 )
static const ShellCommand commands[] =
{
    { "mem",  cmd_mem },
    { "cur",  cmd_current },
    { "tem",  cmd_temperature },
    { "li",   cmd_light },
    { "moen", cmd_motoEn },
    { "mo",   cmd_moto },
    { NULL,          NULL }
};

Thread * shelltp = NULL;

static const ShellConfig shell_cfg =
{
    (BaseChannel *)&SERIAL_UART,
    commands
};

void initSerial( void )
{
    palSetPadMode( GPIOB, 10, PAL_MODE_STM32_ALTERNATE_PUSHPULL );
    palSetPadMode( GPIOB, 11, PAL_MODE_INPUT );
    sdStart( &SERIAL_UART, &config );

    chprintf( (BaseChannel *)&SERIAL_UART, "Hello!\r\n" );
    /*
    uint32_t i;
    for ( i=0; i<128; i++ )
    {
        //chprintf( (BaseChannel *)&SERIAL_UART, "Hello!\r\n" );
        sdWrite( &SERIAL_UART, (const uint8_t *)"Hello!\r\n", 8 );
        chThdSleepSeconds( 1 );
    }
    */
}

void processSerial( void )
{
    if ( !shelltp )
        shelltp = shellCreate( &shell_cfg, SHELL_WA_SIZE, NORMALPRIO );
    else if ( chThdTerminated( shelltp ) )
    {
        chThdRelease( shelltp );  // Recovers memory of the previous shell.
        shelltp = NULL;           // Triggers spawning of a new shell.
    }
}

static void cmd_mem( BaseChannel * chp, int argc, char * argv[] )
{
    (void)argc;
    (void)argv;
    size_t n, size;
    n = chHeapStatus( NULL, &size );
    chprintf( chp, "core free memory : %u bytes\r\n", chCoreStatus() );
    chprintf( chp, "heap fragments   : %u\r\n", n );
    chprintf( chp, "heap free total  : %u bytes\r\n", size );
}

static void cmd_temperature( BaseChannel * chp, int argc, char * argv[] )
{
    (void)argc;
    (void)argv;
    chprintf( chp, "temperature: %u\r\n", adcTemperature() );
}

static void cmd_current( BaseChannel * chp, int argc, char * argv[] )
{
    (void)argc;
    (void)argv;
    chprintf( chp, "current: %u\r\n", adcCurrent() );
}

static void cmd_light( BaseChannel * chp, int argc, char * argv[] )
{
    (void)chp;
    if ( argc > 0 )
    {
        if ( argv[0][0] != '0' )
        {
            setLight( 1 );
            return;
        }
    }
    setLight( 0 );
}

static void cmd_motoEn( BaseChannel * chp, int argc, char * argv[] )
{
    (void)chp;
    if ( argc > 0 )
    {
        if ( argv[0][0] != '0' )
        {
            motoSetEn( 1 );
            return;
        }
    }
    motoSetEn( 0 );
}

static void cmd_moto( BaseChannel * chp, int argc, char * argv[] )
{
    (void)chp;
    uint16_t en = 0;
    if ( argc > 0 )
    {
        if ( argv[0][0] != '0' )
            en |= 1;
    }
    if ( argc > 1 )
    {
        if ( argv[1][0] != '0' )
            en |= 2;
    }
    if ( argc > 2 )
    {
        if ( argv[2][0] != '0' )
            en |= 4;
    }
    if ( argc > 3 )
    {
        if ( argv[3][0] != '0' )
            en |= 8;
    }
    motoSet( en );
}










