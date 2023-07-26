#include "bflb_mtimer.h"
#include "bflb_gpio.h"
#include "bflb_pwm_v2.h"
#include "bflb_clock.h"
#include "board.h"

struct bflb_device_s *gpio;
struct bflb_device_s *pwm;

#define WATER GPIO_PIN_3
#define IN_1 GPIO_PIN_10
#define IN_2 GPIO_PIN_11
#define IN_3 GPIO_PIN_12
#define IN_4 GPIO_PIN_13
#define AIN_1 GPIO_PIN_17
#define AIN_2 GPIO_PIN_19
#define LED GPIO_PIN_18

//#define IN_1 GPIO_PIN_0
//#define IN_2 GPIO_PIN_1
//#define IN_3 GPIO_PIN_2
//#define IN_4 GPIO_PIN_3
//#define IN_5 GPIO_PIN_19
//#define PWM_1 GPIO_PIN_17
//#define PWM_2 GPIO_PIN_18

int16_t turn_num = 3000;
int16_t i = 0;
    //pwm在0~100
void motor_gpio_init()
{
    gpio = bflb_device_get_by_name("gpio");
	bflb_gpio_init(gpio, WATER, GPIO_OUTPUT | GPIO_SMT_EN | GPIO_DRV_0);
    bflb_gpio_init(gpio, IN_1, GPIO_OUTPUT | GPIO_SMT_EN | GPIO_DRV_0);
    bflb_gpio_init(gpio, IN_2, GPIO_OUTPUT | GPIO_SMT_EN | GPIO_DRV_0);
    bflb_gpio_init(gpio, IN_3, GPIO_OUTPUT | GPIO_SMT_EN | GPIO_DRV_0);
    bflb_gpio_init(gpio, IN_4, GPIO_OUTPUT | GPIO_SMT_EN | GPIO_DRV_0);
	bflb_gpio_init(gpio, AIN_2, GPIO_OUTPUT | GPIO_SMT_EN | GPIO_DRV_0);
	bflb_gpio_init(gpio, AIN_1, GPIO_FUNC_PWM0 | GPIO_ALTERNATE | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_1);
	bflb_gpio_init(gpio, LED, GPIO_FUNC_PWM0 | GPIO_ALTERNATE | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_1);
}


void water_open()
{
	bflb_gpio_set(gpio, WATER);
}

void water_close()
{
	bflb_gpio_reset(gpio, WATER);
}

void curtain_open()//窗帘开
{
    while (1) {
        if (i == turn_num) {
            break;
        }
        bflb_gpio_set(gpio, IN_1);
        bflb_mtimer_delay_ms(2);
        bflb_gpio_reset(gpio, IN_1);
        bflb_gpio_set(gpio, IN_2);
        bflb_mtimer_delay_ms(2);
        bflb_gpio_reset(gpio, IN_2);
        bflb_gpio_set(gpio, IN_3);
        bflb_mtimer_delay_ms(2);
        bflb_gpio_reset(gpio, IN_3);
        bflb_gpio_set(gpio, IN_4);
        bflb_mtimer_delay_ms(2);
        bflb_gpio_reset(gpio, IN_4);
        i++;
		printf("%d\r\n",i);
    }
}
void curtain_close()//窗帘关
{
    while (1) {
        if (i == 0) {
            break;
        }
        bflb_gpio_reset(gpio, IN_4);
        bflb_mtimer_delay_ms(2);
        bflb_gpio_set(gpio, IN_4);
        bflb_gpio_reset(gpio, IN_3);
        bflb_mtimer_delay_ms(2);
        bflb_gpio_set(gpio, IN_3);
        bflb_gpio_reset(gpio, IN_2);
        bflb_mtimer_delay_ms(2);
        bflb_gpio_set(gpio, IN_2);
        bflb_gpio_reset(gpio, IN_1);
        bflb_mtimer_delay_ms(2);
        bflb_gpio_set(gpio, IN_1);
        i--;
		printf("%d\r\n",i);
    }
}
void fan_open(int fan_pwm)//风扇pwm
{
    bflb_pwm_v2_channel_set_threshold(pwm, PWM_CH1, 0, fan_pwm); /* duty = (MotorL_PWM-0)/1000 */
	bflb_pwm_v2_channel_positive_start(pwm, PWM_CH1);
    bflb_pwm_v2_start(pwm);
}

void led_open(int led_pwm)//灯光pwm
{
    bflb_pwm_v2_channel_set_threshold(pwm, PWM_CH2, 0, led_pwm); /* duty = (MotorL_PWM-0)/1000 */
	bflb_pwm_v2_channel_positive_start(pwm, PWM_CH2);
    bflb_pwm_v2_start(pwm);
}

int main(void)
{
    board_init();
    motor_gpio_init();
    bflb_gpio_reset(gpio, IN_1);
    bflb_gpio_reset(gpio, IN_2);
    bflb_gpio_reset(gpio, IN_3);
    bflb_gpio_reset(gpio, IN_4);
	bflb_gpio_reset(gpio, AIN_2);

	pwm = bflb_device_get_by_name("pwm_v2_0");

    struct bflb_pwm_v2_config_s cfg = {
        .clk_source = BFLB_SYSTEM_PBCLK,
        .clk_div = 80,
        .period = 100,
    };//设置频率10KHZ=80MKZ/80/100
	  //周期为100us
    
        bflb_pwm_v2_init(pwm, &cfg);
    	//Give_Motor_PWM(10,10);
    while (1) {
		water_open();
	    //water_close();
		//fan_open(80);
		led_open(50);
        //open();
		//close();
        //bflb_mtimer_delay_ms(1000);
    }
}
