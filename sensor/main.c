#include "main.h"
#include "temt6000.h"
#include "driver_sgp30_advance.h"
#include "driver_sgp30_basic.h"
#include "dht22.h"

#include "lv_conf.h"
#include "lvgl.h"

#include "lv_port_disp.h"
#include "lv_port_indev.h"

#include "lcd.h"

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

#ifdef USE_LVGL
#if LV_USE_METER && LV_BUILD_EXAMPLES

static lv_obj_t * temt6000meter;
lv_meter_indicator_t * temt6000indic;

/**
 * A simple meter
 */
void temt6000meter_init(void)
{
    temt6000meter = lv_meter_create(lv_scr_act());
    lv_obj_center(temt6000meter);
    lv_obj_set_align(temt6000meter,LV_ALIGN_TOP_LEFT);
    lv_obj_set_size(temt6000meter, 100, 100);
    lv_meter_scale_t*scale=lv_meter_add_scale(temt6000meter);

    /*Add a scale first*/
    lv_meter_set_scale_ticks(temt6000meter,scale, 41, 2, 10, lv_palette_main(LV_PALETTE_GREY));
    lv_meter_set_scale_major_ticks(temt6000meter,scale, 8, 4, 15, lv_color_black(), 10);


    /*Add a blue arc to the start*/
    temt6000indic = lv_meter_add_arc(temt6000meter,scale, 3, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_meter_set_indicator_start_value(temt6000meter, temt6000indic, 0);
    lv_meter_set_indicator_end_value(temt6000meter, temt6000indic, 20);

    /*Make the tick lines blue at the start of the scale*/
    temt6000indic = lv_meter_add_scale_lines(temt6000meter,scale, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_BLUE),
                                     false, 0);
    lv_meter_set_indicator_start_value(temt6000meter, temt6000indic, 0);
    lv_meter_set_indicator_end_value(temt6000meter, temt6000indic, 20);

    /*Add a red arc to the end*/
    temt6000indic = lv_meter_add_arc(temt6000meter,scale, 3, lv_palette_main(LV_PALETTE_RED), 0);
    lv_meter_set_indicator_start_value(temt6000meter, temt6000indic, 80);
    lv_meter_set_indicator_end_value(temt6000meter, temt6000indic, 100);

    /*Make the tick lines red at the end of the scale*/
    temt6000indic = lv_meter_add_scale_lines(temt6000meter,scale, lv_palette_main(LV_PALETTE_RED), lv_palette_main(LV_PALETTE_RED), false,
                                     0);
    lv_meter_set_indicator_start_value(temt6000meter, temt6000indic, 80);
    lv_meter_set_indicator_end_value(temt6000meter, temt6000indic, 100);

    /*Add a needle line indicator*/
    temt6000indic = lv_meter_add_needle_line(temt6000meter,scale, 4, lv_palette_main(LV_PALETTE_GREY), -10);

    /*Create an animation to set the value*/
    /*
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_exec_cb(&a, set_value);
    lv_anim_set_var(&a, indic);
    lv_anim_set_values(&a, 0, 100);
    lv_anim_set_time(&a, 2000);
    lv_anim_set_repeat_delay(&a, 100);
    lv_anim_set_playback_time(&a, 500);
    lv_anim_set_playback_delay(&a, 100);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&a);
    */
}

static lv_obj_t * sgp30meter;
lv_meter_indicator_t * sgp30indic;

/**
 * A simple meter
 */
