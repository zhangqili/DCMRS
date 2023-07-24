#include "bflb_mtimer.h"
#include "bflb_gpio.h"
#include "bflb_pwm_v2.h"
#include "bflb_clock.h"
#include "bflb_irq.h"
#include "bflb_mtimer.h"
#include "bflb_adc.h"
#include "bflb_i2c.h"
#include "bflb_irq.h"
#include "bflb_cam.h"
#include "bflb_uart.h"
#include "board.h"
#include "stdio.h"

extern struct bflb_device_s *pwm;
extern struct bflb_device_s *gpio;