
#include "data_struct.h"


TData st;

TData * data( void )
{
    return &st;
}

void dataInit( void )
{
    st.index = 0;
    uint16_t i;
    st.adcData = 2047;
    for ( i=0; i<PLOT_DATA_SZ; i++ )
        st.plotData[i] = 2047;
    st.mutex = xSemaphoreCreateMutex(); 
}

void    adcSetData( int32_t val )
{
    while ( xSemaphoreTake( st.mutex, 1 ) != pdTRUE )
       vTaskDelay( 1 );
    st.adcData = val;
    xSemaphoreGive( st.mutex );
}

void   plotPlaceCurrentData( void )
{
    while ( xSemaphoreTake( st.mutex, 1 ) != pdTRUE )
       vTaskDelay( 1 );
    st.plotData[ st.index ] = st.adcData;
    st.index = (st.index + 1) % PLOT_DATA_SZ;
    xSemaphoreGive( st.mutex );
}

void plotPrepareData( void )
{
    while ( xSemaphoreTake( st.mutex, 1 ) != pdTRUE )
        vTaskDelay( 1 );
    uint16_t i;
    for ( i=0; i<PLOT_DATA_SZ; i++ )
        st.plotDataPlot[i] = st.plotData[i];
    xSemaphoreGive( st.mutex );
}







