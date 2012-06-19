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

#include "ff.h"

// USB related includes.
#include "usb_lib.h"
#include "usb_desc.h"
#include "hw_config.h"
#include "usb_pwr.h"
#include "usb_istr.h"
#include <stdint.h>
#include <stdio.h>

void USB_LP_CAN1_RX0_IRQHandler(void)
{
  USB_Istr();
}




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

DWORD get_fattime( void )
{
    static DWORD i = 0;
    return i++;
}


/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
    // The very first line should remap NVIC table 
    // in order to make remapped firmware work with 
    // interrupts in the right way.
    // Otherwise actual interrupts table location 
    // and one MCU tries to work with would be different.
    NVIC_SetVectorTable( NVIC_VectTab_FLASH, 0x5000 );
    // USB setup.
    Set_USBClock();
	USB_Interrupts_Config();
	USB_Init();

    //gpioConfig();
    //adcInit();
    //lcdPower( 1 );
    //dataInit();

    // FatFS.
    FRESULT rc;
    FATFS   fatfs;
    FIL     fil;
    DIR     dir;
    FILINFO info;
    UINT    bw, br, i;
    char stri[] = "file content!\n";
    disk_initialize( 0 );
    f_mount( 0, &fatfs );
    rc = f_open( &fil, "MESSAGE.TXT", FA_WRITE | FA_OPEN_ALWAYS );
    if ( rc == FR_OK )
    {
        rc = f_lseek( &fil, fil.fsize );
        if ( rc == FR_OK )
        {
            f_write( &fil, stri, sizeof(stri), &br );
            f_close( &fil );

            char res[128];
            rc = f_open( &fil, "MESSAGE.TXT", FA_READ );
            if ( rc == FR_OK )
            {
                rc = f_read( &fil, res, sizeof( res ), &br );
                f_close( &fil );
            }
        }
    }
    while ( 1 )
        ;



    //xTaskCreate( vTaskDisplay, ( signed char * ) "a", configMINIMAL_STACK_SIZE*8, NULL, tskIDLE_PRIORITY+1, NULL );
    //vTaskStartScheduler();   
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

        // Disc utility.
        disk_timerproc();
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


