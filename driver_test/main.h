
#ifndef MAIN_H_
#define MAIN_H_

#include "bflb_gpio.h"
#include "bflb_adc.h"
#include "bflb_mtimer.h"
#include "board.h"
#include "log.h"

#define DHT22_PIN GPIO_PIN_18

extern struct bflb_device_s *gpio;
extern struct bflb_device_s *adc;

#endif MAIN_H_