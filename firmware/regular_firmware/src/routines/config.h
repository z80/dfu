
#ifndef __CONFIG_H_
#define __CONFIG_H_

#define VERSION        1234

#define IN_QUEUE_SIZE  64
#define OUT_QUEUE_SIZE 64
#define STATE_RESET_TIMEOUT 10000

#define CMD_DATA 0
#define CMD_FUNC 1
#define CMD_BUFFER_SIZE 32

// Funcs invocation commands
#define FUNC_IDLE           0
#define FUNC_VERSION        1
// GPIO
#define FUNC_GPIO_CONFIG    2
#define FUNC_GPIO_SET       3
#define FUNC_GPIO           4


#endif

