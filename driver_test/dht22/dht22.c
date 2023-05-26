/*
Library:					DHT22 - AM2302 Temperature and Humidity Sensor
Written by:				Mohamed Yaqoob (MYaqoobEmbedded YouTube Channel)
Date Written:			21/11/2018
Last modified:		-/-
Description:			This is an STM32 device driver library for the DHT22 Temperature and Humidity Sensor, using STM HAL libraries

References:				This library was written based on the DHT22 datasheet
										- https://cdn-shop.adafruit.com/datasheets/Digital+humidity+and+temperature+sensor+AM2302.pdf
										
* Copyright (C) 2018 - M. Yaqoob
   This is a free software under the GNU license, you can redistribute it and/or modify it under the terms
   of the GNU General Public Licenseversion 3 as published by the Free Software Foundation.
	
   This software library is shared with puplic for educational purposes, without WARRANTY and Author is not liable for any damages caused directly
   or indirectly by this software, read more about this on the GNU General Public License.
*/

//Header files
#include "dht22.h"

//Bit fields manipulations
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))

//1. One wire data line
static uint16_t oneWire_PIN;

//*** Functions prototypes ***//
//OneWire Initialise
void DHT22_Init(uint16_t DataPin)
{
	oneWire_PIN = DataPin;
}
//Change pin mode
void ONE_WIRE_PinMode(OnePinMode_Typedef mode)
{
	if(mode == ONE_OUTPUT)
	{
		bflb_gpio_deinit(gpio, oneWire_PIN);
		bflb_gpio_init(gpio, oneWire_PIN, GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0);
	}
	else if(mode == ONE_INPUT)
	{
		bflb_gpio_init(gpio, oneWire_PIN, GPIO_INPUT | GPIO_FLOAT | GPIO_SMT_EN | GPIO_DRV_0);
	}
}	
//One Wire pin HIGH/LOW Write
void ONE_WIRE_Pin_Write(bool state)
{
	if(state) bflb_gpio_set(gpio, oneWire_PIN);
	else bflb_gpio_reset(gpio, oneWire_PIN);
}
bool ONE_WIRE_Pin_Read(void)
{
	return (1&bflb_gpio_read(gpio, oneWire_PIN));
}

//Microsecond delay
void DelayMicroSeconds(uint32_t uSec)
{
	bflb_mtimer_delay_us(uSec);
}

//DHT Begin function
void DHT22_StartAcquisition(void)
{
	//Change data pin mode to OUTPUT
	ONE_WIRE_PinMode(ONE_OUTPUT);
	//Put pin LOW
	ONE_WIRE_Pin_Write(0);
	//500uSec delay
	DelayMicroSeconds(500);
	//Bring pin HIGH
	ONE_WIRE_Pin_Write(1);
	//30 uSec delay
	DelayMicroSeconds(30);
	//Set pin as input
	ONE_WIRE_PinMode(ONE_INPUT);
}
//Read 5 bytes
void DHT22_ReadRaw(uint8_t *data)
{
	uint32_t rawBits = 0UL;
	uint8_t checksumBits=0;
	
	DelayMicroSeconds(40);
	while(!ONE_WIRE_Pin_Read());
	while(ONE_WIRE_Pin_Read());
	for(int8_t i=31; i>=0; i--)
	{
		while(!ONE_WIRE_Pin_Read());
		DelayMicroSeconds(40);
		if(ONE_WIRE_Pin_Read())
		{
			rawBits |= (1UL << i);
		}
		while(ONE_WIRE_Pin_Read());
	}
	
	for(int8_t i=7; i>=0; i--)
	{
		while(!ONE_WIRE_Pin_Read());
		DelayMicroSeconds(40);
		if(ONE_WIRE_Pin_Read())
		{
			checksumBits |= (1UL << i);
		}
		while(ONE_WIRE_Pin_Read());
	}
	
	
	//Copy raw data to array of bytes
	data[0] = (rawBits>>24)&0xFF;
	data[1] = (rawBits>>16)&0xFF;
	data[2] = (rawBits>>8)&0xFF;
	data[3] = (rawBits>>0)&0xFF;
	data[4] = (checksumBits)&0xFF;
}

//Get Temperature and Humidity data
bool DHT22_GetTemp_Humidity(float *Temp, float *Humidity)
{
	uint8_t dataArray[6], myChecksum;
	uint16_t Temp16, Humid16;
	//Implement Start data Aqcuisition routine
	DHT22_StartAcquisition();
	//Aqcuire raw data
	DHT22_ReadRaw(dataArray);
	//calculate checksum
	myChecksum = 0;
	for(uint8_t k=0; k<4; k++) 
	{
		myChecksum += dataArray[k];
	}
	if(myChecksum == dataArray[4])
	{
		Temp16 = (dataArray[2] <<8) | dataArray[3];
		Humid16 = (dataArray[0] <<8) | dataArray[1];
		
		*Temp = Temp16/10.0f;
		*Humidity = Humid16/10.0f;
		return 1;
	}
	return 0;
}
