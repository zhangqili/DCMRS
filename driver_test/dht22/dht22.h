#ifndef _DHT11_H
#define _DHT11_H
 

#include "main.h"
 
#define DHT22_PIN			GPIO_PIN_18
 
#define DHT22_OUT_1			bflb_gpio_set(gpio, DHT22_PIN)
#define DHT22_OUT_0			bflb_gpio_reset(gpio, DHT22_PIN)
 
#define DHT22_IN			bflb_gpio_read(gpio, DHT22_PIN)

typedef struct
{
	uint8_t humi_high;
	uint8_t humi_low;
	uint8_t temp_high;
	uint8_t temp_low;
	uint8_t check_sum;
		                 
} DHT22_Data_TypeDef;
 
uint8_t DHT22_ReadData(DHT22_Data_TypeDef* DHT22_Data);
 
#endif	/* __BSP_DHT11_H */











