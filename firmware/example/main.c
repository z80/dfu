/*******************************************************************************
* File Name          : main.c
* Author             : Martin Thomas, main-skeleton based on code from the
*                      STMicroelectronics MCD Application Team
* Version            : see VERSION_STRING below
* Date               : see VERSION_STRING below
* Description        : Main program body for the SD-Card tests
********************************************************************************
* License: 3BSD
*******************************************************************************/

#define VERSION_STRING "V1.2.1 7/2010"

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "stm32f10x.h"
#include "platform_config.h"
#include "main.h"
#include "comm.h"
#include "rtc.h"
#include "diskio.h" /* disk_timerproc */
#include "ff_test_term.h"
#include "misc_test_term.h"


/* Private typedef -----------------------------------------------------------*/
typedef enum { APPSTATE_FF_TERM, APPSTATE_TESTMENU } AppState;

/* Private variables ---------------------------------------------------------*/
USART_InitTypeDef USART_InitStructure;

const char welcome[] = "\r\nHello from a STM32 Demo-Application, M. Thomas\r\n" VERSION_STRING"\r\n";
const char gimmick[] =
"  _____ _______ __  __ ____ ___  ______ __  ___\r\n"\
" / ____|__   __|  \\/  |___ \\__ \\|  ____/_ |/ _ \\\r\n"\
"| (___    | |  | \\  / | __) | ) | |__   | | | | |_  __\r\n"\
" \\___ \\   | |  | |\\/| ||__ < / /|  __|  | | | | \\ \\/ /\r\n"\
" ____) |  | |  | |  | |___) / /_| |     | | |_| |>  <\r\n"\
"|_____/   |_|  |_|  |_|____/____|_|     |_|\\___//_/\\_\\\r\n";


/* Private function prototypes -----------------------------------------------*/
void Periph_Configuration(void);
void GPIO_Configuration(void);
void NVIC_Configuration(void);

void stdio_tests(void);
void dcc_tests(void);

/* Public functions -- -------------------------------------------------------*/

/*******************************************************************************
* Function Name  : main_systick_action
* Description    : operations to be done every 1ms
* Input          : None
* Output         : None
* Return         : None
*  overrides weak SysTick_Handler in startup_stm32*.c
*  When a RAMFUNC every function called from inside the ISR must be
*  reachable. This can be achieved by using compiler-option -mlong-calls
*  which globally enables long-calls. Here this option has not been used
*  instead the unreachable functions GPIO_Set/ResetBits have been replaced
*  by direct register-writes and disk_timerproc has also been attributed
*  as RAMFUNC to be reachable.
*******************************************************************************/
RAMFUNC void SysTick_Handler(void)
{
	static uint16_t cnt=0;
	static uint8_t flip=0, cntdiskio=0;

	cnt++;
	if( cnt >= 500 ) {
		cnt = 0;
		/* alive sign */
		if ( flip ) {
			// GPIO_SetBits(GPIO_LED, GPIO_Pin_LED2 );
			GPIO_LED->BSRR = GPIO_Pin_LED2;
		} else {
			// GPIO_ResetBits(GPIO_LED, GPIO_Pin_LED2 );
			GPIO_LED->BRR = GPIO_Pin_LED2;
		}
		flip = !flip;
	}

	cntdiskio++;
	if ( cntdiskio >= 10 ) {
		cntdiskio = 0;
		disk_timerproc(); /* to be called every 10ms */
	}

	ff_test_term_timerproc(); /* to be called every ms */
}

/*******************************************************************************
* Function Name  : main
* Description    : Main program
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int main(void)
{
	AppState appState = APPSTATE_FF_TERM;

	/* System Clocks Configuration */
	Periph_Configuration();

	/* NVIC configuration */
	NVIC_Configuration();

	/* Configure the GPIO ports */
	GPIO_Configuration();

	/* Turn on/off LED(s) */
	GPIO_SetBits(GPIO_LED, GPIO_Pin_LED1 /*| GPIO_Pin_LED4*/);

	/* Setup SysTick Timer for 1 millisecond interrupts, also enables Systick and Systick-Interrupt */
	if (SysTick_Config(SystemCoreClock / 1000))
	{
		/* Capture error */
		while (1);
	}

	/* USART1 and USART2 configuration ------------------------------------------------------*/
	/* USART and USART2 configured as follow:
	 - BaudRate = 19200 baud
	 - Word Length = 8 Bits
	 - One Stop Bit
	 - No parity
	 - Hardware flow control disabled (RTS and CTS signals)
	 - Receive and transmit enabled
	*/
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl =USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	/* Configure USART1 */
	USART_Init(USART1, &USART_InitStructure);
	/* Configure USART2 */
	USART_Init(USART2, &USART_InitStructure);

	/* Enable the USART1 */
	USART_Cmd(USART1, ENABLE);
	/* Enable the USART2 */
	USART_Cmd(USART2, ENABLE);
	
	comm_puts(welcome);
	comm_puts(gimmick);

