

#ifndef __PAWN_CONFIG_H_
#define __PAWN_CONFIG_H_

// Total Pawn script, heap, stack size in cells, not in bytes (!!!)
#define PAWN_MEM_SIZE ( 512 )
// Array of int type for host computer IO.
#define PAWN_IO_SIZE  ( 8 )
// FLASH parameters.
#define PAWN_PAGE_SIZE     1024
// Start FLASH block to keep pawn program in.
#define PAWN_START_PAGE    40
// Start of FLASH memory
#define PAWN_FLASH_START   0x8000000
// Chibios PAWN thread stack depth.
#define PAWN_THREAD_STACK_DEPTH 2048

#endif

