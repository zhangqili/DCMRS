#include "bflb_mtimer.h"
#include "bflb_gpio.h"
#include "bflb_pwm_v2.h"
#include "bflb_clock.h"
#include "board.h"

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"

struct bflb_device_s *pwm;
struct bflb_device_s *gpio;

//#define IN_1_A GPIO_PIN_0
//#define IN_1_B GPIO_PIN_1
//#define IN_2_A GPIO_PIN_2
//#define IN_2_B GPIO_PIN_3
#define IN_1 GPIO_PIN_0
#define IN_2 GPIO_PIN_1
#define IN_3 GPIO_PIN_2
#define IN_4 GPIO_PIN_3
#define IN_5 GPIO_PIN_19
#define PWM_1 GPIO_PIN_17
#define PWM_2 GPIO_PIN_18

int8_t speed=20;
int8_t turn_little=5;
int8_t turn_more=10;
//pwm在0~100


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
}

int ABS(int a)
{
	a = a>0?a:(-a);
	return a;
}

int limit(int a)
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

void run(void)
{
	while(1)
	{
	    if((bflb_gpio_read(gpio, IN_1)==1)&&(bflb_gpio_read(gpio, IN_2)==0)&&(bflb_gpio_read(gpio, IN_4)==1))
	    {
	    	Turn_Left_little();
            printf("Turn_Left_little\r\n");
	    }
	    if((bflb_gpio_read(gpio, IN_5)==1)&&(bflb_gpio_read(gpio, IN_4)==0)&&(bflb_gpio_read(gpio, IN_2)==1))
	    {
	    	Turn_Right_little();
            printf("Turn_Right_little\r\n");
	    }
	    if(bflb_gpio_read(gpio, IN_1)==0)
	    {
	    	Turn_Left_more();
            printf("Turn_Left_more\r\n");
	    }
	    if(bflb_gpio_read(gpio, IN_5)==0)
	    {
	    	Turn_Right_more();
            printf("Turn_Right_more\r\n");
	    }
	    if(bflb_gpio_read(gpio, IN_3)==0)
	    {
	    	run_straight();
             printf("run_straight\r\n");
	    }
	    vTaskDelay(200/portTICK_RATE_MS);

	}
}
int main(void)
{
    board_init();
    car_gpio_init(); 
   

    pwm = bflb_device_get_by_name("pwm_v2_0");

    struct bflb_pwm_v2_config_s cfg = {
        .clk_source = BFLB_SYSTEM_PBCLK,
        .clk_div = 80,
        .period = 100,
    };//设置频率10KHZ=80MKZ/80/100
	  //周期为100us
    
        bflb_pwm_v2_init(pwm, &cfg);
    	//Give_Motor_PWM(10,10);
    //xTaskCreate(data_task, (char *)"data_task", 512, NULL, configMAX_PRIORITIES - 3, &data_handle);	
    xTaskCreate(run, (char *)"run_task", 2048, NULL, configMAX_PRIORITIES - 2, NULL);
    vTaskStartScheduler();


    while (1) {
        //run();
        //bflb_mtimer_delay_ms(1000);
    }
}
