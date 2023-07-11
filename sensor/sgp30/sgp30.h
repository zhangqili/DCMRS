/*
 * sgp30.h
 *
 *  Created on: May 27, 2023
 *      Author: xq123
 */

#ifndef SGP30_H_
#define SGP30_H_
#include "main.h"

#define SGP30_ADDR          0x58
#define SGP30_ADDR_WRITE    SGP30_ADDR       //0xb0
#define SGP30_ADDR_READ     (SGP30_ADDR)   //0xb1

#define CRC8_POLYNOMIAL 0x31

typedef struct sgp30_data_st {
    uint16_t co2;
    uint16_t tvoc;
}sgp30_data_t;

typedef enum sgp30_cmd_en {
    /* 初始化空气质量测量 */
    INIT_AIR_QUALITY = 0x2003,

    /* 开始空气质量测量 */
    MEASURE_AIR_QUALITY = 0x2008

} sgp30_cmd_t;

extern sgp30_data_t sgp30_data;

uint8_t sgp30_send_cmd(sgp30_cmd_t cmd);
int sgp30_soft_reset(void);
int sgp30_init(void);
int sgp30_start(void);
uint8_t CheckCrc8(uint8_t* const message, uint8_t initial_value);
int sgp30_read(void);

#endif /* SGP30_H_ */
