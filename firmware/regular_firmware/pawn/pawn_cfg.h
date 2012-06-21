
#ifndef __PAWN_CFG_H_
#define __PAWN_CFG_H_

#define STACK_SIZE 256  // cells (cell is equal to 4 bytes usually.)
#define POOL_SIZE  1024 // bytes
#define STACK_SZ ( (${STACK_SIZE} + 8) * sizeof(cell) )
#define POOL_SZ  ${POOL_SIZE}

#endif

