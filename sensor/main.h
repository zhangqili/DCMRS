
#ifndef MAIN_H_
#define MAIN_H_

#include "board.h"
#include "bflb_gpio.h"
#include "bflb_mtimer.h"
#include "bflb_adc.h"
#include "bflb_i2c.h"
#include "bflb_irq.h"
#include "bflb_uart.h"
#include "bflb_pwm_v2.h"
#include "bflb_clock.h"
#include "bflb_spi.h"
#include "bflb_dma.h"
#include "log.h"

#define USE_WIFI 0

//#define USE_LVGL

#define WATER GPIO_PIN_2
#define IN_1 GPIO_PIN_10
#define IN_2 GPIO_PIN_11
#define IN_3 GPIO_PIN_12
#define IN_4 GPIO_PIN_13
#define AIN_1 GPIO_PIN_17
//#define AIN_2 GPIO_PIN_19
#define LED GPIO_PIN_18


extern struct bflb_device_s *gpio;
extern struct bflb_device_s *adc;
extern struct bflb_device_s *i2c0;
extern struct bflb_device_s *spi0;
extern struct bflb_device_s *dma0_ch0;

extern uint8_t tcp_rec_buf[64];
extern char *recv_data;


extern struct bflb_adc_result_s adc_result;
extern uint16_t co2;
extern uint16_t tvoc;

extern int16_t target_turn_num;
extern int16_t curtain_i;
extern uint8_t board_led;

void data_task(void *pvParameters);

void curtain_task(void *pvParameters);

void fan_open(int fan_pwm);
void led_open(int led_pwm);

extern bool curtain_running;
extern uint8_t connect_status;

#define water_open() bflb_gpio_set(gpio, WATER)

#define water_close() bflb_gpio_reset(gpio, WATER)

#endif MAIN_H_