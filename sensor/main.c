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
//#include "tcp_server_config.h"
#include "mqtt_user.h"

struct bflb_device_s *gpio;
struct bflb_device_s *adc;
struct bflb_device_s *i2c0;

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

static TaskHandle_t data_handle;
static void data_task(void *pvParameters)
{
    while (1)
    {
        //printf("data running\n");
        TEMT6000_Read(&adc_result);
        //sgp30_basic_read(&co2, &tvoc);
        vTaskDelay(500/portTICK_RATE_MS);
    }
    vTaskDelete(NULL);
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

    gpio = bflb_device_get_by_name("gpio");
    adc = bflb_device_get_by_name("adc");
    TEMT6000_Init(ADC_CHANNEL_10);
    //TEMT6000_Read(&adc_result);
    sgp30_basic_init();
    //sgp30_basic_read(&co2, &tvoc);
    // bflb_cam_stop(cam0);

    //lv_example_get_started_114514();
    //lv_example_label_1();
    //lv_example_chart_6();
    // !Cam test

    //lv_task_handler();
    //xTaskCreate( lv_100ask_task_handler, "lvgl_task_handler", LVGL_TASK_HANDLER_STACK_SIZE, NULL, LVGL_TASK_HANDLER_PRIORITY, (TaskHandle_t *) NULL );	
    
    //wifi_start_firmware_task();
    //wifi_mgmr_connection_info();
    wifi_start_firmware_task();
    wifi_mgmr_init(&conf);
    tcpip_init(NULL, NULL);
#ifdef USE_LVGL
    xTaskCreate(lvgl_task, (char *)"lvgl_task", 1024, NULL, configMAX_PRIORITIES - 4, &lvgl_handle);	
#endif
    xTaskCreate(data_task, (char *)"data_task", 512, NULL, configMAX_PRIORITIES - 3, &data_handle);	
    xTaskCreate(wifi_task, (char *)"wifi_task", 2048, NULL, configMAX_PRIORITIES - 2, &wifi_handle);							
    vTaskStartScheduler();

    while (1) {
    }
}
