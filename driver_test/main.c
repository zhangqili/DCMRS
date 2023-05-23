#define DBG_TAG "MAIN"
#include "main.h"
#include "dht22.h"

struct bflb_device_s *gpio;

float temperature;
float humidity;

int main(void)
{
    board_init();

    gpio = bflb_device_get_by_name("gpio");
    printf("gpio output\r\n");
    DHT22_Init(GPIO_PIN_10);

    while (1) {
        //printf("hello, world\n");
        //bflb_gpio_set(gpio, GPIO_PIN_0);
        //printf("GPIO_PIN_1=%x\r\n", bflb_gpio_read(gpio, GPIO_PIN_1));
        //bflb_mtimer_delay_ms(2000);

        //bflb_gpio_reset(gpio, GPIO_PIN_0);
        printf("DHT22 incoming\n");
        DHT22_GetTemp_Humidity(&temperature,&humidity);
        printf("T=%f H=%f\n",temperature,humidity);
        bflb_mtimer_delay_ms(1000);
    }
    while (1) {
        
        bflb_mtimer_delay_ms(1000);
    }
}
