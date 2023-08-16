
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
#include "lefl.h"

#define USE_WIFI 0

#define DHT22_PIN GPIO_PIN_18

extern struct bflb_device_s *gpio;
extern struct bflb_device_s *adc;
extern struct bflb_device_s *i2c0;

extern float temperature;
extern float humidity;
extern float co2content;
extern float lightlux;

extern uint8_t carstate;

extern lefl_loop_array_t temp_history;
extern lefl_loop_array_t humi_history;
extern lefl_loop_array_t co2_history;
extern lefl_loop_array_t light_history;

extern uint8_t carstate_send_flag;
extern uint8_t sensor_send_flag;

extern int16_t ShadeSwitch;
extern int16_t FanSwitch;
extern int16_t IrrigationSwitch;
extern int16_t LightControl;
extern int16_t LEDRed;
extern int16_t LEDGreen;
extern int16_t LEDBlue;

extern uint8_t tcp_rec_buf[64];
extern char *recv_data;

extern char dates[16][32];

#endif MAIN_H_