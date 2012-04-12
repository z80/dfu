/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/main.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */  

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include <stdio.h>
#include "3310.h"
//#include "lcd.h"
#include "FreeRTOS.h"
#include "task.h"
#include "data_struct.h"
#include "task_plot.h"
#include "task_adc.h"

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
 USART_InitTypeDef USART_InitStructure;

/* Private function prototypes -----------------------------------------------*/
#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

#define GPIO_PORT           GPIOB
#define GREEN_GPIO_PIN      GPIO_Pin_7
#define RED_GPIO_PIN        GPIO_Pin_6
#define GPIO_PORT_CLK       RCC_AHBPeriph_GPIOB



static void clockConfig( void );
static void gpioConfig( void );
void vTaskDisplay( void * args );
/* Private functions ---------------------------------------------------------*/

void delay()
{
  const int cnt = 8000000;
  volatile int i;
  for ( i=0; i<cnt; i++ )
      asm( "nop" );
}


/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
    NVIC_SetVectorTable( NVIC_VectTab_FLASH, 0x3000 );
    clockConfig();
    //gpioConfig();
    adcInit();
    lcdPower( 1 );
    dataInit();

    xTaskCreate( vTaskDisplay, ( signed char * ) "a", configMINIMAL_STACK_SIZE*8, NULL, tskIDLE_PRIORITY+1, NULL );
    //xTaskCreate( vTaskAdc,  (signed char *)"a", configMINIMAL_STACK_SIZE*8, NULL, tskIDLE_PRIORITY+1, NULL );
    //xTaskCreate( vTaskPlot, (signed char *)"b", configMINIMAL_STACK_SIZE*8, NULL, tskIDLE_PRIORITY+1, NULL );
    vTaskStartScheduler();
    
    //prepareData();
    //LcdInit();
    while (1)
    {
        //GPIO_Write( GPIOB, GREEN_GPIO_PIN | RED_GPIO_PIN | GPIO_Pin_10 );
        //delay();
        //GPIO_Write( GPIOB, 0 );
        delay();

        lcdClear();
        lcdLine( 0, 0, 80, 30, PIXEL_ON );
        lcdLine( 0, 40, 80, 0, PIXEL_ON );
        lcdLine( 40, 40, 40, 0, PIXEL_ON );
        lcdGotoXy( 5, 5 );
        lcdStrConst( FONT_1X, "Hello!" );
        lcdUpdate();
        delay();
        lcdClear();
        //lcdLine( 0, 0, 80, 30, PIXEL_ON );
        //lcdLine( 0, 40, 80, 0, PIXEL_ON );
        //lcdLine( 40, 40, 40, 0, PIXEL_ON );
        lcdGotoXy( 5, 5 );
        lcdStrConst( FONT_1X, "Hi!" );
        lcdUpdate(); 
  }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif




static void clockConfig( void ) 
{
    RCC_DeInit();
    
    RCC_HSEConfig( RCC_HSE_ON );
    RCC_WaitForHSEStartUp();
    while( RCC_GetFlagStatus( RCC_FLAG_HSERDY ) != SET )
        ;
    RCC_PLLCmd( DISABLE );
    RCC_HCLKConfig( RCC_SYSCLK_Div1 );
    RCC_PCLK1Config( RCC_HCLK_Div1 );
    RCC_PCLK2Config( RCC_HCLK_Div1 );
    RCC_PLLConfig( RCC_PLLSource_HSE_Div1, RCC_PLLMul_4 );
    RCC_PLLCmd( ENABLE );
    while ( RCC_GetFlagStatus( RCC_FLAG_PLLRDY ) != SET )
        ;
    RCC_SYSCLKConfig( RCC_SYSCLKSource_PLLCLK );

    //RCC_HSIConfig( RCC_HSI_OFF );

    RCC_USBCLKConfig( RCC_USBCLKSource_PLLCLK_Div1 );
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_USB, ENABLE );
}

static void gpioConfig( void )
{
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE );
    GPIO_InitTypeDef GPIO_InitStructure;
  
    GPIO_InitStructure.GPIO_Pin = GREEN_GPIO_PIN | RED_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIO_PORT, &GPIO_InitStructure );
}

void vTaskDisplay( void * args )
{
    for ( ;; )
    {
        lcdClear();
        //lcdLine( 40, 40, 40, 0, PIXEL_ON );
        lcdGotoXy( 1, 1 );
        lcdStrConst( FONT_1X, "Hello!" );
        lcdUpdate();
        vTaskDelay( 1000 );
        
        lcdClear();
        lcdGotoXy( 1, 1 );
        lcdStrConst( FONT_1X, "Hi!" );
        lcdUpdate(); 
        vTaskDelay( 1000 );
    }
}

void vApplicationMallocFailedHook( void )
{
    volatile uint8_t i=0;
    for ( ;; )
        i++;
}

void vApplicationStackOverflowHook( xTaskHandle task, signed char * pcTaskName )
{
    volatile uint8_t i=0;
    for ( ;; )
        i++;
}



