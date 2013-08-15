
#include "ch.h"
#include "hal.h"

#include "usb_ctrl.h"
#include "pawn_ctrl.h"
#include "misc.h"

//*
//* Application entry point.
//*
int main(void)
{
  //NVIC_SetVectorTable( NVIC_VectTab_FLASH, 0x2800 );

  halInit();
  chSysInit();

  pawnInit();
  initUsb();

  pawnRun();
  while ( 1 )
  {
      processShell();
      chThdSleepMilliseconds( 250 );
  }
  return 0;
}






