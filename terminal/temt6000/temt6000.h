
#ifndef TMET6000_H_
#define TMET6000_H_

#include "main.h"

void TEMT6000_Init(uint8_t adc_channel);
void TEMT6000_Read(struct bflb_adc_result_s *dat);

//CH0 19
//CH1 20
//CH4 14
//CH5 13
//CH6 12
//CH7 10
//CH8 1
//CH10 27

#endif TMET6000_H_