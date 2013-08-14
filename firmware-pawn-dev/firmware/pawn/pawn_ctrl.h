
#ifndef __PAWN_CTRL_H_
#define __PAWN_CTRL_H_

#include <ch.h>
#include <shell.h>

void     pawnInit( void );
void     pawnSetIo( uint8_t cnt, uint8_t * vals );
void     pawnIo( uint8_t cnt, uint8_t * vals );
void     pawnSetMem( uint8_t cnt, uint16_t at, uint8_t * vals );
uint16_t pawnWriteFlash( uint8_t page );
void     pawnRun( void );
uint8_t  pawnIsRunning( void );
void     pawnStop( void );

#endif

