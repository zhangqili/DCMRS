#include "dht22.h"
#include "main.h"

 //////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//DHT11数字温湿度传感器驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/12
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////
/* USER CODE BEGIN 0 */



/**
	******************************************************************************
	* @file			bsp_dht11.c
	* @author		
	* @date			
	* @version	v1.0
	* @note			DHT11 driver
	******************************************************************************
	*/
 


 
/**
	* @brief DHT22 输出模式
	*/
static void DHT22_Mode_OUT_PP(void)
{
	bflb_gpio_init(gpio, DHT22_PIN, GPIO_INPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0);
/* 	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.Pin = GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); */
}
 
/**
	* @brief DHT22 输入模式
	*/
static void DHT22_Mode_IN_NP(void)
{
	bflb_gpio_init(gpio, DHT22_PIN, GPIO_INPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0);

/* 	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.Pin = GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); */
}
 
/**
	* @brief DHT22 读取字节
	*/
uint8_t DHT22_ReadByte(void)
{
	uint8_t i, temp = 0;
 
	for (i = 0; i < 8; i++)
	{
		while (DHT22_IN == 0);		// 等待低电平结束
		
		bflb_mtimer_delay_us(40);					//	延时 40 微秒		低电平为 0 ，高电平为 1
		
		if (DHT22_IN == 1)
		{
			while (DHT22_IN == 1);	// 等待高电平结束
			
			temp |= (uint8_t)(0X01 << (7 - i));			// 先发送高位 MSB
		}
		else
		{
			temp &= (uint8_t)~(0X01 << (7 - i));
		}
	}
	return temp;
}
 
/**
	* @brief DHT22 读取一次数据
	*/
uint8_t DHT22_ReadData(DHT22_Data_TypeDef *DHT22_Data)
{
	DHT22_Mode_OUT_PP();		// 主机输出，主机拉低
	DHT22_OUT_0;	
	bflb_mtimer_delay_ms(18);					// 延时 18 ms
	
	DHT22_OUT_1;						// 主机拉高，延时 30 us
	bflb_mtimer_delay_us(30);	
 
	DHT22_Mode_IN_NP();			// 主机输入，获取 DHT11 数据
	
	if (DHT22_IN == 0)			// 收到从机应答
	{
		while (DHT22_IN == 0);		// 等待从机应答的低电平结束
		
		while (DHT22_IN == 1);		// 等待从机应答的高电平结束
		
		/*开始接收数据*/   
		DHT22_Data->humi_high  = DHT22_ReadByte();
		DHT22_Data->humi_low = DHT22_ReadByte();
		DHT22_Data->temp_high  = DHT22_ReadByte();
		DHT22_Data->temp_low = DHT22_ReadByte();
		DHT22_Data->check_sum = DHT22_ReadByte();
		
		DHT22_Mode_OUT_PP();		// 读取结束，主机拉高
		DHT22_OUT_1;	
		
		// 数据校验
		if (DHT22_Data->check_sum == DHT22_Data->humi_high + DHT22_Data->humi_low + DHT22_Data->temp_high + DHT22_Data->temp_low)	
		{
			return 1;
		}		
		else
		{
			return 0;
		}
	}
	else		// 未收到从机应答
	{
		return 0;
	}
}
 
// 测试程序
/*
while (1)
{
	if (DHT11_ReadData(&DHT11_Data))
	{
		DEBUG_INFO("\r\n读取DHT11成功!\r\n\r\n湿度为%d.%d ％RH ，温度为 %d.%d℃ \r\n",					\
		DHT11_Data.humi_int,DHT11_Data.humi_deci,DHT11_Data.temp_int,DHT11_Data.temp_deci);
	}
	else
	{
		DEBUG_INFO("Read DHT11 ERROR!\r\n");
	}
	HAL_Delay(3000);
}
*/







