
#ifndef __I2C_H_
#define __I2C_H_

void i2cInit( void );
void i2cConfig( uint8_t host, uint16_t address );

#define HDW_I2C                         I2C1
#define HDW_I2C_CLK                     RCC_APB1Periph_I2C1
#define HDW_I2C_SCL_PIN                 GPIO_Pin_6                  // PB.06
#define HDW_I2C_SCL_GPIO_PORT           GPIOB                       // GPIOB
#define HDW_I2C_SCL_GPIO_CLK            RCC_APB2Periph_GPIOB
#define HDW_I2C_SDA_PIN                 GPIO_Pin_7                  // PB.07
#define HDW_I2C_SDA_GPIO_PORT           GPIOB                       // GPIOB
#define HDW_I2C_SDA_GPIO_CLK            RCC_APB2Periph_GPIOB
#define HDW_I2C_SMBUSALERT_PIN          GPIO_Pin_5                  // PB.05
#define HDW_I2C_SMBUSALERT_GPIO_PORT    GPIOB                       // GPIOB
#define HDW_I2C_SMBUSALERT_GPIO_CLK     RCC_APB2Periph_GPIOB
#define HDW_I2C_DR                      ((uint32_t)0x40005410)


#endif


