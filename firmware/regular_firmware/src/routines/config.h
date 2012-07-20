
#ifndef __CONFIG_H_
#define __CONFIG_H_

#define VERSION        1234

#define IN_QUEUE_SIZE  64
#define OUT_QUEUE_SIZE 64

#define STATE_RESET_TIMEOUT 50


#define I2C_QUEUE_SIZE 64
// Idle command or status.
#define I2C_IDLE       0
// I2C commands.
#define I2C_MASTER_SEND    1
#define I2C_MASTER_RECEIVE 2
#define I2C_SLAVE_SEND     3
#define I2C_SLAVE_RECEIVE  4
// I2C possible status
#define I2C_IO_INVOKED     1
#define I2C_MASTER_IO      2
#define I2C_ENTERED_IO     3
#define I2C_STARTED        4
#define I2C_ERROR          5
#define I2C_WAIT_BUSY      6

#define CMD_DATA 0
#define CMD_FUNC 1
#define CMD_BUFFER_SIZE 16

// Funcs invocation commands
#define FUNC_IDLE           0
#define FUNC_VERSION        1
// GPIO
#define FUNC_GPIO_EN        2
#define FUNC_GPIO_CONFIG    3
#define FUNC_GPIO_SET       4
#define FUNC_GPIO           5
// I2C
#define FUNC_I2C_STATUS     10
#define FUNC_I2C_EN         11
#define FUNC_I2C_CONFIG     12
#define FUNC_I2C_IO         13
#define FUNC_I2C_RESULT     14

#endif

