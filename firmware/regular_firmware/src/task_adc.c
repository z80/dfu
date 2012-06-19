
#include "task_adc.h"
#include "stm32f10x.h"
#include "data_struct.h"

#define GPIO_ADC       GPIOA
#define ADC_CLK        RCC_APB2Periph_ADC1
#define GPIO_ADC_CLK   RCC_APB2Periph_GPIOA
#define ADC_PIN        GPIO_Pin_0
#define ADC_CHANNEL    ADC_Channel_0

// Bandstop Chebyshev's filter with 50Hz at 1000Hz measure freq.
#define AX0     (55924)
#define AX1     (-213130)
#define AX2     (314910)
#define AX3     (-213130)
#define AX4     (55924)
#define AY1     (244248)
#define AY2     (-353125)
#define AY3     (234023)
#define AY4     (-60170)
#define AFACTOR 16

#define FILTER_LEN 4

int32_t g_xx[FILTER_LEN], 
        g_yy[FILTER_LEN];

static uint16_t adc( void );
static void     filterInit( void );
static int32_t  filter( uint16_t x );

void vTaskAdc( void * args )
{
    static portTickType prevTime;
    static portTickType pointTime;
    static TData * st;
    int32_t i;
    st = data();
    filterInit();
    prevTime = xTaskGetTickCount();
    
    pointTime = 0;
    for ( ;; )
    {

        // Get ADC data.
        i = adc();
        // Filter ADC data.
        i = filter( i );
        // Put ADC data.
        adcSetData( i );
        vTaskDelayUntil( &prevTime, ADC_INTERVAL );
        pointTime += ADC_INTERVAL;
        if ( pointTime >= PLOT_TIME_PER_PT )
        {
            pointTime = 0;
            plotPlaceCurrentData();
        }
    }
}

void adcInit( void )
{
    // ADC Clock Div.
    RCC_ADCCLKConfig( RCC_PCLK2_Div8 );
    // Turn ADC and appropriate GPIO's clocks on.
    RCC_APB2PeriphClockCmd( ADC_CLK | GPIO_ADC_CLK, ENABLE );
    // Setup GPIO in type.
    GPIO_InitTypeDef init;
    init.GPIO_Pin  = ADC_PIN;
    init.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init( GPIO_ADC, &init );
    // Setp ADC.
    ADC_DeInit( ADC1 );
    ADC_InitTypeDef adcInit;
    ADC_StructInit( &adcInit );
    adcInit.ADC_Mode               = ADC_Mode_Independent;
    adcInit.ADC_ContinuousConvMode = DISABLE;
    adcInit.ADC_ExternalTrigConv   = ADC_ExternalTrigConv_None;
    adcInit.ADC_DataAlign          = ADC_DataAlign_Right;
    adcInit.ADC_NbrOfChannel       = 1;
    
    // Now do the setup 
    ADC_Init( ADC1, &adcInit );
    // Enable ADC1
    ADC_Cmd( ADC1, ENABLE );

    // Enable ADC1 reset calibaration register
    ADC_ResetCalibration( ADC1 );
    // Check the end of ADC1 reset calibration register
    while( ADC_GetResetCalibrationStatus( ADC1 ) )
        ;
    // Start ADC1 calibaration
    ADC_StartCalibration( ADC1 );
    // Check the end of ADC1 calibration
    while( ADC_GetCalibrationStatus( ADC1 ) )
        ;
}

static uint16_t adc( void )
{
    ADC_RegularChannelConfig( ADC1, ADC_CHANNEL, 1, ADC_SampleTime_239Cycles5 );
    ADC_SoftwareStartConvCmd( ADC1, ENABLE );
    while ( ADC_GetFlagStatus( ADC1, ADC_FLAG_EOC ) != SET )
        ;
    uint16_t res;
    res = ADC_GetConversionValue( ADC1 );
    return res;
}

static void filterInit( void )
{
    g_xx[0] = 
    g_xx[1] = 
    g_xx[2] = 
    g_xx[3] = 
    g_yy[0] = 
    g_yy[1] = 
    g_yy[2] = 
    g_yy[3] = 0;
}

static int32_t filter( uint16_t x )
{
    // Filter input data.
    int32_t sum = (int32_t)x;
    sum *= AX0;
    sum += AX1 * g_xx[0] + AY1 * g_yy[0] + 
           AX2 * g_xx[1] + AY2 * g_yy[1] + 
           AX3 * g_xx[2] + AY3 * g_yy[2] + 
           AX4 * g_xx[3] + AY4 * g_yy[3];
    // Filter operates with integer coefs shifted left on AFACTOR.
    // So result should be shifted back, e.i. right on the same digits number.
    sum >>= AFACTOR;
    // Shift coefs in filter memory.
    g_xx[3] = g_xx[2];
    g_xx[2] = g_xx[1];
    g_xx[1] = g_xx[0];
    g_xx[0] = x;
    g_yy[3] = g_yy[2];
    g_yy[2] = g_yy[1];
    g_yy[1] = g_yy[0];
    g_yy[0] = sum;

    return sum;
}






