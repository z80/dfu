
#include "i2c.h"

uint32_t g_i2cTimeout = 1024;

void i2cSetEn( uint8_t en )
{
    if ( en )
    {
        GPIO_InitTypeDef  GPIO_InitStructure;
        
        // HDW_I2C Periph clock enable
        RCC_APB1PeriphClockCmd( HDW_I2C_CLK, ENABLE);
        
        // HDW_I2C_SCL_GPIO_CLK, HDW_I2C_SDA_GPIO_CLK 
        //     and HDW_I2C_SMBUSALERT_GPIO_CLK Periph clock enable
        RCC_APB2PeriphClockCmd( HDW_I2C_SCL_GPIO_CLK | HDW_I2C_SDA_GPIO_CLK |
                                HDW_I2C_SMBUSALERT_GPIO_CLK, ENABLE );
        
        // Configure HDW_I2C pins: SCL
        GPIO_InitStructure.GPIO_Pin   = HDW_I2C_SCL_PIN;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_OD;
        GPIO_Init( HDW_I2C_SCL_GPIO_PORT, &GPIO_InitStructure );
        
        // Configure HDW_I2C pins: SDA
        GPIO_InitStructure.GPIO_Pin = HDW_I2C_SDA_PIN;
        GPIO_Init( HDW_I2C_SDA_GPIO_PORT, &GPIO_InitStructure );
        
        // Configure HDW_I2C pin: SMBUS ALERT
        GPIO_InitStructure.GPIO_Pin  = HDW_I2C_SMBUSALERT_PIN;
        // GPIO mode internal pullup.
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
        GPIO_Init( HDW_I2C_SMBUSALERT_GPIO_PORT, &GPIO_InitStructure );
    }
    else
    {
        I2C_DeInit( HDW_I2C );
        I2C_Cmd( HDW_I2C, DISABLE );
        RCC_APB1PeriphClockCmd( HDW_I2C_CLK, DISABLE );
    }
}


void i2cConfig( uint8_t host, uint16_t address, uint8_t _10bit, uint16_t speed )
{
    I2C_InitTypeDef   I2C_InitStructure;
    
    //LM75_LowLevel_Init();
    
    I2C_DeInit( HDW_I2C );
    
    // HDW_I2C Init
    I2C_InitStructure.I2C_Mode                = ( host ) ? I2C_Mode_SMBusHost : I2C_Mode_SMBusDevice;
    I2C_InitStructure.I2C_DutyCycle           = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1         = address;
    I2C_InitStructure.I2C_Ack                 = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = ( _10bit ) ? I2C_AcknowledgedAddress_10bit : I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed          = speed;
    I2C_Init( HDW_I2C, &I2C_InitStructure );
    
    // Enable SMBus Alert interrupt
    //I2C_ITConfig( HDW_I2C, I2C_IT_ERR, ENABLE );
    
    // HDW_I2C Init
    I2C_Cmd( HDW_I2C, ENABLE );
}

void i2cSetTimeout( uint32_t timeout )
{
    g_i2cTimeout = timeout;
}

static uint8_t submitAddress( uint16_t address );

uint8_t i2cSendByte( uint16_t address, uint8_t value )
{
    //uint8_t LM75_BufferTX[2] ={0,0};
    //LM75_BufferTX[0] = (uint8_t)(RegValue >> 8);
    //LM75_BufferTX[1] = (uint8_t)(RegValue);
  
    // Test on BUSY Flag
    uint32_t timeout = g_i2cTimeout;
    while ( I2C_GetFlagStatus( HDW_I2C, I2C_FLAG_BUSY ) )
    {
        if ( ( timeout-- ) == 0 )
            return 1;
    }
    
    // Enable the I2C peripheral
    I2C_GenerateSTART(LM75_I2C, ENABLE);
  
    // Test on SB Flag
    timeout = g_i2cTimeout;
    while ( I2C_GetFlagStatus( HDW_I2C, I2C_FLAG_SB ) == RESET ) 
    {
        if ( ( timeout--) == 0 )
            return 2;
    }
  
    // Transmit the slave address and enable writing operation
    I2C_Send7bitAddress( HDW_I2C, address, I2C_Direction_Transmitter );
  
    // Test on ADDR Flag
    timeout = g_i2cTimeout;
    while (!I2C_CheckEvent(LM75_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if( ( timeout-- ) == 0 )
            return 3;
    }
  
    // Transmit the first address for r/w operations
    I2C_SendData( HDW_I2C, RegName );
  
    // Test on TXE FLag (data sent)
    timeout = g_i2cTimeout;
    while ( ( !I2C_GetFlagStatus( HDW_I2C, I2C_FLAG_TXE ) ) && ( !I2C_GetFlagStatus( HDW_I2C, I2C_FLAG_BTF ) ) )  
    {
        if((LM75_Timeout--) == 0) return 2;
    }
  
    // Enable I2C DMA request
    I2C_DMACmd(LM75_I2C,ENABLE);
  
    // Enable DMA TX Channel
    DMA_Cmd(LM75_DMA_TX_CHANNEL, ENABLE);
  
    // Wait until DMA Transfer Complete
    LM75_Timeout = LM75_LONG_TIMEOUT;
    while (!DMA_GetFlagStatus(LM75_DMA_TX_TCFLAG))
    {
        if((LM75_Timeout--) == 0) return LM75_TIMEOUT_UserCallback();
    }  
  
    // Wait until BTF Flag is set before generating STOP
    LM75_Timeout = LM75_LONG_TIMEOUT;
    while (I2C_GetFlagStatus(LM75_I2C,I2C_FLAG_BTF))  
    {
        if((LM75_Timeout--) == 0) return LM75_TIMEOUT_UserCallback();
    }
  
    // Send STOP Condition
    I2C_GenerateSTOP(LM75_I2C, ENABLE);
  
    // Disable DMA TX Channel
    DMA_Cmd(LM75_DMA_TX_CHANNEL, DISABLE);
  
    // Disable I2C DMA request
    I2C_DMACmd(LM75_I2C,DISABLE);
  
    // Clear DMA TX Transfer Complete Flag
    DMA_ClearFlag(LM75_DMA_TX_TCFLAG);
  
    return LM75_OK;
}

static uint8_t submitAddress( uint16_t address )
{
    uint32_t I2C_TimeOut = g_i2cTimeout;

    // Clear the LM75_I2C AF flag
    I2C_ClearFlag( HDW_I2C, I2C_FLAG_AF );

    // Enable LM75_I2C acknowledgement if it is already disabled by other function
    I2C_AcknowledgeConfig( HDW_I2C, ENABLE );

    //---------------------------- Transmission Phase ---------------------------

    // Send LM75_I2C START condition
    I2C_GenerateSTART( HDW_I2C, ENABLE );

    // Test on LM75_I2C EV5 and clear it
    while ( ( !I2C_GetFlagStatus( HDW_I2C, I2C_FLAG_SB ) ) && I2C_TimeOut )  // EV5
    {
        I2C_TimeOut--;
    }
    if ( I2C_TimeOut == 0 )
    {
        return 1;
    }
      
    I2C_TimeOut = g_i2cTimeout;

    // Send STLM75 slave address for write
    I2C_Send7bitAddress( HDW_I2C, address, I2C_Direction_Transmitter );

    while ( ( !I2C_CheckEvent( HDW_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED ) ) && I2C_TimeOut ) // EV6
    {
        I2C_TimeOut--;
    }

    if ((I2C_GetFlagStatus(LM75_I2C, I2C_FLAG_AF) != 0x00) || (I2C_TimeOut == 0))
    {
        return 2;
    }
    return 0;
}