void sgp30meter_init(void)
{
    sgp30meter = lv_meter_create(lv_scr_act());
    lv_obj_center(sgp30meter);
    lv_obj_set_align(sgp30meter,LV_ALIGN_TOP_RIGHT);
    lv_obj_set_size(sgp30meter, 100, 100);
    lv_meter_scale_t*scale=lv_meter_add_scale(sgp30meter);

    /*Add a scale first*/
    lv_meter_set_scale_ticks(sgp30meter,scale, 41, 2, 10, lv_palette_main(LV_PALETTE_GREY));
    lv_meter_set_scale_major_ticks(sgp30meter,scale, 8, 4, 15, lv_color_black(), 10);


    /*Add a blue arc to the start*/
    sgp30indic = lv_meter_add_arc(sgp30meter,scale, 3, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_meter_set_indicator_start_value(sgp30meter, sgp30indic, 0);
    lv_meter_set_indicator_end_value(sgp30meter, sgp30indic, 20);

    /*Make the tick lines blue at the start of the scale*/
    sgp30indic = lv_meter_add_scale_lines(sgp30meter,scale, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_BLUE),
                                     false, 0);
    lv_meter_set_indicator_start_value(sgp30meter, sgp30indic, 0);
    lv_meter_set_indicator_end_value(sgp30meter, sgp30indic, 20);

    /*Add a red arc to the end*/
    sgp30indic = lv_meter_add_arc(sgp30meter,scale, 3, lv_palette_main(LV_PALETTE_RED), 0);
    lv_meter_set_indicator_start_value(sgp30meter, sgp30indic, 80);
    lv_meter_set_indicator_end_value(sgp30meter, sgp30indic, 100);

    /*Make the tick lines red at the end of the scale*/
    sgp30indic = lv_meter_add_scale_lines(sgp30meter,scale, lv_palette_main(LV_PALETTE_RED), lv_palette_main(LV_PALETTE_RED), false,
                                     0);
    lv_meter_set_indicator_start_value(sgp30meter, sgp30indic, 80);
    lv_meter_set_indicator_end_value(sgp30meter, sgp30indic, 100);

    /*Add a needle line indicator*/
    sgp30indic = lv_meter_add_needle_line(sgp30meter,scale, 4, lv_palette_main(LV_PALETTE_GREY), -10);

    /*Create an animation to set the value*/
    /*
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_exec_cb(&a, set_value);
    lv_anim_set_var(&a, indic);
    lv_anim_set_values(&a, 0, 100);
    lv_anim_set_time(&a, 2000);
    lv_anim_set_repeat_delay(&a, 100);
    lv_anim_set_playback_time(&a, 500);
    lv_anim_set_playback_delay(&a, 100);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&a);
    */
}

#endif
#if LV_USE_CHART && LV_BUILD_EXAMPLES

static lv_obj_t * chart1;
static lv_chart_series_t * ser1;
static lv_chart_series_t * ser2;

static void draw_event_cb(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);

    /*Add the faded area before the lines are drawn*/
    lv_obj_draw_part_dsc_t * dsc = lv_event_get_draw_part_dsc(e);
    if(dsc->part == LV_PART_ITEMS) {
        if(!dsc->p1 || !dsc->p2) return;

        /*Add a line mask that keeps the area below the line*/
        lv_draw_mask_line_param_t line_mask_param;
        lv_draw_mask_line_points_init(&line_mask_param, dsc->p1->x, dsc->p1->y, dsc->p2->x, dsc->p2->y,
                                      LV_DRAW_MASK_LINE_SIDE_BOTTOM);
        int16_t line_mask_id = lv_draw_mask_add(&line_mask_param, NULL);

        /*Add a fade effect: transparent bottom covering top*/
        lv_coord_t h = lv_obj_get_height(obj);
        lv_draw_mask_fade_param_t fade_mask_param;
        lv_draw_mask_fade_init(&fade_mask_param, &obj->coords, LV_OPA_COVER, obj->coords.y1 + h / 8, LV_OPA_TRANSP,
                               obj->coords.y2);
        int16_t fade_mask_id = lv_draw_mask_add(&fade_mask_param, NULL);

        /*Draw a rectangle that will be affected by the mask*/
        lv_draw_rect_dsc_t draw_rect_dsc;
        lv_draw_rect_dsc_init(&draw_rect_dsc);
        draw_rect_dsc.bg_opa = LV_OPA_20;
        draw_rect_dsc.bg_color = dsc->line_dsc->color;

        lv_area_t a;
        a.x1 = dsc->p1->x;
        a.x2 = dsc->p2->x - 1;
        a.y1 = LV_MIN(dsc->p1->y, dsc->p2->y);
        a.y2 = obj->coords.y2;
        lv_draw_rect(dsc->draw_ctx, &draw_rect_dsc, &a);

        /*Remove the masks*/
        lv_draw_mask_free_param(&line_mask_param);
        lv_draw_mask_free_param(&fade_mask_param);
        lv_draw_mask_remove_id(line_mask_id);
        lv_draw_mask_remove_id(fade_mask_id);
    }
    /*Hook the division lines too*/
    else if(dsc->part == LV_PART_MAIN) {
        if(dsc->line_dsc == NULL || dsc->p1 == NULL || dsc->p2 == NULL) return;

        /*Vertical line*/
        if(dsc->p1->x == dsc->p2->x) {
            dsc->line_dsc->color  = lv_palette_lighten(LV_PALETTE_GREY, 1);
            if(dsc->id == 3) {
                dsc->line_dsc->width  = 2;
                dsc->line_dsc->dash_gap  = 0;
                dsc->line_dsc->dash_width  = 0;
            }
            else {
                dsc->line_dsc->width = 1;
                dsc->line_dsc->dash_gap  = 6;
                dsc->line_dsc->dash_width  = 6;
            }
        }
        /*Horizontal line*/
        else {
            if(dsc->id == 2) {
                dsc->line_dsc->width  = 2;
                dsc->line_dsc->dash_gap  = 0;
                dsc->line_dsc->dash_width  = 0;
            }
            else {
                dsc->line_dsc->width = 2;
                dsc->line_dsc->dash_gap  = 6;
                dsc->line_dsc->dash_width  = 6;
            }

            if(dsc->id == 1  || dsc->id == 3) {
                dsc->line_dsc->color  = lv_palette_main(LV_PALETTE_GREEN);
            }
            else {
                dsc->line_dsc->color  = lv_palette_lighten(LV_PALETTE_GREY, 1);
            }
        }
    }
}

