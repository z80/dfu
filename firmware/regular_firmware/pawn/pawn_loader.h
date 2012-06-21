
#ifndef __PAWN_LOADER_H_
#define __PAWN_LOADER_H_

#include "amx.h"
#include "ff.h"
#include "pawn_cfg.h"
#include "pawn_file_cfg.h"

typedef struct SPawnLoader
{
    AMX amx;

    FIL fil;
    unsigned char datablock[ STACK_SZ + POOL_SZ ];
    // unsigned char * header; //[ HEADER_SZ ];
    // unsigned char * stack; //[ STACK_SZ ];
    // unsigned char * pool; //[ POOL_SZ ];
} Pawn;

#define PAWN_OK               0
#define PAWN_ERR_FILE_OPEN    1
#define PAWN_ERR_FORMAT       2
#define PAWN_ERR_INIT         3
#define PAWN_ERR_EXEC         4

Pawn * pawn( void );
AMX  * pawnAmx( void );
int pawnLoad( Pawn * p, const char * fileName );
int pawnExec( Pawn * p );

#endif


