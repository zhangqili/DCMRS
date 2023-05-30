/*
 * sgp30.c
 *
 *  Created on: May 27, 2023
 *      Author: xq123
 */
#include "sgp30.h"
#include "main.h"

#define HAL_OK 0x00U


sgp30_data_t sgp30_data;

/**
 * @brief   向SGP30发送一条指令(16bit)
 * @param   cmd SGP30指令
 * @retval  成功返回HAL_OK
*/
uint8_t sgp30_send_cmd(sgp30_cmd_t cmd)
{

    uint8_t cmd_buffer[2];
    cmd_buffer[0] = cmd >> 8;
    cmd_buffer[1] = cmd;
    struct bflb_i2c_msg_s msgs[1];
    msgs[0].addr = SGP30_ADDR_WRITE;
    msgs[0].flags = 0;
    msgs[0].buffer = cmd_buffer;
    msgs[0].length = 2;
    return bflb_i2c_transfer(i2c0, msgs, 1);
    //return HAL_I2C_Master_Transmit(&hi2c1, SGP30_ADDR_WRITE, (uint8_t*) cmd_buffer, 2, 0xFFFF);
}

/**
 * @brief   软复位SGP30
 * @param   none
 * @retval  成功返回HAL_OK
*/
int sgp30_soft_reset(void)
{
    uint8_t cmd = 0x06;
    struct bflb_i2c_msg_s msgs[1];
    msgs[0].addr = SGP30_ADDR_WRITE;
    msgs[0].flags = 0;
    msgs[0].buffer = &cmd;
    msgs[0].length = 2;
    return bflb_i2c_transfer(i2c0, msgs, 1);
    //return HAL_I2C_Master_Transmit(&hi2c1, 0x00, &cmd, 1, 0xFFFF);
}

/**
 * @brief   初始化SGP30空气质量测量模式
 * @param   none
 * @retval  成功返回0，失败返回-1
*/
int sgp30_init(void)
{
    board_i2c0_gpio_init();
    i2c0 = bflb_device_get_by_name("i2c0");
    bflb_i2c_init(i2c0, 100000);
    
    int status;

    status = sgp30_soft_reset();
    if (status != HAL_OK) {
        return -1;
    }

    bflb_mtimer_delay_ms(100);

    status = sgp30_send_cmd(INIT_AIR_QUALITY);

    bflb_mtimer_delay_ms(100);

    return status == 0 ? 0 : -1;
}

/**
 * @brief   初始化SGP30空气质量测量模式
 * @param   none
 * @retval  成功返回HAL_OK
*/
int sgp30_start(void)
{
    return sgp30_send_cmd(MEASURE_AIR_QUALITY);
}

uint8_t CheckCrc8(uint8_t* const message, uint8_t initial_value)
{
    uint8_t  remainder;     //余数
    uint8_t  i = 0, j = 0;  //循环变量

    /* 初始化 */
    remainder = initial_value;

    for(j = 0; j < 2;j++)
    {
        remainder ^= message[j];

        /* 从最高位开始依次计算  */
        for (i = 0; i < 8; i++)
        {
            if (remainder & 0x80)
            {
                remainder = (remainder << 1)^CRC8_POLYNOMIAL;
            }
            else
            {
                remainder = (remainder << 1);
            }
        }
    }

    /* 返回计算的CRC码 */
    return remainder;
}

/**
 * @brief   读取一次空气质量数据
 * @param   none
 * @retval  成功返回0，失败返回-1
*/
int sgp30_read(void)
{
    int status;
    uint8_t recv_buffer[6]={0};

    /* 启动测量 */
    status = sgp30_start();
    if (status != 0) {
        printf("sgp30 start fail\r\n");
        return -1;
    }

    bflb_mtimer_delay_ms(100);

    /* 读取测量数据 */
    
    struct bflb_i2c_msg_s msgs[1];
    msgs[0].addr = SGP30_ADDR_READ;
    msgs[0].flags = I2C_M_READ;
    msgs[0].buffer = recv_buffer;
    msgs[0].length = 6;
    bflb_i2c_transfer(i2c0, msgs, 1);
    /*
    status = HAL_I2C_Master_Receive(&hi2c1, SGP30_ADDR_READ, (uint8_t*)recv_buffer, 6, 0xFFFF);
    if (status != HAL_OK) {
        printf("I2C Master Receive fail\r\n");
        return -1;
    }
    */

    /* 校验接收的测量数据 */
    if (CheckCrc8(&recv_buffer[0], 0xFF) != recv_buffer[2]) {
        printf("co2 recv data crc check fail\r\n");
        return -1;
    }
    if (CheckCrc8(&recv_buffer[3], 0xFF) != recv_buffer[5]) {
        printf("tvoc recv data crc check fail\r\n");
        return -1;
    }


    /* 转换测量数据 */
    sgp30_data.co2  = recv_buffer[0] << 8 | recv_buffer[1];
    sgp30_data.tvoc = recv_buffer[3] << 8 | recv_buffer[4];

    return 0;
}

