/******************** (C) COPYRIGHT 2011 STMicroelectronics ********************
* File Name          : hw_config.c
* Author             : MCD Application Team
* Version            : V3.3.0
* Date               : 21-March-2011
* Description        : Hardware Configuration & Setup
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"
#include "usb_prop.h"
#include "usb_desc.h"
#include "hw_config.h"
#include "usb_pwr.h"

#include "cr_usbio.h"
#include "cr_dbg.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
//#define RCC_APB2Periph_GPIO_DISCONNECT RCC_APB2Periph_GPIOF
//#define USB_DISCONNECT_PIN	GPIO_Pin_11
//#define USB_DISCONNECT  GPIOF
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
ErrorStatus HSEStartUpStatus;
USART_InitTypeDef USART_InitStructure;

uint8_t  USART_Rx_Buffer [USART_RX_DATA_SIZE];
uint32_t USART_Rx_ptr_in = 0;
uint32_t USART_Rx_ptr_out = 0;
uint32_t USART_Rx_length  = 0;

uint8_t  USB_Tx_State = 0;
static void IntToUnicode (uint32_t value , uint8_t *pbuf , uint8_t len);
/* Extern variables ----------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : Set_System
* Description    : Configures Main system clocks & power
* Input          : None.
* Return         : None.
*******************************************************************************/
void Set_System(void)
{
  //GPIO_InitTypeDef  GPIO_InitStructure;
  
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f10x_xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f10x.c file
     */   
  
  
  /* Enable USB_DISCONNECT GPIO clock */
  //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_DISCONNECT, ENABLE);

  /* Configure USB pull-up pin */
  //GPIO_InitStructure.GPIO_Pin = USB_DISCONNECT_PIN;
  //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  //GPIO_Init(USB_DISCONNECT, &GPIO_InitStructure);

  //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);

  /* Configure LEDS */
  //GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
  //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  //GPIO_Init(GPIOF, &GPIO_InitStructure);
}

/*******************************************************************************
* Function Name  : Set_USBClock
* Description    : Configures USB Clock input (48MHz)
* Input          : None.
* Return         : None.
*******************************************************************************/
void Set_USBClock(void)
{
  // I configure clock again here by hands.

  /* Select USBCLK source */
  RCC_USBCLKConfig( RCC_USBCLKSource_PLLCLK_Div1 );
  
  /* Enable the USB clock */
  RCC_APB1PeriphClockCmd( RCC_APB1Periph_USB, ENABLE );
}

/*******************************************************************************
* Function Name  : Enter_LowPowerMode
* Description    : Power-off system clocks and power while entering suspend mode
* Input          : None.
* Return         : None.
*******************************************************************************/
void Enter_LowPowerMode(void)
{
  /* Set the device state to suspend */
  bDeviceState = SUSPENDED;
}

/*******************************************************************************
* Function Name  : Leave_LowPowerMode
* Description    : Restores system clocks and power while exiting suspend mode
* Input          : None.
* Return         : None.
*******************************************************************************/
void Leave_LowPowerMode(void)
{
  DEVICE_INFO *pInfo = &Device_Info;

  /* Set the device state to the correct state */
  if (pInfo->Current_Configuration != 0)
  {
    /* Device configured */
    bDeviceState = CONFIGURED;
  }
  else
  {
    bDeviceState = ATTACHED;
  }
}

/*******************************************************************************
* Function Name  : USB_Interrupts_Config
* Description    : Configures the USB interrupts
* Input          : None.
* Return         : None.
*******************************************************************************/
void USB_Interrupts_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );

  NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 16;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 16;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

}

/*******************************************************************************
* Function Name  : USB_Cable_Config
* Description    : Software Connection/Disconnection of USB Cable
* Input          : None.
* Return         : Status
*******************************************************************************/
void USB_Cable_Config (FunctionalState NewState)
{
  if (NewState != DISABLE)
  {
    ;//GPIO_ResetBits(USB_DISCONNECT, USB_DISCONNECT_PIN);
  }
  else
  {
    ;//GPIO_SetBits(USB_DISCONNECT, USB_DISCONNECT_PIN);
  }
}