/**
 * Add a faded area effect to the line chart and make some division lines ticker
 */
void lv_example_chart_2(void)
{
    /*Create a chart1*/
    chart1 = lv_chart_create(lv_scr_act());
    lv_obj_set_size(chart1, 200, 150);
    lv_obj_center(chart1);
    lv_chart_set_type(chart1, LV_CHART_TYPE_LINE);   /*Show lines and points too*/
    lv_obj_set_align(chart1,LV_ALIGN_BOTTOM_MID);

    lv_chart_set_div_line_count(chart1, 5, 7);

    lv_obj_add_event_cb(chart1, draw_event_cb, LV_EVENT_DRAW_PART_BEGIN, NULL);
    lv_chart_set_update_mode(chart1, LV_CHART_UPDATE_MODE_CIRCULAR);

    /*Add two data series*/
    ser1 = lv_chart_add_series(chart1, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);
    //ser2 = lv_chart_add_series(chart1, lv_palette_main(LV_PALETTE_BLUE), LV_CHART_AXIS_SECONDARY_Y);

    uint32_t i;
    for(i = 0; i < 10; i++) {
        //lv_chart_set_next_value(chart1, ser1, lv_rand(20, 90));
        //lv_chart_set_next_value(chart1, ser2, lv_rand(30, 70));
    }
}

#endif
#if LV_USE_LABEL && LV_BUILD_EXAMPLES
lv_obj_t * label1;
/**
 * Show line wrap, re-color, line align and text scrolling.
 */
void lv_example_label_1(void)
{
    label1 = lv_label_create(lv_scr_act());
    lv_label_set_long_mode(label1, LV_LABEL_LONG_WRAP);     /*Break the long lines*/
    lv_label_set_text(label1, "Receive data show here");
    lv_obj_set_width(label1, 150);  /*Set smaller width to make the lines wrap*/
    lv_obj_set_style_text_align(label1, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label1, LV_ALIGN_CENTER, 0, -40);
}

#endif
static TaskHandle_t lvgl_handle;
static void set_data(lv_timer_t * timer)
{
    LV_UNUSED(timer);
    lv_meter_set_indicator_value(temt6000meter, temt6000indic, adc_result.millivolt/4);
    lv_meter_set_indicator_value(sgp30meter, sgp30indic, (co2-400)/120);
    lv_chart_set_next_value(chart1, ser1, adc_result.millivolt/4);
    lv_label_set_text(label1,recv_data);
}

/* lvgl log cb */
void lv_log_print_g_cb(const char *buf)
{
    printf("[LVGL] %s", buf);
}
static void lvgl_task(void *pvParameters)
{

    printf("lvgl case\r\n");
    lv_log_register_print_cb(lv_log_print_g_cb);
    lv_init();
    lv_port_disp_init();
    lv_port_indev_init();

    //lv_obj_t *scr = lv_scr_act();
    //lv_obj_t *avatar = lv_img_create(scr);
    //lv_img_set_angle(avatar, 0);
    //lv_obj_align(avatar, LV_ALIGN_CENTER, -1, -1);

    temt6000meter_init();
    sgp30meter_init();
    lv_example_chart_2();
    lv_example_label_1();
    //lv_demo_benchmark();
    // lv_demo_stress();
    lv_timer_create(set_data, 200, NULL);
    


    while (1)
    {
        //TEMT6000_Read(&adc_result);
        //printf("lvgl running\n");
        //rtos_mutex_lock(lvgl_mutex);
        lv_task_handler();
        vTaskDelay(10/portTICK_RATE_MS);
        //rtos_mutex_unlock(lvgl_mutex);
        //printf("lvgl done\n");
        //vTaskDelay(10/portTICK_RATE_MS);
        
        //vTaskDelay(10/portTICK_RATE_MS);
        //vTaskDelay(1);
        //bflb_mtimer_delay_ms(50);
    }
    vTaskDelete(NULL);
}
#endif

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
