
#ifndef __I2C_LIB_H_
#define __I2C_LIB_H_

void I2C_Configuration(void);
void I2C_EE_ByteWrite(uint8_t val, uint16_t WriteAddr);
uint8_t I2C_EE_ByteRead( uint16_t ReadAddr);
void Delay_ms(uint32_t ms);

#endif





