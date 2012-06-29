
#ifndef __DEVICE_FUNCTIONS_H_
#define __DEVICE_FUNCTIONS_H_

// Low level functions.
#define CMD_DATA    0
#define CMD_FUNC    1

#define FUNC_VERSION        1
// GPIO
#define FUNC_GPIO_EN        2
#define FUNC_GPIO_CONFIG    3
#define FUNC_GPIO_SET       4
#define FUNC_GPIO           5

// TGpioMode 
#define GPIO_AF           0x00
#define GPIO_INF          0x04
#define GPIO_IPD          0x28
#define GPIO_IPU          0x48
#define GPIO_OOD          0x14
#define GPIO_OPP          0x10
#define GPIO_AFOD         0x1C
#define GPIO_AFPP         0x18



#endif
