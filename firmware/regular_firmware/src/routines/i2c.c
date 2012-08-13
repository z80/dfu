
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
    i2c->master  = 1;
    i2c->status  = I2C_IDLE;

    i2c->i2c     = ( index == 0 ) ? I2C1 : I2C2;
    i2c->selfAddress = 0;
    i2c->address = 0;
    i2c->speed   = 100000;
    i2c->timeout = 16;

    i2c->bytesWritten = 0;
    i2c->bytesRead    = 0;

    i2c->slaveStopped = 0;
}

void i2cSetEn( uint8_t index, uint8_t en )
{
    TI2C * idc = &(g_i2c[index]);
    uint32_t      periph = ( index == 0 ) ? RCC_APB1Periph_I2C1 : RCC_APB1Periph_I2C2;
    uint16_t      sckPin = ( index == 0 ) ? GPIO_Pin_6 : GPIO_Pin_10;
    uint16_t      sdaPin = ( index == 0 ) ? GPIO_Pin_7 : GPIO_Pin_11;
    if ( en )
    {
        // HDW_I2C Periph clock enable
        RCC_APB1PeriphClockCmd( periph, ENABLE );
        GPIO_InitTypeDef  gpio;
        RCC_APB2PeriphClockCmd( RCC_I2C_GPIO_CLK, ENABLE );
        // Configure HDW_I2C pins: SCL and SDA
        gpio.GPIO_Pin   = sckPin | sdaPin;
        gpio.GPIO_Speed = GPIO_Speed_50MHz;
        gpio.GPIO_Mode  = GPIO_Mode_AF_OD;
        GPIO_Init( I2C_PORT, &gpio );

        I2C_DeInit( idc->i2c );
        // HDW_I2C Init
        I2C_InitTypeDef   I2C_InitStructure;
        I2C_InitStructure.I2C_Mode                = I2C_Mode_I2C; //( idc->master ) ? I2C_Mode_SMBusHost : I2C_Mode_SMBusDevice;
        I2C_InitStructure.I2C_DutyCycle           = I2C_DutyCycle_16_9;
        I2C_InitStructure.I2C_OwnAddress1         = idc->selfAddress;
        I2C_InitStructure.I2C_Ack                 = I2C_Ack_Enable;
        I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
        I2C_InitStructure.I2C_ClockSpeed          = idc->speed;
        I2C_Cmd( idc->i2c, ENABLE );
        I2C_Init( idc->i2c, &I2C_InitStructure );

        I2C_AcknowledgeConfig( idc->i2c, ENABLE );
        //I2C_GenerateSTOP( i2c, ENABLE );
        //if ( index == 0 )
        //	idc->status = 100;
    }
    else
    {
        I2C_DeInit( idc->i2c );
        I2C_Cmd( idc->i2c, DISABLE );
        // Because there are at least two of them don't turn off clock.
        RCC_APB1PeriphClockCmd( periph, DISABLE );
    }

}

void i2cConfig( uint8_t index, uint8_t master, uint8_t address, uint32_t speed )
{
    TI2C * idc = &(g_i2c[index]);
    idc->master  = master;
    idc->selfAddress = address;
    idc->speed   = speed;

    I2C_DeInit( idc->i2c );
    // HDW_I2C Init
    I2C_InitTypeDef   I2C_InitStructure;
    I2C_InitStructure.I2C_Mode                = I2C_Mode_I2C; //( idc->master ) ? I2C_Mode_SMBusHost : I2C_Mode_SMBusDevice;
    I2C_InitStructure.I2C_DutyCycle           = I2C_DutyCycle_16_9;
    I2C_InitStructure.I2C_OwnAddress1         = idc->selfAddress;
    I2C_InitStructure.I2C_Ack                 = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed          = idc->speed;
    I2C_Init( idc->i2c, &I2C_InitStructure );

    I2C_AcknowledgeConfig( idc->i2c, ENABLE );

	I2C_ITConfig( idc->i2c, I2C_IT_EVT, DISABLE );
	I2C_ITConfig( idc->i2c, I2C_IT_BUF, DISABLE );
	I2C_ITConfig( idc->i2c, I2C_IT_ERR, DISABLE );
	// For a particular I2C channel.
    NVIC_ClearIRQChannelPendingBit( I2C1_EV_IRQChannel );
    NVIC_ClearIRQChannelPendingBit( I2C1_ER_IRQChannel );
      if ( !idc->master )
    {
        NVIC_InitTypeDef NVIC_InitStructure;

        NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
        // For a particular I2C channel.
        NVIC_InitStructure.NVIC_IRQChannel = I2C1_EV_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 9;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);
        // For a particular I2C channel.
        NVIC_InitStructure.NVIC_IRQChannel = I2C1_ER_IRQn;
        NVIC_Init(&NVIC_InitStructure);

    	idc->slaveStopped = 0;
    	idc->status       = I2C_IDLE;
    	I2C_ITConfig( idc->i2c, I2C_IT_EVT, ENABLE );
    	I2C_ITConfig( idc->i2c, I2C_IT_BUF, ENABLE );
    	I2C_ITConfig( idc->i2c, I2C_IT_ERR, ENABLE );
    }
    //if ( ( index == 0 ) && (master == 1 ) && ( address == 0x00 ) )
    //	idc->status = 101;
}

