
#include "pawn_ctrl.h"
#include "amx.h"
#include "pawn_config.h"

#include "stm32f10x_flash.h"

//#include "i2c_ctrl.h"

typedef struct
{
    AMX amx;
    char isRunning;
    int result, error;
    uint32_t    memblock[ PAWN_MEM_SIZE ];
    uint8_t     ioblock[ PAWN_IO_SIZE ];
} Pawn;

Pawn g_pawn;

static cell n_io( AMX * amx, const cell * params );
static cell n_setIo( AMX * amx, const cell * params );

AMX_NATIVE const g_nativetable[] =
{
    n_io,
    n_setIo
};


int AMXAPI aux_Monitor( AMX * amx )
{
    (void)amx;
    return AMX_ERR_EXIT;
}

static void pawnExec( Pawn * pawn )
{
    chSysLock();
        pawn->isRunning = 0;
    chSysUnlock();
    uint32_t i;
    unsigned char * tmp = (unsigned char *)&pawn->amx;
    for ( i=0; i<sizeof( pawn->amx ); i++ )
        tmp[i] = 0;
    pawn->amx.data = (uint8_t *)pawn->memblock;
    uint8_t * rom = (uint8_t *)(PAWN_FLASH_START + PAWN_PAGE_SIZE * PAWN_START_PAGE);
    pawn->result = amx_Init( &pawn->amx, rom );
    if ( pawn->result != AMX_ERR_NONE )
        return;

    static cell ret = 0;
    chSysLock();
        pawn->isRunning = 1;
    chSysUnlock();

    pawn->error = amx_Exec( &pawn->amx, &ret, AMX_EXEC_MAIN );
    pawn->result = ret;
    amx_Cleanup( &pawn->amx );

    chSysLock();
        pawn->isRunning = 0;
    chSysUnlock();
}




static cell n_io( AMX * amx, const cell * params )
{
    (void)amx;
    uint32_t index;
    index = (uint32_t)params[1];
    return (cell)( g_pawn.ioblock[ index ] );
}

static cell n_setIo( AMX * amx, const cell * params )
{
    (void)amx;
    uint32_t index;
    index = (uint8_t)params[1];
    g_pawn.ioblock[ index ] = params[2];
    return 0;
}



























static WORKING_AREA( waExec, PAWN_THREAD_STACK_DEPTH );
static msg_t execThread( void *arg );
static BinarySemaphore semaphore;

void pawnInit( void )
{
    // Initialize mailbox.
    chBSemInit( &semaphore, TRUE );
    // Creating thread.
    chThdCreateStatic( waExec, sizeof(waExec), NORMALPRIO, execThread, NULL );
}

void pawnSetIo( uint8_t cnt, uint8_t * vals )
{
    uint8_t i;
    for ( i=0; i<cnt; i++ )
        g_pawn.ioblock[i] = vals[i];
}

void pawnIo( uint8_t cnt, uint8_t * vals )
{
    uint8_t i;
    for ( i=0; i<cnt; i++ )
        vals[i] = g_pawn.ioblock[i];
}

void pawnSetMem( uint8_t cnt, uint16_t at, uint8_t * vals )
{
    uint16_t i;
    uint8_t * mem = (uint8_t *)(g_pawn.memblock);
    for ( i=0; i<cnt; i++ )
        mem[at+i] = vals[i];
}

uint8_t pawnMem( uint16_t at )
{
    uint8_t * mem = (uint8_t *)(g_pawn.memblock);
    return mem[ at ]; 
}

uint16_t pawnWriteFlash( uint8_t page )
{
    FLASH_Unlock();
    uint32_t flashD = PAWN_FLASH_START + PAWN_PAGE_SIZE * ( page + PAWN_START_PAGE );
    FLASH_Status st = FLASH_ErasePage( flashD );
    if ( st != FLASH_COMPLETE )
    {
        FLASH_Lock();
        return st;
    }
    uint32_t * memD   = (uint32_t *)( g_pawn.memblock );
    uint32_t   i;
    for ( i=0; i<PAWN_MEM_SIZE; i++ )
    {
        // WORD size is 4 bytes
        st = FLASH_ProgramWord( flashD+i*4, memD[i] );
        if ( st != FLASH_COMPLETE )
        {
	        FLASH_Lock();
	        return 10+st;
	    }
    }
    // to be done.
    FLASH_Lock();
    return 0;
}

void pawnRun( void )
{
    chBSemSignal( &semaphore );
}

uint8_t pawnIsRunning( void )
{
    chSysLock();
        char res = g_pawn.isRunning;
    chSysUnlock();
    return res;
}

void pawnStop( void )
{
    chSysLock();
        amx_SetDebugHook( &(g_pawn.amx), aux_Monitor );
    chSysUnlock();
}

int pawnResult( void )
{
    return g_pawn.result;
}

int pawnError( void )
{
    return g_pawn.error;
}



static msg_t execThread( void *arg )
{
    (void)arg;
    chRegSetThreadName( "e" );
    while ( 1 )
    {
        chBSemWait( &semaphore );

        // If's come here run Pawn machine.
        pawnExec( &g_pawn );
    }
    return 0;
}




