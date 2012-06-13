/******************** (C) COPYRIGHT 2011 STMicroelectronics ********************
* File Name          : main.c
* Author             : MCD Application Team
* Version            : V3.3.0
* Date               : 21-March-2011
* Description        : Mass Storage demo main file
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#ifdef STM32L1XX_MD
 #include "stm32l1xx.h"
#else
 #include "stm32f10x.h"
#endif /* STM32L1XX_MD */
 
#include "usb_lib.h"
#include "hw_config.h"
#include "usb_pwr.h"

#include "diskio.h"
#include "dfu_config.h"
#include "ff.h"
#include "flash_if.h"

static void    switchesInit( void );
static uint8_t diskMode( void );
static uint8_t dfuMode( void );
static void    dfu( void );

extern uint16_t MAL_Init (uint8_t lun);

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : main.
* Description    : Main routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
int main(void)
{
    switchesInit();
    //NVIC_SetVectorTable( NVIC_VectTab_FLASH, 0x3000 );
    //Set_System();
    uint8_t disk = diskMode();
    uint8_t dfu  = dfuMode();
    while ( 1 )
    {
    }

    if ( disk )
    {
        // Initialize disk drive.
        disk_initialize( 0 );
        if ( dfu )
        {
            // Overwrite image from disk.
        }
        // After dfu initialize USB disk.
        Set_USBClock();
        Led_Config();
        USB_Interrupts_Config();
        USB_Init();
        while (bDeviceState != CONFIGURED);

        USB_Configured_LED();

        while (1)
        {
        }
    }
}

#ifdef USE_FULL_ASSERT
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert_param error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert_param error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

static void switchesInit( void )
{
    RCC_APB2PeriphClockCmd( PIN_CLK, ENABLE );
    GPIO_InitTypeDef s;
    s.GPIO_Pin   = PIN_FLASH_MODE | PIN_OP_MODE;
    s.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    s.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init( PIN_PORT, &s );
}

static uint8_t diskMode( void )
{
    if ( GPIO_ReadInputDataBit( PIN_PORT, PIN_FLASH_MODE ) )
        return 1;
    return 0;
}

static uint8_t dfuMode( void )
{
    if ( GPIO_ReadInputDataBit( PIN_PORT, PIN_OP_MODE ) )
        return 1;
    return 0;
}

static void    dfu( void )
{
    FRESULT rc;
    FATFS fatfs;
    FIL   fil;
    FILINFO info;
    UINT bw, br, i;
    rc = f_mount( 0, &fatfs );
    if ( rc != FR_OK )
        goto dfu_end;

    rc = f_open( &fil, "DFU_FILE_NAME", FA_READ );
    if ( rc != FR_OK )
        goto dfu_end;

    FLASH_If_Init();
    uint8_t buffer[ FLASH_SECTOR_SIZE ];
    uint32_t flashPtr = FIRMWARE_START_ADDRESS;
    uint8_t * flashData;
    uint32_t ttt;
    uint8_t doReflash = 0;
    // First compare flash content with file content.
    // To check if reflash is really necessary.
    do {
        rc = f_read( &fil, buffer, sizeof(buffer), &br );
        if ( rc != FR_OK )
            goto dfu_end;
        flashData = FLASH_IF_Read( flashPtr );
        for ( ttt=0; ttt<br; ttt++ )
        {
            if ( buffer[ttt] != flashData[ttt] )
            {
                doReflash = 1;
                break;
            }
        }
        if ( doReflash )
            break;
        flashPtr += FLASH_SECTOR_SIZE;
    } while ( br > 0 );
    f_close( &fil );
    
    if ( !doReflash )
        goto dfu_end;

    // Reflash routine itself.
    rc = f_open( &fil, "DFU_FILE_NAME", FA_READ );
    if ( rc != FR_OK )
        goto dfu_end;

    do {
        rc = f_read( &fil, buffer, sizeof(buffer), &br );
        if ( rc != FR_OK )
            goto dfu_end;
        // Erase sector.
        // ...
        // Write the data in "buffer" to FLASH memory of the controller.
        // ...
    } while ( br > 0 );
    f_close( &fil );
 
    // May be write a brief report about flashing.

dfu_end:
    f_mount( 0, NULL );
}