/*******************************************************************************
* Function Name  : Handle_USBAsynchXfer.
* Description    : send data to USB.
* Input          : None.
* Return         : none.
*******************************************************************************/
void Handle_USBAsynchXfer (void)
{
    /*
  
  uint16_t USB_Tx_ptr;
  uint16_t USB_Tx_length;


    if (USART_Rx_ptr_out == USART_RX_DATA_SIZE)
    {
      USART_Rx_ptr_out = 0;
    }

    if(USART_Rx_ptr_out == USART_Rx_ptr_in)
    {
      USB_Tx_State = 0;
      return;
    }

    if(USART_Rx_ptr_out > USART_Rx_ptr_in) // rollback
    {
      USART_Rx_length = USART_RX_DATA_SIZE - USART_Rx_ptr_out;
    }
    else
    {
      USART_Rx_length = USART_Rx_ptr_in - USART_Rx_ptr_out;
    }

    if (USART_Rx_length > VIRTUAL_COM_PORT_DATA_SIZE)
    {
      USB_Tx_ptr = USART_Rx_ptr_out;
      USB_Tx_length = VIRTUAL_COM_PORT_DATA_SIZE;

      USART_Rx_ptr_out += VIRTUAL_COM_PORT_DATA_SIZE;
      USART_Rx_length -= VIRTUAL_COM_PORT_DATA_SIZE;
    }
    else
    {
      USB_Tx_ptr = USART_Rx_ptr_out;
      USB_Tx_length = USART_Rx_length;

      USART_Rx_ptr_out += USART_Rx_length;
      USART_Rx_length = 0;
    }
    USB_Tx_State = 1;


        UserToPMABufferCopy(&USART_Rx_Buffer[USB_Tx_ptr], ENDP1_TXADDR, USB_Tx_length);
        SetEPTxCount(ENDP1, USB_Tx_length);
        SetEPTxValid(ENDP1);
        */
    xQueueHandle q = fromMcu();
    portBASE_TYPE cr = pdFALSE;
    uint8_t i = 0;
    while ( crQUEUE_RECEIVE_FROM_ISR( q, &(USART_Rx_Buffer[i]), &cr ) == pdTRUE )
    {
        //USART_Rx_Buffer[i] = 'a';
        i++;
    }
    //setRed( i & 1 );
    //setGreen( i & 2 );
 
    UserToPMABufferCopy( USART_Rx_Buffer, ENDP1_TXADDR, i );
    SetEPTxCount( ENDP1, i );
    SetEPTxValid( ENDP1 );

}
/*******************************************************************************
* Function Name  : UART_To_USB_Send_Data.
* Description    : send the received data from UART 0 to USB.
* Input          : None.
* Return         : none.
*******************************************************************************/
void USB_Send_Data(uint8_t data)
{
  
  USART_Rx_Buffer[USART_Rx_ptr_in] = data;
  USART_Rx_ptr_in++;

  /* To avoid buffer overflow */
  if(USART_Rx_ptr_in >= USART_RX_DATA_SIZE)
  {
    USART_Rx_ptr_in = 0;
  }
}

/*******************************************************************************
* Function Name  : Get_SerialNum.
* Description    : Create the serial number string descriptor.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Get_SerialNum(void)
{
  uint32_t Device_Serial0, Device_Serial1, Device_Serial2;

  Device_Serial0 = *(__IO uint32_t*)(0x1FFFF7E8);
  Device_Serial1 = *(__IO uint32_t*)(0x1FFFF7EC);
  Device_Serial2 = *(__IO uint32_t*)(0x1FFFF7F0);

  Device_Serial0 += Device_Serial2;

  if (Device_Serial0 != 0)
  {
    IntToUnicode (Device_Serial0, &Virtual_Com_Port_StringSerial[2] , 8);
    IntToUnicode (Device_Serial1, &Virtual_Com_Port_StringSerial[18], 4);
  }
}

/*******************************************************************************
* Function Name  : HexToChar.
* Description    : Convert Hex 32Bits value into char.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
static void IntToUnicode (uint32_t value , uint8_t *pbuf , uint8_t len)
{
  uint8_t idx = 0;
  
  for( idx = 0 ; idx < len ; idx ++)
  {
    if( ((value >> 28)) < 0xA )
    {
      pbuf[ 2* idx] = (value >> 28) + '0';
    }
    else
    {
      pbuf[2* idx] = (value >> 28) + 'A' - 10; 
    }
    
    value = value << 4;
    
    pbuf[ 2* idx + 1] = 0;
  }
}

void USB_SetLeds(uint8_t LED_Command) {
	/*switch (LED_Command) {
	case 'A': {
	    GPIO_SetBits(GPIOF, GPIO_Pin_6);
	    break;
	}
	case 'B': {
	    GPIO_SetBits(GPIOF, GPIO_Pin_7);
	    break;
	}
	case 'C': {
	    GPIO_SetBits(GPIOF, GPIO_Pin_8);
	    break;
	}
	case 'D': {
	    GPIO_SetBits(GPIOF, GPIO_Pin_9);
	    break;
	}
	case 'a': {
	    GPIO_ResetBits(GPIOF, GPIO_Pin_6);
	    break;
	}
	case 'b': {
	    GPIO_ResetBits(GPIOF, GPIO_Pin_7);
	    break;
	}
	case 'c': {
	    GPIO_ResetBits(GPIOF, GPIO_Pin_8);
	    break;
	}
	case 'd': {
	    GPIO_ResetBits(GPIOF, GPIO_Pin_9);
	    break;
	}
	case '1': {
	    GPIO_SetBits(GPIOF, GPIO_Pin_6 | GPIO_Pin_7 |GPIO_Pin_8 |GPIO_Pin_9);
	    break;
	}
	case '0': {
	    GPIO_ResetBits(GPIOF, GPIO_Pin_6 | GPIO_Pin_7 |GPIO_Pin_8 |GPIO_Pin_9);
	    break;
	}
	}*/
}
/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