#if STDIO_TEST
	stdio_tests();
#endif

#if DCC_TEST
	dcc_tests();
#endif

	rtc_init();

	while (1)
	{
		switch ( appState ) {
		case APPSTATE_FF_TERM:
			/* ff_test_term is not reentrant, blocks until exit */
			if ( !ff_test_term() ) {
				appState = APPSTATE_TESTMENU;
			}
			break;
		case APPSTATE_TESTMENU:
			/* misc_test_term is a state-machine, main-loop keeps running
			 * but may be throttled by time-consuming routines */
			if ( !misc_test_term() ) {
				appState = APPSTATE_FF_TERM;
			}
			break;
		default:
			appState = APPSTATE_TESTMENU;
			break;
		}
	}
}

/*******************************************************************************
* Function Name  : PeriphConfiguration
* Description    : Configures the different system clocks.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Periph_Configuration(void)
{
  /* Enable USART1, GPIOA, GPIOD and AFIO clocks */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA |RCC_APB2Periph_GPIOx
                         | RCC_APB2Periph_AFIO, ENABLE);
  /* Enable USART2 clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

  /* Enable GPIO_LED clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_LED, ENABLE);

  /* DMA1 clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
}

/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configures the different GPIO ports.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

#if defined(USE_STM3210B_EVAL) || defined(USE_EK_STM32F)
  /* Enable the USART2 Pins Software Remapping */
  GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);
#endif

  /* Configure USART1 TX (PA.09) as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Configure USART2 TX as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_TxPin;
  GPIO_Init(GPIOx, &GPIO_InitStructure);

  /* Configure USART1 RX (PA.10) as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Configure USART2 RX as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_RxPin;
  GPIO_Init(GPIOx, &GPIO_InitStructure);

  /* Configure GPIO for LEDs as Output push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_LED1 | GPIO_Pin_LED2 | GPIO_Pin_LED3 | GPIO_Pin_LED4;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIO_LED, &GPIO_InitStructure);

#if defined(USE_MINI_STM32)
  /* touch-controller's CS (PA4), SD-Card's CS (PB6) and DataFlash CS (PB7) high = unselect */
  /* PB6 and PB7 both have an external 4,7kOhm pull-up */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
#if 1
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
#else
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_SetBits(GPIOA, GPIO_Pin_4);
#endif
#endif /* defined(USE_MINI_STM32) */
}

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures Vector Table base location.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
#ifdef VECT_TAB_RAM
/* vector-offset (TBLOFF) from bottom of SRAM. defined in linker script */
extern uint32_t _isr_vectorsram_offs;
void NVIC_Configuration(void)
{
  /* Set the Vector Table base location at 0x20000000+_isr_vectorsram_offs */
  NVIC_SetVectorTable(NVIC_VectTab_RAM, (uint32_t)&_isr_vectorsram_offs);
}
#else
extern uint32_t _isr_vectorsflash_offs;
void NVIC_Configuration(void)
{
  /* Set the Vector Table base location at 0x08000000+_isr_vectorsflash_offs */
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, (uint32_t)&_isr_vectorsflash_offs);
}
#endif /* VECT_TAB_RAM */


/*******************************************************************************
* Function Name  : assert_failed
* Description    : called on failed assertion if compiled with USE_FULL_ASSERT
* Input          : pointers to char-arrays/strings: filename, function-name,
*                  line in file
* Output         : via xprintf
* Return         : None
*******************************************************************************/
#ifdef USE_FULL_ASSERT

#include "term_io.h"

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(const uint8_t* file, const uint8_t* function, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  xprintf("\n !!! Wrong parameter value detected\n");
  xprintf(" - file %s\n", file);
  xprintf(" - function %s\n", function);
  xprintf(" - line %lu\n", line);

#if 0
  /* Infinite loop */
  while (1)
  {
  }
#endif
}
#endif
