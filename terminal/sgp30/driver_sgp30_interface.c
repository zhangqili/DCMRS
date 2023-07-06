/**
 * Copyright (c) 2015 - present LibDriver All rights reserved
 * 
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. 
 *
 * @file      driver_sgp30_interface_template.c
 * @brief     driver sgp30 interface template source file
 * @version   2.0.0
 * @author    Shifeng Li
 * @date      2021-03-07
 *
 * <h3>history</h3>
 * <table>
 * <tr><th>Date        <th>Version  <th>Author      <th>Description
 * <tr><td>2021/03/07  <td>2.0      <td>Shifeng Li  <td>format the code
 * <tr><td>2020/11/05  <td>1.0      <td>Shifeng Li  <td>first upload
 * </table>
 */

#include "driver_sgp30_interface.h"
#include "main.h"

/**
 * @brief  interface iic bus init
 * @return status code
 *         - 0 success
 *         - 1 iic init failed
 * @note   none
 */
uint8_t sgp30_interface_iic_init(void)
{
    board_i2c0_gpio_init();
    i2c0 = bflb_device_get_by_name("i2c0");
    bflb_i2c_init(i2c0, 100000);
    return 0;
}

/**
 * @brief  interface iic bus deinit
 * @return status code
 *         - 0 success
 *         - 1 iic deinit failed
 * @note   none
 */
uint8_t sgp30_interface_iic_deinit(void)
{
    return 0;
}

/**
 * @brief     interface iic bus write command
 * @param[in] addr is the iic device write address
 * @param[in] *buf points to a data buffer
 * @param[in] len is the length of data buffer
 * @return    status code
 *            - 0 success
 *            - 1 write failed
 * @note      none
 */
uint8_t sgp30_interface_iic_write_cmd(uint8_t addr, uint8_t *buf, uint16_t len)
{
    struct bflb_i2c_msg_s msgs[1];
    msgs[0].addr = addr;
    msgs[0].flags = 0;
    msgs[0].buffer = buf;
    msgs[0].length = len;
    bflb_i2c_transfer(i2c0, msgs, 1);
    return 0;
}

/**
 * @brief      interface iic bus read command
 * @param[in]  addr is the iic device write address
 * @param[out] *buf points to a data buffer
 * @param[in]  len is the length of data buffer
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 * @note       none
 */
uint8_t sgp30_interface_iic_read_cmd(uint8_t addr, uint8_t *buf, uint16_t len)
{
    struct bflb_i2c_msg_s msgs[1];
    msgs[0].addr = addr;
    msgs[0].flags = I2C_M_READ;
    msgs[0].buffer = buf;
    msgs[0].length = len;
    bflb_i2c_transfer(i2c0, msgs, 1);
    return 0;
}

/**
 * @brief     interface delay ms
 * @param[in] ms
 * @note      none
 */
void sgp30_interface_delay_ms(uint32_t ms)
{
    bflb_mtimer_delay_ms(ms);
}

/**
 * @brief     interface print format data
 * @param[in] fmt is the format data
 * @note      none
 */
void sgp30_interface_debug_print(const char *const fmt, ...)
{
    
}
