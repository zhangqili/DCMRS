#include "main.h"

#include "bl_fw_api.h"
#include "wifi_mgmr_ext.h"
#include "wifi_mgmr.h"

#include <lwip/tcpip.h>
#include <lwip/sockets.h>
#include <lwip/netdb.h>

#include "bl616_glb.h"
#include "rfparam_adapter.h"

#define DBG_TAG "MAIN"
#include "log.h"

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "rtos_def.h"

#include "netdb.h"
//#include "tcp_server_config.h"
#include "mqtt_user.h"

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
/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define WIFI_STACK_SIZE  (1536)
#define TASK_PRIORITY_FW (16)

/****************************************************************************
 * Private Types
 ****************************************************************************/

/****************************************************************************
 * Private Data
 ****************************************************************************/

static struct bflb_device_s *uart0;

static TaskHandle_t wifi_fw_task;

static wifi_conf_t conf = {
    .country_code = "CN",
};
volatile uint32_t wifi_state = 0;

uint8_t carstate=0;

extern void shell_init_with_task(struct bflb_device_s *shell);

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Functions
 ****************************************************************************/

int wifi_start_firmware_task(void)
{
    LOG_I("Starting wifi ...\r\n");

    /* enable wifi clock */

    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_IP_WIFI_PHY | GLB_AHB_CLOCK_IP_WIFI_MAC_PHY | GLB_AHB_CLOCK_IP_WIFI_PLATFORM);
    GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_WIFI);

    /* set ble controller EM Size */

    GLB_Set_EM_Sel(GLB_WRAM160KB_EM0KB);

    if (0 != rfparam_init(0, NULL, 0)) {
        LOG_I("PHY RF init failed!\r\n");
        return 0;
    }

    LOG_I("PHY RF init success!\r\n");

    /* Enable wifi irq */

    extern void interrupt0_handler(void);
    bflb_irq_attach(WIFI_IRQn, (irq_callback)interrupt0_handler, NULL);
    bflb_irq_enable(WIFI_IRQn);

    xTaskCreate(wifi_main, (char *)"fw", WIFI_STACK_SIZE, NULL, TASK_PRIORITY_FW, &wifi_fw_task);

    return 0;
}

void wifi_event_handler(uint32_t code)
{
    switch (code) {
        case CODE_WIFI_ON_INIT_DONE: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_INIT_DONE\r\n", __func__);
            wifi_mgmr_init(&conf);
        } break;
        case CODE_WIFI_ON_MGMR_DONE: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_MGMR_DONE\r\n", __func__);
        } break;
        case CODE_WIFI_ON_SCAN_DONE: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_SCAN_DONE\r\n", __func__);
            wifi_mgmr_sta_scanlist();
        } break;
        case CODE_WIFI_ON_CONNECTED: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_CONNECTED\r\n", __func__);
            void mm_sec_keydump();
            mm_sec_keydump();
        } break;
        case CODE_WIFI_ON_GOT_IP: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_GOT_IP\r\n", __func__);
            LOG_I("[SYS] Memory left is %d Bytes\r\n", kfree_size());
            wifi_state = 1;
        } break;
        case CODE_WIFI_ON_DISCONNECT: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_DISCONNECT\r\n", __func__);
            wifi_state = 0;
        } break;
        case CODE_WIFI_ON_AP_STARTED: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_AP_STARTED\r\n", __func__);
        } break;
        case CODE_WIFI_ON_AP_STOPPED: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_AP_STOPPED\r\n", __func__);
        } break;
        case CODE_WIFI_ON_AP_STA_ADD: {
            LOG_I("[APP] [EVT] [AP] [ADD] %lld\r\n", xTaskGetTickCount());
        } break;
        case CODE_WIFI_ON_AP_STA_DEL: {
            LOG_I("[APP] [EVT] [AP] [DEL] %lld\r\n", xTaskGetTickCount());
        } break;
        default: {
            LOG_I("[APP] [EVT] Unknown code %u \r\n", code);
        }
    }
}


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
void run(void *pvParameters)
{
	while(1)
	{
        if(!carstate)
        {
            printf("Standby\r\n");
            Give_Motor_PWM(0,0);
	        vTaskDelay(200/portTICK_RATE_MS);
            continue;
        }
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


static TaskHandle_t wifi_handle;
static void wifi_task(void *pvParameters)
{
    //wifi_mgmr_init(&conf);
    vTaskDelay(1000/portTICK_RATE_MS);
    wifi_sta_connect("test","19260817",NULL,NULL,1,0,0,1);
    //vTaskSuspend(lvgl_handle);
    //vTaskSuspend(data_handle);
    while(!wifi_mgmr_sta_state_get())
    {
        printf("wifi connecting...\n");
        vTaskDelay(1000/portTICK_RATE_MS);
    }
    
    //vTaskResume(lvgl_handle);
    //vTaskResume(data_handle);
    //wifi_tcp_server_init(1000,NULL);
    vTaskDelay(5000/portTICK_RATE_MS);
    example_mqtt(0,NULL);
    /*
    struct addrinfo hints = {0};
    struct addrinfo *servinfo;
    int rv;
    rv=getaddrinfo(ADDRESS, PORT, &hints, &servinfo);
    */
    //printf("=======================\ncode:%d",open_nb_socket(ADDRESS, PORT));
    while (1)
    {
        printf("wifi running\n");
        //TEMT6000_Read(&adc_result);
        //sgp30_basic_read(&co2, &tvoc);
        //vTaskDelay(100);
        vTaskDelay(1000/portTICK_RATE_MS);
    }
    vTaskDelete(NULL);
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

    
    wifi_start_firmware_task();
    wifi_mgmr_init(&conf);
    tcpip_init(NULL, NULL);


    xTaskCreate(wifi_task, (char *)"wifi_task", 2048, NULL, configMAX_PRIORITIES - 2, &wifi_handle);	
    xTaskCreate(run, (char *)"run_task", 2048, NULL, configMAX_PRIORITIES - 3, NULL);
    vTaskStartScheduler();


    while (1) {
        //run();
        //bflb_mtimer_delay_ms(1000);
    }
}
