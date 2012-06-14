/******************** (C) COPYRIGHT 2011 STMicroelectronics ********************
* File Name          : flash_if.c
* Author             : MCD Application Team
* Version            : V3.3.0
* Date               : 21-March-2011
* Description        : specific media access Layer for internal flash
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "flash_if.h"
#include "dfu_config.h"

/*******************************************************************************
* Function Name  : FLASH_If_Init
* Description    : Initializes the Media on the STM32
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
uint16_t FLASH_If_Init(void)
{
    FLASH_Unlock();
    return 0;
}

uint16_t FLASH_If_Finit(void)
{
    FLASH_Lock();
    return 0;
}

/*******************************************************************************
* Function Name  : FLASH_If_Erase
* Description    : Erase sector
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
uint16_t FLASH_If_Erase(uint32_t SectorAddress)
{
    FLASH_ErasePage(SectorAddress);
    return 0;
}

/*******************************************************************************
* Function Name  : FLASH_If_Write
* Description    : Write sectors
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
uint16_t FLASH_If_Write(uint32_t SectorAddress, uint8_t * data, uint32_t DataLength)
{
    uint32_t idx = 0;
    
    // I'll use only aligned data always or actual data size is greater 
    // then DataLength's value!!!
    if  (DataLength & 0x3) /* Not an aligned data */
    {
      for (idx = DataLength; idx < ((DataLength & 0xFFFC) + 4); idx++)
      {
          data[idx] = 0xFF;
      }
    } 
  
 
    /* Data received are Word multiple */    
    for (idx = 0; idx <  DataLength; idx = idx + 4)
    {
        FLASH_ProgramWord(SectorAddress, *(uint32_t *)(data + idx));  
        SectorAddress += 4;
    } 
 
    return 0;
}

/*******************************************************************************
* Function Name  : FLASH_If_Read
* Description    : Read sectors
* Input          : None
* Output         : None
* Return         : buffer address pointer
*******************************************************************************/
uint8_t *FLASH_If_Read (uint32_t SectorAddress)
{
    return  (uint8_t*)(SectorAddress);
}

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/





