#define DBG_TAG "MAIN"
#include "main.h"
#include "dht22.h"
#include "temt6000.h"

struct bflb_device_s *gpio;
struct bflb_device_s *adc;

float temperature;
float humidity;

uint32_t dat;
int main(void)
{
    board_init();

    gpio = bflb_device_get_by_name("gpio");
    adc = bflb_device_get_by_name("adc");
    printf("gpio output\r\n");
    DHT22_Init(DHT22_PIN);
    TEMT6000_Init(ADC_CHANNEL_10);
    TEMT6000_Read(&dat);

    while (1) {
        //printf("hello, world\n");
        //bflb_gpio_set(gpio, GPIO_PIN_0);
        //printf("GPIO_PIN_1=%x\r\n", bflb_gpio_read(gpio, GPIO_PIN_1));
        //bflb_mtimer_delay_ms(2000);

        //bflb_gpio_reset(gpio, GPIO_PIN_0);
        printf("DHT22 incoming%c\n",bflb_gpio_read(gpio,GPIO_PIN_10));
        DHT22_GetTemp_Humidity(&temperature,&humidity);
        printf("T=%f H=%f\n",temperature,humidity);
        bflb_mtimer_delay_ms(1000);
    }
    while (1) {
        
        bflb_mtimer_delay_ms(1000);
    }
}
