
#include "spi_sd_lowlevel.h"

void SD_LowLevel_DeInit(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  SPI_Cmd(SD_SPI, DISABLE); /*!< SD_SPI disable */
  SPI_I2S_DeInit(SD_SPI);   /*!< DeInitializes the SD_SPI */
  
  /*!< SD_SPI Periph clock disable */
  RCC_APB2PeriphClockCmd(SD_SPI_CLK, DISABLE); 

  /*!< Configure SD_SPI pins: SCK */
  GPIO_InitStructure.GPIO_Pin = SD_SPI_SCK_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(SD_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure SD_SPI pins: MISO */
  GPIO_InitStructure.GPIO_Pin = SD_SPI_MISO_PIN;
  GPIO_Init(SD_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure SD_SPI pins: MOSI */
  GPIO_InitStructure.GPIO_Pin = SD_SPI_MOSI_PIN;
  GPIO_Init(SD_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure SD_SPI_CS_PIN pin: SD Card CS pin */
  GPIO_InitStructure.GPIO_Pin = SD_CS_PIN;
  GPIO_Init(SD_CS_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure SD_SPI_DETECT_PIN pin: SD Card detect pin */
  GPIO_InitStructure.GPIO_Pin = SD_DETECT_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(SD_DETECT_GPIO_PORT, &GPIO_InitStructure);
  GPIO_ResetBits( SD_DETECT_GPIO_PORT, SD_DETECT_PIN );
}




 #define CARD_SUPPLY_SWITCHABLE   1
 #define GPIO_PWR                 GPIOB
 #define RCC_APB2Periph_GPIO_PWR  RCC_APB2Periph_GPIOB
 #define GPIO_Pin_PWR             GPIO_Pin_11
 #define GPIO_Mode_PWR            GPIO_Mode_Out_PP
 #define SOCKET_WP_CONNECTED      0
 #define SOCKET_CP_CONNECTED      0
 #define SPI_SD                   SPI2
 #define GPIO_CS                  GPIOB
 #define RCC_APB2Periph_GPIO_CS   RCC_APB2Periph_GPIOB
 #define GPIO_Pin_CS              GPIO_Pin_12
 #define DMA_Channel_SPI_SD_RX    DMA1_Channel2
 #define DMA_Channel_SPI_SD_TX    DMA1_Channel3
 #define DMA_FLAG_SPI_SD_TC_RX    DMA1_FLAG_TC2
 #define DMA_FLAG_SPI_SD_TC_TX    DMA1_FLAG_TC3
 #define GPIO_SPI_SD              GPIOB
 #define GPIO_Pin_SPI_SD_SCK      GPIO_Pin_13
 #define GPIO_Pin_SPI_SD_MISO     GPIO_Pin_14
 #define GPIO_Pin_SPI_SD_MOSI     GPIO_Pin_15
 #define RCC_APBPeriphClockCmd_SPI_SD  RCC_APB1PeriphClockCmd
 #define RCC_APBPeriph_SPI_SD     RCC_APB1Periph_SPI2
 /* - for SPI1 and full-speed APB2: 72MHz/4 */
 #define SPI_BaudRatePrescaler_SPI_SD  SPI_BaudRatePrescaler_4



#define SELECT()        GPIO_ResetBits( SD_CS_GPIO_PORT, SD_CS_PIN )    /* MMC CS = L */
#define DESELECT()      GPIO_SetBits( SD_CS_GPIO_PORT, SD_CS_PIN )      /* MMC CS = H */


static void card_power( uint8_t on )		// switch FET for card-socket VCC
{
	GPIO_InitTypeDef GPIO_InitStructure;

	// Turn on GPIO for power-control pin connected to FET's gate
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_PWR, ENABLE);
	// Configure I/O for Power FET
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_PWR;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_PWR;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_PWR, &GPIO_InitStructure);
	if (on)
    {
		// Chip select internal pull-down (to avoid parasite powering)
		GPIO_InitStructure.GPIO_Pin |= GPIO_Pin_CS;
		GPIO_Init(GPIO_CS, &GPIO_InitStructure);
		GPIO_SetBits(GPIO_PWR, GPIO_Pin_PWR);
	}
    else
    {
		GPIO_ResetBits(GPIO_PWR, GPIO_Pin_PWR);
	}
}



/**
  * @brief  Initializes the SD_SPI and CS pins.
  * @param  None
  * @retval None
  */
void SD_LowLevel_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  SPI_InitTypeDef   SPI_InitStructure;

  //!< SD_SPI_CS_GPIO, SD_SPI_MOSI_GPIO, SD_SPI_MISO_GPIO, SD_SPI_DETECT_GPIO 
  //     and SD_SPI_SCK_GPIO Periph clock enable
  RCC_APB2PeriphClockCmd(SD_CS_GPIO_CLK | SD_SPI_MOSI_GPIO_CLK | SD_SPI_MISO_GPIO_CLK |
                         SD_SPI_SCK_GPIO_CLK | SD_DETECT_GPIO_CLK, ENABLE);

  //!< SD_SPI Periph clock enable
  RCC_APB2PeriphClockCmd(SD_SPI_CLK, ENABLE); 

  // Turn SPI off as well as card power.
  SPI_Cmd( SD_SPI, DISABLE ); //!< SD_SPI enable
  card_power( 0 ); 
  volatile uint32_t ttt;
  for ( ttt=0; ttt<300000; ttt++ )
      asm( "nop;" );
 
  //!< Configure SD_SPI pins: SCK
  GPIO_InitStructure.GPIO_Pin = SD_SPI_SCK_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(SD_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

  //!< Configure SD_SPI pins: MOSI
  GPIO_InitStructure.GPIO_Pin = SD_SPI_MOSI_PIN;
  GPIO_Init(SD_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

  //!< Configure SD_SPI pins: MISO
  GPIO_InitStructure.GPIO_Pin = SD_SPI_MISO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
  GPIO_Init(SD_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);
  
  //!< Configure SD_SPI_CS_PIN pin: SD Card CS pin
  GPIO_InitStructure.GPIO_Pin = SD_CS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(SD_CS_GPIO_PORT, &GPIO_InitStructure);

  //!< Configure SD_SPI_DETECT_PIN pin: SD Card detect pin
  GPIO_InitStructure.GPIO_Pin = SD_DETECT_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(SD_DETECT_GPIO_PORT, &GPIO_InitStructure);
  GPIO_SetBits( SD_DETECT_GPIO_PORT, SD_DETECT_PIN );

  //!< SD_SPI Config
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;

  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SD_SPI, &SPI_InitStructure);
  
  SPI_Cmd(SD_SPI, ENABLE); //!< SD_SPI enable
  
  card_power( 1 );

    /*
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	volatile uint8_t dummyread;

	// Enable GPIO clock for CS
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_CS, ENABLE);
	// Enable SPI clock, SPI1: APB2, SPI2: APB1
	RCC_APBPeriphClockCmd_SPI_SD(RCC_APBPeriph_SPI_SD, ENABLE);
    
    card_power(0);
	volatile int i;
    for ( i=0; i<300000; i++ )
        asm( "nop;" );
    card_power(1);

	//for (Timer1 = 25; Timer1; );	// Wait for 250ms
    for ( i=0; i<300000; i++ )
        asm( "nop;" );

	// Configure I/O for Flash Chip select
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_CS;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_CS, &GPIO_InitStructure);

	// De-select the Card: Chip Select high
	DESELECT();

	// Configure SPI pins: SCK and MOSI with default alternate function (not re-mapped) push-pull
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_SPI_SD_SCK | GPIO_Pin_SPI_SD_MOSI;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(GPIO_SPI_SD, &GPIO_InitStructure);
	// Configure MISO as Input with internal pull-up
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_SPI_SD_MISO;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
	GPIO_Init(GPIO_SPI_SD, &GPIO_InitStructure);

	// SPI configuration
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256; // 72000kHz/256=281kHz < 400kHz
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;

	SPI_Init(SPI_SD, &SPI_InitStructure);
	SPI_CalculateCRC(SPI_SD, DISABLE);
	SPI_Cmd(SPI_SD, ENABLE);

	// drain SPI
	while (SPI_I2S_GetFlagStatus(SPI_SD, SPI_I2S_FLAG_TXE) == RESET) { ; }
	dummyread = SPI_I2S_ReceiveData(SPI_SD);

    */
}




