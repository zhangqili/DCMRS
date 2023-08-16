#include "main.h"
#include "temt6000.h"
#include "driver_sgp30_advance.h"
#include "driver_sgp30_basic.h"
#include "dht22.h"

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
#include "rgb.h"
//#include "tcp_server_config.h"
#include "mqtt_user.h"

struct bflb_device_s *gpio;
struct bflb_device_s *adc;
struct bflb_device_s *i2c0;
struct bflb_device_s *pwm;
struct bflb_device_s *spi0;
struct bflb_device_s *dma0_ch0;

uint8_t tcp_rec_buf[64];
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
struct bflb_adc_result_s adc_result;
uint16_t co2;
uint16_t tvoc;
DHT22_Data_TypeDef dht22_dat;

volatile uint32_t wifi_state = 0;
bool curtain_running = false;

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

#define MAX_TURN_NUM 3000
int16_t target_turn_num = 0;
int16_t curtain_i = 0;
uint8_t board_led = 0;
//pwm在0~100
void motor_gpio_init()
{
    bflb_gpio_init(gpio, WATER, GPIO_OUTPUT | GPIO_SMT_EN | GPIO_DRV_0);
    bflb_gpio_init(gpio, IN_1, GPIO_OUTPUT | GPIO_SMT_EN | GPIO_DRV_0);
    bflb_gpio_init(gpio, IN_2, GPIO_OUTPUT | GPIO_SMT_EN | GPIO_DRV_0);
    bflb_gpio_init(gpio, IN_3, GPIO_OUTPUT | GPIO_SMT_EN | GPIO_DRV_0);
    bflb_gpio_init(gpio, IN_4, GPIO_OUTPUT | GPIO_SMT_EN | GPIO_DRV_0);
    //bflb_gpio_init(gpio, AIN_2, GPIO_OUTPUT | GPIO_SMT_EN | GPIO_DRV_0);
    //bflb_gpio_init(gpio, AIN_1, GPIO_FUNC_PWM0 | GPIO_ALTERNATE | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, LED, GPIO_FUNC_PWM0 | GPIO_ALTERNATE | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_1);
}

void curtain_open() //窗帘开
{
    bflb_gpio_set(gpio, IN_1);
    vTaskDelay(2);
    bflb_gpio_reset(gpio, IN_1);
    bflb_gpio_set(gpio, IN_2);
    vTaskDelay(2);
    bflb_gpio_reset(gpio, IN_2);
    bflb_gpio_set(gpio, IN_3);
    vTaskDelay(2);
    bflb_gpio_reset(gpio, IN_3);
    bflb_gpio_set(gpio, IN_4);
    vTaskDelay(2);
    bflb_gpio_reset(gpio, IN_4);
}
void curtain_close() //窗帘关
{
    bflb_gpio_reset(gpio, IN_4);
    vTaskDelay(2);
    bflb_gpio_set(gpio, IN_4);
    bflb_gpio_reset(gpio, IN_3);
    vTaskDelay(2);
    bflb_gpio_set(gpio, IN_3);
    bflb_gpio_reset(gpio, IN_2);
    vTaskDelay(2);
    bflb_gpio_set(gpio, IN_2);
    bflb_gpio_reset(gpio, IN_1);
    vTaskDelay(2);
    bflb_gpio_set(gpio, IN_1);
}
void fan_open(int fan_pwm) //风扇pwm
{
    bflb_pwm_v2_channel_set_threshold(pwm, PWM_CH2, 0, fan_pwm); /* duty = (MotorL_PWM-0)/1000 */
    bflb_pwm_v2_channel_positive_start(pwm, PWM_CH2);
    bflb_pwm_v2_start(pwm);
}

void led_open(int led_pwm) //灯光pwm
{
    bflb_pwm_v2_channel_set_threshold(pwm, PWM_CH2, 0, led_pwm); /* duty = (MotorL_PWM-0)/1000 */
    bflb_pwm_v2_channel_positive_start(pwm, PWM_CH2);
    bflb_pwm_v2_start(pwm);
}

TaskHandle_t data_handle;
void data_task(void *pvParameters)
{
    DHT22_ReadData(&dht22_dat);
    vTaskDelete(NULL);
}

TaskHandle_t curtain_handle;
void curtain_task(void *pvParameters)
{
    curtain_running = true;
    while (1) {
        if (curtain_i < target_turn_num) {
            curtain_close();
            curtain_i++;
        } else if (curtain_i > target_turn_num) {
            curtain_open();
            curtain_i--;
        } else {
            break;
        }
        printf("%d\n", curtain_i);
    }
    curtain_running = false;
    bflb_gpio_reset(gpio, IN_1);
    bflb_gpio_reset(gpio, IN_2);
    bflb_gpio_reset(gpio, IN_3);
    bflb_gpio_reset(gpio, IN_4);
    vTaskDelete(NULL);
}

static TaskHandle_t wifi_handle;
static void wifi_task(void *pvParameters)
{
    //wifi_mgmr_init(&conf);
    vTaskDelay(1000 / portTICK_RATE_MS);
    wifi_sta_connect("dcmrs", "19260817", NULL, NULL, 1, 0, 0, 1);
    while (!wifi_mgmr_sta_state_get()) {
        printf("wifi connecting...\n");
        vTaskDelay(1000 / portTICK_RATE_MS);
    }

    vTaskDelay(5000 / portTICK_RATE_MS);
    example_mqtt(0, NULL);
    while (1) {
        printf("wifi running\n");
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
    vTaskDelete(NULL);
}

int main(void)
{
    board_init();
    gpio = bflb_device_get_by_name("gpio");
    adc = bflb_device_get_by_name("adc");
    motor_gpio_init();
    bflb_gpio_init(gpio, GPIO_PIN_32, GPIO_OUTPUT | GPIO_PULL_NONE | GPIO_SMT_EN | GPIO_DRV_0);
    bflb_gpio_reset(gpio, IN_1);
    bflb_gpio_reset(gpio, IN_2);
    bflb_gpio_reset(gpio, IN_3);
    bflb_gpio_reset(gpio, IN_4);

    pwm = bflb_device_get_by_name("pwm_v2_0");

    struct bflb_pwm_v2_config_s cfg = {
        .clk_source = BFLB_SYSTEM_PBCLK,
        .clk_div = 80,
        .period = 100,
    };  //设置频率10KHZ=80MKZ/80/100
        //周期为100us

    bflb_pwm_v2_init(pwm, &cfg);
    TEMT6000_Init(ADC_CHANNEL_10);
    sgp30_basic_init();
    RGB_Init();
    RGB_TurnOff();
    for (int j = 0; j < LED_NUM; j++) {
        for (int i = 0; i < j; i++) {
            RGB_Set(100, 100, 100, i);
        }
        bflb_mtimer_delay_ms(10);
    }

    wifi_start_firmware_task();
    wifi_mgmr_init(&conf);
    wifi_mgmr_sta_autoconnect_enable();
    tcpip_init(NULL, NULL);
#ifdef USE_LVGL
    xTaskCreate(lvgl_task, (char *)"lvgl_task", 1024, NULL, configMAX_PRIORITIES - 4, &lvgl_handle);
#endif
    //xTaskCreate(data_task, (char *)"data_task", 512, NULL, configMAX_PRIORITIES - 3, &data_handle);
    xTaskCreate(wifi_task, (char *)"wifi_task", 3072, NULL, 16 + 1, &wifi_handle);
    vTaskStartScheduler();

    while (1) {
    }
}
