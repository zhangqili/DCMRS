#define DBG_TAG "MAIN"
#include "main.h"
#include "dht22.h"
#include "temt6000.h"
//#include "sgp30.h"
#include "driver_sgp30_advance.h"
#include "driver_sgp30_basic.h"

struct bflb_device_s *gpio;
struct bflb_device_s *adc;
struct bflb_device_s *i2c0;

float temperature;
float humidity;

uint32_t dat;
uint16_t co2;
uint16_t tvoc;
DHT22_Data_TypeDef dht22_dat;
int main(void)
{
    board_init();

    gpio = bflb_device_get_by_name("gpio");
    adc = bflb_device_get_by_name("adc");
    printf("gpio output\r\n");
    TEMT6000_Init(ADC_CHANNEL_10);
    TEMT6000_Read(&dat);

    sgp30_basic_init();
    sgp30_basic_read(&co2, &tvoc);
    printf("CO2:%d, TVOC:%d\n", co2, tvoc);

    //sgp30_init();
    //sgp30_start();
    //sgp30_read();
    //printf("CO2:%d, TVOC:%d\n",sgp30_data.co2,sgp30_data.tvoc);

    while (1) {
        //sgp30_read();
        //printf("CO2:%d, TVOC:%d\n",sgp30_data.co2,sgp30_data.tvoc);
        sgp30_basic_read(&co2, &tvoc);
        //bflb_mtimer_delay_ms(2000);
        DHT22_ReadData(&dht22_dat);
        //printf("CO2:%d, TVOC:%d\n", co2, tvoc);
        printf("hello, world\n");
        printf("temp:%f\n",(dht22_dat.temp_high*256+dht22_dat.temp_low+13)/10.0);
        printf("humi:%f\n",(dht22_dat.temp_high*256+dht22_dat.humi_low+2)/10.0);
        //bflb_gpio_set(gpio, GPIO_PIN_0);
        //printf("GPIO_PIN_1=%x\r\n", bflb_gpio_read(gpio, GPIO_PIN_1));
        //bflb_mtimer_delay_ms(2000);

        //bflb_gpio_reset(gpio, GPIO_PIN_0);
        //printf("DHT22 incoming%c\n",bflb_gpio_read(gpio,GPIO_PIN_10));
        //DHT22_GetTemp_Humidity(&temperature,&humidity);
        //printf("T=%f H=%f\n",temperature,humidity);
        bflb_mtimer_delay_ms(200);
    }
}
