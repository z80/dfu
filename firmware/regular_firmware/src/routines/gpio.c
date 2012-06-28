
#include "gpio.h"

static GPIO_TypeDef * gpioPort( uint8_t index )
{
    GPIO_TypeDef * p;
    switch ( index )
    {
        case 0:
            p = GPIOA;
            break;
        case 1:
            p = GPIOB;
            break;
        case 2:
            p = GPIOC;
            break;
        case 3:
            p = GPIOD;
            break;
        default:
            p = GPIOA;
            break;
    }
    return p;
} 



void gpioEn( uint8_t index, uint8_t en )
{
    uint32_t portIndex;
    switch ( index )
    {
    case 0:
        portIndex = RCC_APB2Periph_GPIOA;
        break;
    case 1:
        portIndex = RCC_APB2Periph_GPIOB;
        break;
    case 2:
        portIndex = RCC_APB2Periph_GPIOC;
        break;
    case 3:
        portIndex = RCC_APB2Periph_GPIOD;
        break;
    default:
        portIndex = RCC_APB2Periph_GPIOA;
        break;
    }
    RCC_APB2PeriphClockCmd( portIndex, ( en ) ? ENABLE: DISABLE );
}

void gpioConfig( uint8_t port, uint16_t pins, uint8_t state )
{
    GPIO_InitTypeDef s;
    s.GPIO_Pin  = pins;
    s.GPIO_Mode = state;
    s.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_TypeDef * p = gpioPort( port );
    GPIO_Init( p, &s );
}

void gpioSet( uint8_t port, uint16_t pins, uint16_t vals )
{
    GPIO_TypeDef * p = gpioPort( port );
    GPIO_SetBits( p, vals & pins );
    GPIO_ResetBits( p, (~vals) & pins );
}

uint16_t gpio( uint8_t port )
{
    GPIO_TypeDef * p = gpioPort( port );
    uint16_t vals = GPIO_ReadInputData( p );
    return vals;
}



