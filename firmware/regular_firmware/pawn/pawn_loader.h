
#ifndef __PAWN_LOADER_H_
#define __PAWN_LOADER_H_

struct SPawnLoader;
//struct tagAMX;
typedef struct SPawnLoader Pawn;
//typedef struct tagAMX      AMX;
#include "amx.h"

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


