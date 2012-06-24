
#ifndef __CONFIG_H_
#define __CONFIG_H_

#define IN_QUEUE_SIZE  64
#define OUT_QUEUE_SIZE 64
#define STATE_RESET_TIMEOUT 10000

#define CMD_DATA 0
#define CMD_FUNC 1
#define CMD_BUFFER_SIZE 32

// Funcs invocation commands
#define FUNC_IDLE         0
// GPIO
#define FUNC_GPIO_CONFIG  1
#define FUNC_GPIO_SET     2
#define FUNC_GPIO         3


#endif

