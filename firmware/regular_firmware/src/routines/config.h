
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
#define I2C_BUSY           2
#define I2C_ERROR_BUSY     3
#define I2C_MMS            4
#define I2C_ERROR_MMS      5
#define I2C_TMS            6
#define I2C_ERROR_TMS      7
#define I2C_MBT            8
#define I2C_ERROR_MBT      9
#define I2C_BTF            10
#define I2C_ERROR_BTF      11
#define I2C_MMS_R          12
#define I2C_ERROR_MMS_R    13
#define I2C_RMS            14
#define I2C_ERROR_RMS      15
#define I2C_MBR            16
#define I2C_ERROR_MBR      17

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
#define FUNC_I2C_TIMEOUT    15
#define FUNC_I2C_BYTES_WR   16
#define FUNC_I2C_BYTES_RD   17
#define FUNC_I2C_WR_QUEUE   18

#endif

