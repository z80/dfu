
#include "task_plot.h"
#include "FreeRTOS.h"
#include "data_struct.h"
#include "3310.h"
#include <stdio.h>
/* for caddr_t (typedef char * caddr_t;) */
#include <sys/types.h>


void vTaskPlot( void * args )
{
    static portTickType prevTime;
    static TData * st;
    static uint8_t i;
    static char stri[16];
    st = data();
    prevTime = xTaskGetTickCount();
    for ( ;; )
    {
        plotPrepareData();
        int32_t vmin, vmax;
        vmin = vmax = st->plotDataPlot[0];
        for ( i=1; i<PLOT_DATA_SZ; i++ )
        {
            vmin = ( vmin > st->plotDataPlot[i] ) ? st->plotDataPlot[i] : vmin;
            vmax = ( vmax < st->plotDataPlot[i] ) ? st->plotDataPlot[i] : vmax;
        }
        vmax = ( vmax > vmin ) ? vmax : (vmax + 1);

        int32_t y1, y2;
        y1 = LCD_Y_RES - 1 - (LCD_Y_RES - 1) * ( st->plotDataPlot[0] - vmin ) / (vmax - vmin);
        //if ( y1 >= LCD_Y_RES )
        //    y1 = LCD_Y_RES-1;
        lcdClear();
       
        sprintf( stri, "max: %i", (int)vmax );
        lcdGotoXy( 1, 1 );
        lcdStrConst( FONT_1X, stri );

        sprintf( stri, "min: %i", (int)vmin );
        lcdGotoXy( 1, 6 );
        lcdStrConst( FONT_1X, stri );

        //lcdGotoXy( 2, 1 );
        //lcdStrConst( FONT_1X, "hi!" );
       
        for ( i=1; i<PLOT_DATA_SZ; i++ )
        {
            y2 = LCD_Y_RES - 1 - (LCD_Y_RES - 1) * ( st->plotDataPlot[i] - vmin ) / (vmax - vmin);
            //if ( y2 >= LCD_Y_RES )
            //    y2 = LCD_Y_RES-1;
            lcdLine( i-1, y1, i, y2, PIXEL_ON );
            y1 = y2;
        }
        lcdLine( st->index, 0, st->index, LCD_Y_RES-1, PIXEL_ON );
        lcdUpdate();
        vTaskDelayUntil( &prevTime, PLOT_REPLOT_INTERVAL );
    }
}

extern int  _sdata;

caddr_t _sbrk ( int incr )
{
  static unsigned char *heap = NULL;
  unsigned char *prev_heap;

  if (heap == NULL) {
    heap = (unsigned char *)&_sdata;
  }
  prev_heap = heap;
  /* check removed to show basic approach */

  heap += incr;

  return (caddr_t) prev_heap;
}

