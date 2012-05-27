

#ifndef __SPI_SD_LOWLEVEL_H_
#define __SPI_SD_LOWLEVEL_H_

#include "stm32f10x.h"

#define SD_SPI                           SPI2
#define SD_SPI_CLK                       RCC_APB1Periph_SPI2
#define SD_SPI_SCK_PIN                   GPIO_Pin_13                 /* PA.13 */
#define SD_SPI_SCK_GPIO_PORT             GPIOB                       /* GPIOB */
#define SD_SPI_SCK_GPIO_CLK              RCC_APB2Periph_GPIOB
#define SD_SPI_MISO_PIN                  GPIO_Pin_14                 /* PA.14 */
#define SD_SPI_MISO_GPIO_PORT            GPIOB                       /* GPIOB */
#define SD_SPI_MISO_GPIO_CLK             RCC_APB2Periph_GPIOB
#define SD_SPI_MOSI_PIN                  GPIO_Pin_15                 /* PA.15 */
#define SD_SPI_MOSI_GPIO_PORT            GPIOB                       /* GPIOB */
#define SD_SPI_MOSI_GPIO_CLK             RCC_APB2Periph_GPIOB
#define SD_CS_PIN                        GPIO_Pin_12                 /* PC.12 */
#define SD_CS_GPIO_PORT                  GPIOB                       /* GPIOB */
#define SD_CS_GPIO_CLK                   RCC_APB2Periph_GPIOB
//#define SD_DETECT_PIN                    GPIO_Pin_7                  /* PE.07 */
//#define SD_DETECT_GPIO_PORT              GPIOE                       /* GPIOE */
//#define SD_DETECT_GPIO_CLK               RCC_APB2Periph_GPIOE

void SD_LowLevel_DeInit( void );
void SD_LowLevel_Init( void ); 

#endif



