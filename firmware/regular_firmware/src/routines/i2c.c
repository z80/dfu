
#include "i2c.h"

TI2C g_i2c[2];

TI2C * i2c( uint8_t index )
{
    return &(g_i2c[index]);
}

uint8_t i2cStatus( uint8_t index )
{
    TI2C * i2c = &(g_i2c[index]);
    return i2c->status;
}

void i2cInit( uint8_t index )
{
    TI2C * i2c = &(g_i2c[index]);
    //i2c->sendQueue    = xQueueCreate( I2C_QUEUE_SIZE, 1 );
    //i2c->receiveQueue = xQueueCreate( I2C_QUEUE_SIZE, 1 );
    i2c->sendCnt    = 0;
    i2c->receiveCnt = 0;
    i2c->master  = 0;
    i2c->status  = I2C_IDLE;
    
    i2c->i2c     = ( index == 0 ) ? I2C1 : I2C2;
    i2c->address = 0;
    i2c->speed   = 100000;
    i2c->timeout = 128;
}

void i2cSetEn( uint8_t index, uint8_t en )
{
    I2C_TypeDef * i2c = ( index == 0 ) ? I2C1 : I2C2;
    uint32_t      periph = ( index == 0 ) ? RCC_APB1Periph_I2C1 : RCC_APB1Periph_I2C2;
    uint16_t      sckPin = ( index == 0 ) ? GPIO_Pin_6 : GPIO_Pin_10;
    uint16_t      sdaPin = ( index == 0 ) ? GPIO_Pin_7 : GPIO_Pin_11;
    if ( en )
    {
        GPIO_InitTypeDef  gpio;
        
        RCC_APB2PeriphClockCmd( RCC_I2C_GPIO_CLK, ENABLE );
        // HDW_I2C Periph clock enable
        RCC_APB1PeriphClockCmd( periph, ENABLE );
        
        // Configure HDW_I2C pins: SCL and SDA
        gpio.GPIO_Pin   = sckPin | sdaPin;
        gpio.GPIO_Speed = GPIO_Speed_50MHz;
        gpio.GPIO_Mode  = GPIO_Mode_AF_OD;
        GPIO_Init( I2C_PORT, &gpio );
    }
    else
    {
        I2C_DeInit( i2c );
        I2C_Cmd( i2c, DISABLE );
        // Because there are at least two of them don't turn off clock.
        RCC_APB1PeriphClockCmd( periph, DISABLE );
    }

}

void i2cConfig( uint8_t index, uint8_t master, uint8_t address, uint32_t speed )
{
    TI2C * i2c = &(g_i2c[index]);
    i2c->master  = master;
    i2c->address = address;
    i2c->speed   = speed;
}

uint8_t * i2cSendQueue( uint8_t index )
{
    TI2C * i2c = &(g_i2c[index]);
    return i2c->sendQueue;
}

uint8_t * i2cReceiveQueue( uint8_t index )
{
    TI2C * i2c = &(g_i2c[index]);
    return i2c->receiveQueue;
}

void i2cIo( uint8_t index, uint8_t address, uint8_t sendCnt, uint8_t receiveCnt, uint8_t * sendData )
{
    uint8_t i;
    TI2C * idc = i2c( index );
    uint8_t * d = i2cSendQueue( index );
    for ( i=0; i<sendCnt; i++ )
        d[i] = sendData[i];
    idc->address    = address;
    idc->sendCnt    = sendCnt;
    idc->receiveCnt = receiveCnt;
    idc->status     = I2C_STARTED;
}








/*
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
    I2C_GenerateSTART( HDW_I2C, ENABLE);
  
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
    while (!I2C_CheckEvent( HDW_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED ) )
    {
        if( ( timeout-- ) == 0 )
            return 3;
    }
  
    // Transmit the first address for r/w operations
    I2C_SendData( HDW_I2C, value );
  
    // Test on TXE FLag (data sent)
    timeout = g_i2cTimeout;
    while ( ( !I2C_GetFlagStatus( HDW_I2C, I2C_FLAG_TXE ) ) && ( !I2C_GetFlagStatus( HDW_I2C, I2C_FLAG_BTF ) ) )  
    {
        if ( ( timeout--) == 0 )
          return 4;
    }
  
  
    // Wait until BTF Flag is set before generating STOP
    timeout = g_i2cTimeout;
    while ( I2C_GetFlagStatus( HDW_I2C, I2C_FLAG_BTF ) )  
    {
        if ( ( timeout-- ) == 0 )
            return 5;
    }
  
    // Send STOP Condition
    I2C_GenerateSTOP( HDW_I2C, ENABLE );
  
    return 0;
}

static uint8_t submitAddress( uint16_t address )
{
    uint32_t timeout = g_i2cTimeout;

    // Clear the LM75_I2C AF flag
    I2C_ClearFlag( HDW_I2C, I2C_FLAG_AF );

    // Enable LM75_I2C acknowledgement if it is already disabled by other function
    I2C_AcknowledgeConfig( HDW_I2C, ENABLE );

    //---------------------------- Transmission Phase ---------------------------

    // Send LM75_I2C START condition
    I2C_GenerateSTART( HDW_I2C, ENABLE );

    // Test on LM75_I2C EV5 and clear it
    while ( ( !I2C_GetFlagStatus( HDW_I2C, I2C_FLAG_SB ) ) && timeout )  // EV5
    {
        timeout--;
    }
    if ( timeout == 0 )
    {
        return 1;
    }
      
    timeout = g_i2cTimeout;

    // Send STLM75 slave address for write
    I2C_Send7bitAddress( HDW_I2C, address, I2C_Direction_Transmitter );

    while ( ( !I2C_CheckEvent( HDW_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED ) ) && timeout ) // EV6
    {
        timeout--;
    }

    if ((I2C_GetFlagStatus( HDW_I2C, I2C_FLAG_AF) != 0x00) || (timeout == 0 ) )
    {
        return 2;
    }
    return 0;
}
*/








