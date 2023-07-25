#include "main.h"
#include "car.h"


int8_t speed=20;
int8_t turn_little=5;
int8_t turn_more=10;

void car_gpio_init()
{
    gpio = bflb_device_get_by_name("gpio");
    //bflb_gpio_init(gpio, IN_1_A, GPIO_OUTPUT |  GPIO_SMT_EN | GPIO_DRV_0);
    //bflb_gpio_init(gpio, IN_1_B, GPIO_OUTPUT |  GPIO_SMT_EN | GPIO_DRV_0);
    //bflb_gpio_init(gpio, IN_2_A, GPIO_OUTPUT |  GPIO_SMT_EN | GPIO_DRV_0);
    //bflb_gpio_init(gpio, IN_2_B, GPIO_OUTPUT |  GPIO_SMT_EN | GPIO_DRV_0);
    bflb_gpio_init(gpio, IN_1, GPIO_INPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0);
    bflb_gpio_init(gpio, IN_2, GPIO_INPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0);
    bflb_gpio_init(gpio, IN_3, GPIO_INPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0);
    bflb_gpio_init(gpio, IN_4, GPIO_INPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0);
    bflb_gpio_init(gpio, IN_5, GPIO_INPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0);
    bflb_gpio_init(gpio, PWM_1, GPIO_FUNC_PWM0 | GPIO_ALTERNATE | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, PWM_2, GPIO_FUNC_PWM0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_32, GPIO_OUTPUT | GPIO_PULL_NONE| GPIO_SMT_EN | GPIO_DRV_0);
}

inline int limit(int a)
{
	a = a>0?a:0;
    a = a<100?a:100;
	return a;
}

void Give_Motor_PWM(int MotorL_PWM,int MotorR_PWM)
{
	/*if (MotorL_PWM>0) //左电机正转
	{
        bflb_gpio_set(gpio, IN_1_A);
        bflb_gpio_reset(gpio, IN_1_B);
	}
	else              //左电机反转
	{
        bflb_gpio_reset(gpio, IN_1_A);
        bflb_gpio_set(gpio, IN_1_B);
	}
	if (MotorR_PWM>0) //右电机正转
	{   
        bflb_gpio_set(gpio, IN_2_A);
        bflb_gpio_reset(gpio, IN_2_B);
	}
	else              //右电机反转
	{
        bflb_gpio_reset(gpio, IN_2_A);
        bflb_gpio_set(gpio, IN_2_B);
	}*/
    bflb_pwm_v2_channel_set_threshold(pwm, PWM_CH1, 0, MotorL_PWM); /* duty = (MotorL_PWM-0)/1000 */
    bflb_pwm_v2_channel_set_threshold(pwm, PWM_CH2, 0, MotorR_PWM); /* duty = (MotorR_PWM-0)/1000 */
	bflb_pwm_v2_channel_positive_start(pwm, PWM_CH1);
    bflb_pwm_v2_channel_positive_start(pwm, PWM_CH2);
    bflb_pwm_v2_start(pwm);
}

void Turn_Right_more(void)
{
    Give_Motor_PWM(speed-turn_more,speed+turn_more);
}

void Turn_Left_more(void)
{
	Give_Motor_PWM(speed+turn_more,speed-turn_more);
}

void Turn_Right_little(void)
{
	Give_Motor_PWM(speed-turn_little,speed+turn_little);
}

void Turn_Left_little(void)
{
	Give_Motor_PWM(speed+turn_little,speed-turn_little);
}
void run_straight(void)
{
	Give_Motor_PWM(speed,speed);
}
