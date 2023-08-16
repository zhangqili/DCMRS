/*
 * rgb.h
 *
 *  Created on: May 21, 2023
 *      Author: xq123
 */

#ifndef RGB_H_
#define RGB_H_
#include "main.h"

#define LED_NUM                 (96)
#define ONE_PULSE               (0xFF00)
#define ZERO_PULSE              (0xF000)
#define NONE_PULSE              (0)
#define RGB_BUFFER_LENGTH       (400+3*8*(LED_NUM+1))


extern uint8_t red;
extern uint8_t green;
extern uint8_t blue;

//#define RGB_Start() HAL_TIMEx_PWMN_Start_DMA(&htim8,TIM_CHANNEL_3,RGB_buffer,RGB_BUFFER_LENGTH);

extern  ATTR_NOCACHE_NOINIT_RAM_SECTION  uint16_t RGB_Buffer[RGB_BUFFER_LENGTH];

void RGB_Set(uint8_t r,uint8_t g,uint8_t b,uint16_t index);
void RGB_Flash();
void RGB_TurnOff();
void RGB_Init();

#endif /* RGB_H_ */