void i2cSetTimeout( uint8_t index, uint32_t timeout )
{
	TI2C * idc = &(g_i2c[index]);
	idc->timeout = timeout;
}

uint8_t * i2cSendQueue( uint8_t index )
{
    TI2C * idc = &(g_i2c[index]);
    return idc->sendQueue;
}

uint8_t * i2cReceiveQueue( uint8_t index )
{
    TI2C * idc = &(g_i2c[index]);
    return idc->receiveQueue;
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
    idc->status     = I2C_IO_INVOKED;
    idc->bytesWritten = 0;
    idc->bytesRead    = 0;
}

uint8_t i2cBytesWritten( uint8_t index )
{
    TI2C * idc = i2c( index );
    return idc->bytesWritten;
}

uint8_t i2cBytesRead( uint8_t index )
{
    TI2C * idc = i2c( index );
    return idc->bytesRead;
}






void i2cIrqHandler( uint8_t index )
{
	TI2C * idc = i2c( index );
	uint32_t reason = I2C_GetLastEvent( idc->i2c );
    switch ( reason )
    {

    case I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED:
        idc->bytesRead = 0;
        idc->status    = I2C_SRAM;
        I2C_ClearFlag( idc->i2c, I2C_FLAG_ADDR );
        break;

    case I2C_EVENT_SLAVE_TRANSMITTER_ADDRESS_MATCHED:
        idc->bytesWritten = 0;
        idc->slaveStopped = 0;
        idc->status = I2C_STAM;
        I2C_SendData( idc->i2c, idc->sendQueue[ idc->bytesWritten++ ] );
        break;

    case I2C_EVENT_SLAVE_BYTE_RECEIVED:
        idc->receiveQueue[ idc->bytesRead++ ] = I2C_ReceiveData( idc->i2c );
        idc->status = I2C_SBR;
        break;

    case I2C_EVENT_SLAVE_BYTE_TRANSMITTED:
    	if ( !idc->slaveStopped )
    	{
            I2C_SendData( idc->i2c, idc->sendQueue[ idc->bytesWritten++ ] );
            idc->bytesWritten %= I2C_QUEUE_SIZE;
            idc->status = I2C_SBT;
    	}
        break;

    case I2C_EVENT_SLAVE_ACK_FAILURE:
    	idc->slaveStopped = 1;
    	idc->status       = I2C_SAF;
    	I2C_ClearFlag( idc->i2c, I2C_FLAG_AF );
    	break;

    case I2C_EVENT_SLAVE_STOP_DETECTED:
    	idc->slaveStopped = 1;
    	idc->status      = I2C_SSD;
        //if(I2C_GetFlagStatus( idc->i2c, I2C_FLAG_ADDR ) == SET )
        //    I2C_ClearFlag( idc->i2c, I2C_FLAG_ADDR );
        //if(I2C_GetFlagStatus( idc->i2c, I2C_FLAG_STOPF ) == SET )
        //    I2C_ClearFlag( idc->i2c, I2C_FLAG_STOPF );
    	I2C_ClearFlag( idc->i2c, I2C_FLAG_STOPF );
        break;
    }

   //I2C_CleanADDRandSTOPF();
}



void I2C1_EV_IRQHandler(void)
{
	i2cIrqHandler( 0 );
}

void I2C1_ER_IRQHandler(void)
{
	TI2C * idc = i2c( 0 );
	uint32_t reason = I2C_GetLastEvent( idc->i2c );
	reason = reason;
}

void I2C2_EV_IRQHandler(void)
{
	i2cIrqHandler( 1 );
}

void I2C2_ER_IRQHandler(void)
{

}




















/*
void I2C3_EV_IRQHandler()
{
    switch (I2C_GetLastEvent(I2C3))
    {

   case I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED:
       STM_EVAL_LEDOn(LED3);
       STM_EVAL_LEDOff(LED5);
       break;

   case I2C_EVENT_SLAVE_BYTE_RECEIVED:
       STM_EVAL_LEDToggle(LED4);
       STM_EVAL_LEDOff(LED3);
       I2C_InputBuffer[I2C_InputBufferIndex++] = I2C_ReceiveData(I2C3);
       break;

   case I2C_EVENT_SLAVE_STOP_DETECTED:
       STM_EVAL_LEDOn(LED6);
       STM_EVAL_LEDOff(LED4);
       break;
   }

   I2C_CleanADDRandSTOPF();

   if(I2C_InputBufferIndex > MOTOR_PACKAGE_SIZE-1)
   {
     motorHandleEvent(I2C_InputBuffer);
     I2C_InputBufferIndex = 0;
     uint8_t resetIndex;
     for(resetIndex = 0; resetIndex < MOTOR_PACKAGE_SIZE; resetIndex ++)
       I2C_InputBuffer[resetIndex] = 0;
   }
}

inline void I2C_CleanADDRandSTOPF()
{
 while ((I2C3->SR1 & I2C_SR1_ADDR) == I2C_SR1_ADDR)
 {
   volatile uint32_t temp;
   temp=I2C3->SR1;
   temp=I2C3->SR2;
 }
 while ((I2C3->SR1&I2C_SR1_STOPF) == I2C_SR1_STOPF)
 {
   volatile uint32_t temp;
   temp=I2C3->SR1;
   I2C3->CR1 |= 0x1;
 }
}
*/





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








