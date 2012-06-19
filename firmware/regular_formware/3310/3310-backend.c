
#include "stm32f10x.h"
#include "3310-backend.h"

#define SPI_3310  SPI1

#define PORT_CS   GPIOA
#define PIN_CS    GPIO_Pin_4

#define PORT_DC   GPIOA
#define PIN_DC    GPIO_Pin_6

#define PORT_RST  GPIOA
#define PIN_RST   GPIO_Pin_3

#define PORT_PWR GPIOB
#define PIN_PWR  GPIO_Pin_10

#define PIN_SCK  GPIO_Pin_5
#define PIN_MOSI GPIO_Pin_7

#define DC_LOW()    GPIO_ResetBits( PORT_DC, PIN_DC )
#define DC_HIGH()   GPIO_SetBits(   PORT_DC, PIN_DC )
#define CS_LOW()    GPIO_ResetBits( PORT_DC, PIN_CS )
#define CS_HIGH()   GPIO_SetBits(   PORT_DC, PIN_CS )
#define RST_LOW()   GPIO_ResetBits( PORT_RST, PIN_RST )
#define RST_HIGH()  GPIO_SetBits(   PORT_RST, PIN_RST )
#define PWR_LOW()   GPIO_ResetBits( PORT_PWR, PIN_PWR )
#define PWR_HIGH()  GPIO_SetBits(   PORT_PWR, PIN_PWR )

#define DMA_CHANNEL_3310 DMA1_Channel3
#define DMA_FLAG_3310    DMA1_FLAG_TC3
#define DMA_FLAG_EN_3310 DMA1_FLAG_TE3
#define DMA_CLK_3310     RCC_AHBPeriph_DMA1
#define DMA_3310         DMA1

BYTE g_dmaStarted = 0;

void delay3310( int cnt )
{
    volatile int i;
    for ( i=0; i<cnt; i++ )
        ;
}

static void initDma( BYTE * mem, int cnt );

void init3310( BYTE * data, int cnt )
{
    GPIO_SetBits( PORT_CS, PIN_CS | PIN_DC | PIN_RST );
    GPIO_ResetBits( PORT_PWR, PIN_PWR );

    GPIO_InitTypeDef GPIO_InitStructure;
    
    // Enable SPI1 and GPIO clocks */
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_SPI1,  ENABLE );
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE );
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE );
    
    GPIO_InitStructure.GPIO_Pin = PIN_CS | PIN_DC | PIN_RST;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init( PORT_CS, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = PIN_PWR;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init( PORT_PWR, &GPIO_InitStructure);
 
    /* Configure SPI1 pins: SCK and MOSI only to their Alternative (SPI) function */
    GPIO_InitStructure.GPIO_Pin = PIN_SCK | PIN_MOSI;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( PORT_CS, &GPIO_InitStructure );
 
    // To prevent powering from data lines.
    RST_HIGH();
    CS_HIGH();
    DC_HIGH();
    // Turn power on.
    PWR_HIGH();
    delay3310( 100000 );

   /* SPI1 configuration */
    SPI_InitTypeDef  SPI_InitStructure;
    SPI_InitStructure.SPI_Direction         = SPI_Direction_1Line_Tx;
    SPI_InitStructure.SPI_Mode              = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize          = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL              = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA              = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS               = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
    SPI_InitStructure.SPI_FirstBit          = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial     = 7;
    SPI_Init( SPI_3310, &SPI_InitStructure );
    /* Enable SPI1 */
    SPI_Cmd( SPI_3310, ENABLE );
    //SPI_SSOutputCmd( SPI_3310, ENABLE );

    //initDma( data, cnt );
}

static void initDma( BYTE * mem, int cnt )
{
    uint32_t addr = (uint32_t)&(SPI1->DR);
    RCC_AHBPeriphClockCmd(  DMA_CLK_3310, ENABLE );

    DMA_InitTypeDef  DMA_InitStructure;
    DMA_DeInit( DMA_CHANNEL_3310 );
    DMA_InitStructure.DMA_PeripheralBaseAddr = addr;
    DMA_InitStructure.DMA_MemoryBaseAddr     = (uint32_t)mem;
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize         = cnt;
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;
    DMA_Init( DMA_CHANNEL_3310, &DMA_InitStructure );

    /* Enable SPI_MASTER DMA Tx request */
    SPI_I2S_DMACmd( SPI_3310, SPI_I2S_DMAReq_Tx, ENABLE );
}

