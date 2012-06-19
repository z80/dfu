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
static void    reportFailure( const char * stri );

typedef void (*pFunction)(void);
pFunction Jump_To_Application;
uint32_t JumpAddress;

uint8_t do_report_failure = 0;

int main(void)
{
    switchesInit();
    uint8_t do_disk = diskMode();
    uint8_t do_dfu  = dfuMode();
    //while ( 1 )
    //{
    //}

    if ( do_disk )
    {
jump_to_application_failure:
        // Initialize disk drive.
        disk_initialize( 0 );
        if ( do_report_failure )
        {
            reportFailure( _T( "Failed to start regular firmware\n" ) );
        }
        else
        {
            if ( do_dfu )
            {
                // Overwrite image from disk.
                dfu();
            }
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
    else
    {
        if ( ( (*(__IO uint32_t *)FIRMWARE_START_ADDRESS) & 0x2FFE0000 ) == 0x20000000 )
        {
            JumpAddress = *(__IO uint32_t *)(FIRMWARE_START_ADDRESS + 4);
            Jump_To_Application = (pFunction)JumpAddress;
            __set_MSP( *(__IO uint32_t *)FIRMWARE_START_ADDRESS );
            Jump_To_Application();
        }
        // On jump failure or on if condition failure will jump 
        // to Usb FLASH disk initialization routine.
        // Clear probable reflash flag.
        do_report_failure = 1;
        goto jump_to_application_failure;
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
        return 0;
    return 1;
}

static uint8_t dfuMode( void )
{
    if ( GPIO_ReadInputDataBit( PIN_PORT, PIN_OP_MODE ) )
        return 0;
    return 1;
}

static void    dfu( void )
{
    FRESULT rc;
    FATFS fatfs;
    FIL   fil;
    // FILINFO info;
    UINT br;
    // UINT bw, i;
    rc = f_mount( 0, &fatfs );
    if ( rc != FR_OK )
        goto dfu_end;

    rc = f_open( &fil, DFU_FILE_NAME, FA_READ );
    if ( rc != FR_OK )
    {
        reportFailure( _T( "No firmware.bin file to reflash the MCU\n" ) );
        goto dfu_end;
    }

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
        flashData = FLASH_If_Read( flashPtr );
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
    {
        reportFailure( _T( "Firmwre is the same, no need in reflashing the MCU\n" ) );
        goto dfu_end;
    }

    // Reflash routine itself.
    rc = f_open( &fil, DFU_FILE_NAME, FA_READ );
    if ( rc != FR_OK )
    {
        reportFailure( _T( "Failed to open firmware.bin file in second time to reflash the MCU\n" ) );
        goto dfu_end;
    }

    flashPtr = FIRMWARE_START_ADDRESS;
    do {
        rc = f_read( &fil, buffer, sizeof(buffer), &br );
        if ( rc != FR_OK )
            goto dfu_end;

        // Erase sector.
        FLASH_If_Erase( flashPtr );
        // Write the data in "buffer" to FLASH memory of the controller.
        FLASH_If_Write( flashPtr, buffer, br );
        // Increment flashPtr to pint to another sector.
        flashPtr += FLASH_SECTOR_SIZE;
    } while ( br > 0 );
    f_close( &fil );
 
    // May be write a brief report about flashing.

dfu_end:
    FLASH_If_Finit();
    f_mount( 0, NULL );
}

static void    reportFailure( const char * stri )
{
    FRESULT rc;
    FATFS fatfs;
    FIL   fil;
    // FILINFO info;
    // UINT br;
    // UINT bw;
 
    rc = f_mount( 0, &fatfs );
    if ( rc != FR_OK )
        goto report_end;

   rc = f_open( &fil, DFU_REPORT_NAME, FA_WRITE | FA_OPEN_ALWAYS );
    if ( rc != FR_OK )
        goto report_end;
    rc = f_lseek( &fil, fil.fsize );
    if ( rc != FR_OK )
        goto report_end;
    rc = f_puts( stri, &fil );
    //if ( rc != FR_OK )
    //    goto report_end;

report_end:
    f_close( &fil );
    f_mount( 0, NULL );
}


DWORD get_fattime (void)
{
    return ((DWORD)(2010 - 1980) << 25)	/* Fixed to Jan. 1, 2010 */
           | ((DWORD)1 << 21)
           | ((DWORD)1 << 16)
           | ((DWORD)0 << 11)
           | ((DWORD)0 << 5)
           | ((DWORD)0 >> 1);
}


