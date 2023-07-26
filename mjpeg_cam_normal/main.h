
#ifndef MAIN_H_
#define MAIN_H_

#include "board.h"
#include "bflb_gpio.h"
#include "bflb_mtimer.h"
#include "bflb_adc.h"
#include "bflb_i2c.h"
#include "bflb_irq.h"
#include "bflb_cam.h"
#include "bflb_uart.h"
#include "bl616_glb.h"
#include "bflb_mjpeg.h"
#include "bflb_pwm_v2.h"
#include "bflb_clock.h"
#include "log.h"

//#define USE_LVGL

extern struct bflb_device_s *gpio;
extern struct bflb_device_s *i2c0;
extern struct bflb_device_s *cam0;
extern struct bflb_device_s *mjpeg;
extern struct bflb_device_s *pwm;

extern uint8_t tcp_rec_buf[64];
extern char *recv_data;


void mjpeg_task(void *pvParameters);
void mjpeg_save_one_frame(void *pvParameters);
void mjpeg_check_and_save(void *pvParameters);


#endif MAIN_H_