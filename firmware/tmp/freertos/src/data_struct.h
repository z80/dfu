
#ifndef __DATA_STRUCT_H_
#define __DATA_STRUCT_H_

#include "3310.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "stm32f10x.h"

//#define ADC_DATA_SZ     128
#define PLOT_DATA_SZ    LCD_X_RES
#define ADC_FREQ        1000
#define ADC_INTERVAL    ( configTICK_RATE_HZ / ADC_FREQ )

#define PLOT_PERIOD             3000   // All screen replot in milliseconds
#define PLOT_REPLOT_INTERVAL    500    // Replot interval in milliseconds
#define PLOT_TIME_PER_PT        (PLOT_PERIOD / PLOT_DATA_SZ)  // Should be 20 at the moment.

typedef struct
{
    // ADC data collected.
    int32_t adcData;
    // PLOT data to be displayed.
    int32_t plotData[ PLOT_DATA_SZ ];
    // Data buffer to be plotted - a copy of plotData;
    int32_t plotDataPlot[ PLOT_DATA_SZ ];
    // Current index data to be put.
    uint8_t  index;

    // Mutex for exclusive data access.
    xSemaphoreHandle mutex;

} TData;

TData * data( void );
void    dataInit( void );
void    adcSetData( int32_t val );
void    plotPlaceCurrentData( void );
void    plotPrepareData( void );


#endif


