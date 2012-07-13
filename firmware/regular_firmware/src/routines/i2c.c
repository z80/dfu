
#include "i2c.h"

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







