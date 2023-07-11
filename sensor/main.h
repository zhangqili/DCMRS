
#ifndef MAIN_H_
#define MAIN_H_

#include "board.h"
#include "bflb_gpio.h"
#include "bflb_mtimer.h"
#include "bflb_adc.h"
#include "bflb_i2c.h"
#include "bflb_irq.h"
#include "bflb_uart.h"
#include "board.h"
#include "log.h"

#define USE_WIFI 0

//#define USE_LVGL

extern struct bflb_device_s *gpio;
extern struct bflb_device_s *adc;
extern struct bflb_device_s *i2c0;

extern uint8_t tcp_rec_buf[64];
extern char *recv_data;


extern struct bflb_adc_result_s adc_result;
extern uint16_t co2;
extern uint16_t tvoc;

#endif MAIN_H_