void finit3310( void )
{
    CS_HIGH();
    RST_HIGH();
    DC_HIGH();
    SPI_Cmd( SPI_3310, DISABLE );
    delay3310( 100000 );
}

/** * @brief Sends a byte through the SPI interface and return the byte
    * received from the SPI bus. 
    * @param byte : byte to send. 
    * @retval : The value of the received byte. 
    */ 
void sendByte3310( BYTE byte )
{
    //SPI_I2S_ReceiveData( SPI_3310 );
    /* Loop while DR register in not emplty */
    while ( SPI_I2S_GetFlagStatus( SPI_3310, SPI_I2S_FLAG_BSY ) == SET )
        ;
    /* Send byte through the SPI_3310 peripheral */
    SPI_I2S_SendData( SPI_3310, byte );
    /* Wait to receive a byte */
    //while ( SPI_I2S_GetFlagStatus( SPI_3310, SPI_I2S_FLAG_RXNE ) == RESET )
    //    ;
    /* Return the byte read from the SPI bus */
    //return SPI_I2S_ReceiveData( SPI_3310 );
}

void sendArray3310( BYTE * data, int cnt )
{
   /* SPI1 configuration */
    /*SPI_Cmd( SPI_3310, DISABLE );
    SPI_InitTypeDef  SPI_InitStructure;
    SPI_InitStructure.SPI_Direction         = SPI_Direction_1Line_Tx;
    SPI_InitStructure.SPI_Mode              = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize          = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL              = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA              = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS               = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
    SPI_InitStructure.SPI_FirstBit          = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial     = 7;
    SPI_Init( SPI_3310, &SPI_InitStructure );*/
    /* Enable SPI1 */

    uint32_t addr = (uint32_t)&(SPI1->DR);
    //RCC_AHBPeriphClockCmd(  DMA_CLK_3310, ENABLE );
    DMA_Cmd( DMA_CHANNEL_3310, DISABLE );
    DMA_InitTypeDef  DMA_InitStructure;
    DMA_DeInit( DMA_CHANNEL_3310 );
    DMA_InitStructure.DMA_PeripheralBaseAddr = addr;
    DMA_InitStructure.DMA_MemoryBaseAddr     = (uint32_t)data;
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize         = cnt;
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_Medium;
    DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;
    DMA_Init( DMA_CHANNEL_3310, &DMA_InitStructure );

    /* Enable SPI_MASTER DMA Tx request */
    SPI_I2S_DMACmd( SPI_3310, SPI_I2S_DMAReq_Tx, ENABLE );
    //SPI_Cmd( SPI_3310, ENABLE );
    DMA_Cmd( DMA_CHANNEL_3310, ENABLE );
    // DMA in profress flag.
    g_dmaStarted = 1;
}

void waitArrayReady3310( void )
{
    // Also there should be no SPI interaction at the same time.
    // Loop while DR register in not emplty
    while ( SPI_I2S_GetFlagStatus( SPI_3310, SPI_I2S_FLAG_BSY ) == SET )
        ;
    // If not started return.
    if ( !g_dmaStarted )
        return;
   // If started, wait till ready.
    int i = 0;
    while ( DMA_GetFlagStatus( DMA_FLAG_3310 ) != SET )
        i++;
    SPI_I2S_DMACmd( SPI_3310, SPI_I2S_DMAReq_Tx, DISABLE );
    // Clear DMA in progress flag.
    g_dmaStarted = 0;
}

void setModeCmd3310( void )
{
    while ( SPI_I2S_GetFlagStatus( SPI_3310, SPI_I2S_FLAG_BSY ) == SET )
        ;
    DC_LOW();
}

void setModeData3310( void )
{
    while ( SPI_I2S_GetFlagStatus( SPI_3310, SPI_I2S_FLAG_BSY ) == SET )
        ;
    DC_HIGH();
}


void dcLow(void)
{
    DC_LOW();
}

void dcHigh(void)
{
    DC_HIGH();
}

void csLow(void)
{
    CS_LOW();
}

void csHigh(void)
{
    CS_HIGH();
}

void rstLow(void)
{
    RST_LOW();
}

void rstHigh(void)
{
    RST_HIGH();
}

void pwrLow(void)
{
    PWR_LOW();
}

void pwrHigh(void)
{
    PWR_HIGH();
}









