
#include "ch.h"
#include "hal.h"

#include "usb_ctrl.h"

//*
//* Application entry point.
//*
int main(void)
{
  halInit();
  chSysInit();

  initUsb();

  while ( 1 )
  {
      processShell();
      chThdSleepMilliseconds( 250 );
  }
  return 0;
}






