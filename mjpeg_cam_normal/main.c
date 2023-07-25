#include "main.h"
#include "lcd_conf_user.h"
#include "lcd.h"
#include "stdio.h"
#include "image_sensor.h"
#include "jpeg_head.h"
#include "fatfs_diskio_register.h"
#include "ff.h"

#define DBG_TAG "MAIN"
#include "log.h"

#include "bl_fw_api.h"
#include "wifi_mgmr_ext.h"
#include "wifi_mgmr.h"

#include <lwip/tcpip.h>
#include <lwip/sockets.h>
#include <lwip/netdb.h>

#include "rfparam_adapter.h"

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "rtos_def.h"

#include "netdb.h"

#include "car.h"
#include "mqtt_user.h"
//#include "tcp_server_config.h"

#define BLOCK_NUM           2
#define ROW_NUM             (240 * BLOCK_NUM)
#define CAM_FRAME_COUNT_USE 5
#define CROP_WQVGA_X        (640)

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define WIFI_STACK_SIZE     (1536)
#define TASK_PRIORITY_FW    (16)

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

struct bflb_device_s *pwm;
struct bflb_device_s *gpio;



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



struct bflb_device_s *i2c0;
struct bflb_device_s *cam0;
struct bflb_device_s *mjpeg;
int pic_count_sum = 0;
uint32_t pic_len;
uint32_t pic_addr;

FATFS fs;
__attribute((aligned(8))) static uint32_t workbuf[4096];

MKFS_PARM fs_para = {
    .fmt = FM_FAT32,
    .n_fat = 1,
    .align = 0,
    .n_root = 1,
    .au_size = 512 * 32,
};

void filesystem_init(void)
{
    FRESULT ret;

    board_sdh_gpio_init();

    fatfs_sdh_driver_register();

    ret = f_mount(&fs, "/sd", 1);

    if (ret == FR_NO_FILESYSTEM) {
        LOG_W("No filesystem yet, try to be formatted...\r\n");

        ret = f_mkfs("/sd", &fs_para, workbuf, sizeof(workbuf));

        if (ret != FR_OK) {
            LOG_F("fail to make filesystem %d\r\n", ret);
            _CALL_ERROR();
        }

        if (ret == FR_OK) {
            LOG_I("done with formatting.\r\n");
            LOG_I("first start to unmount.\r\n");
            ret = f_mount(NULL, "/sd", 1);
            LOG_I("then start to remount.\r\n");
        }
    } else if (ret != FR_OK) {
        LOG_F("fail to mount filesystem,error= %d\r\n", ret);
        LOG_F("SD card might fail to initialise.\r\n");
        _CALL_ERROR();
    } else {
        LOG_D("Succeed to mount filesystem\r\n");
    }

    if (ret == FR_OK) {
        LOG_I("FileSystem cluster size:%d-sectors (%d-Byte)\r\n", fs.csize, fs.csize * 512);
    }
}

char file_name[30];
BYTE RW_Buffer[80 * 1024] = { 0 };
#define SDU_DATA_CHECK 1
#if SDU_DATA_CHECK
BYTE Check_Buffer[sizeof(RW_Buffer)] = { 0 };
#endif
void fatfs_write_read_test()
{
    FRESULT ret;
    FIL fnew;
    UINT fnum;

    uint32_t time_node, i;

    sprintf(file_name, "/sd/photo_%d.jpg", pic_count_sum);
    printf("%s\n",file_name);
    printf("pic_len: %ld\n",pic_len);
    //LOG_I("\r\n******************** be about to write test... **********************\r\n");
    //printf("whoami\n");
    ret = f_open(&fnew, file_name, FA_CREATE_ALWAYS | FA_WRITE);
    //printf("fucku\n");
    if (ret == FR_OK) {
        /*write into file*/
        for (i = 0; i < 1024; i++) {
            ret = f_write(&fnew, (uint8_t *)pic_addr, pic_len, &fnum);
            if (ret) {
            } else {
                LOG_I("\r\n******************** be about to write test%d... **********************\r\n", i);
                break;
            }
        }
        /* close file */
        ret |= f_close(&fnew);

        if (ret == FR_OK) {
            LOG_I("Write Test Succeed! \r\n");
        } else {
            LOG_F("Fail to write files(%d) num:%d\n", ret, i);
            return;
        }
    } else {
        LOG_F("Fail to open or create files: %d.\r\n", ret);
        return;
    }

    /* read test */
    /*
    LOG_I("\r\n******************** be about to read test... **********************\r\n");
    ret = f_open(&fnew, file_name, FA_OPEN_EXISTING | FA_READ);
    if (ret == FR_OK) {
        ret = f_read(&fnew, RW_Buffer, 1024, &fnum);
        for (i = 0; i < 1024; i++) {
            ret = f_read(&fnew, RW_Buffer, sizeof(RW_Buffer), &fnum);
            if (ret) {
            } else {
                break;
            }
        }
        // close file 
        ret |= f_close(&fnew);
        if (ret == FR_OK) {
            LOG_I("Read Test Succeed! \r\n");
        } else {
            LOG_F("Fail to read file: (%d), num:%d\n", ret, i);
            return;
        }
    } else {
        LOG_F("Fail to open files.\r\n");
        return;
    }
    */
}

void mjpeg_isr(int irq, void *arg)
{
}

uint8_t jpg_head_buf[800] = { 0 };
uint32_t jpg_head_len;

#define MJPEG_QUALITY 70

#define SIZE_BUFFER (4 * 1024 * 1024)

void bflb_mjpeg_dump_hex(uint8_t *data, uint32_t len)
{
    uint32_t i = 0;

    for (i = 0; i < len; i++) {
        if (i % 16 == 0) {
            printf("\r\n");
        }

        printf("%02x ", data[i]);
    }

    printf("\r\n");
}

TaskHandle_t mjpeg_handle;
void mjpeg_task(void *pvParameters)
{
    uint8_t *pic;
    uint32_t jpeg_len;

    uint32_t intstatus;

    while (1) {
        if (bflb_mjpeg_get_frame_count(mjpeg) > 0) {
            jpeg_len = bflb_mjpeg_get_frame_info(mjpeg, &pic);
            pic_addr = (uint32_t)pic;
            pic_len = jpeg_len;
            pic_count_sum++;
            bflb_mjpeg_pop_one_frame(mjpeg);
            bflb_cam_stop(cam0);
            bflb_mjpeg_stop(mjpeg);
            board_sdh_gpio_init();
            fatfs_write_read_test();
            board_i2c0_gpio_init();
            bflb_cam_start(cam0);
            bflb_mjpeg_start(mjpeg);
            //vTaskDelay(2);
        }
    }
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
    struct bflb_cam_config_s cam_config;
    struct image_sensor_config_s *sensor_config;

    board_init();
    //car_gpio_init(); 
    filesystem_init();
    board_i2c0_gpio_init();
    board_dvp_gpio_init();

    i2c0 = bflb_device_get_by_name("i2c0");
    cam0 = bflb_device_get_by_name("cam0");

    if (image_sensor_scan(i2c0, &sensor_config)) {
        printf("\r\nSensor name: %s\r\n", sensor_config->name);
    } else {
        printf("\r\nError! Can't identify sensor!\r\n");
        while (1) {
        }
    }

    memcpy(&cam_config, sensor_config, IMAGE_SENSOR_INFO_COPY_SIZE);
    cam_config.with_mjpeg = true;
    cam_config.input_source = CAM_INPUT_SOURCE_DVP;
    cam_config.output_format = CAM_OUTPUT_FORMAT_AUTO;
    cam_config.output_bufaddr = BFLB_PSRAM_BASE;
    cam_config.output_bufsize = cam_config.resolution_x * 2 * ROW_NUM;

    bflb_cam_init(cam0, &cam_config);
    bflb_cam_start(cam0);

    mjpeg = bflb_device_get_by_name("mjpeg");

    struct bflb_mjpeg_config_s config;

    config.format = MJPEG_FORMAT_YUV422_YUYV;
    config.quality = MJPEG_QUALITY;
    config.rows = ROW_NUM;
    config.resolution_x = cam_config.resolution_x;
    config.resolution_y = cam_config.resolution_y;
    config.input_bufaddr0 = (uint32_t)BFLB_PSRAM_BASE;
    config.input_bufaddr1 = 0;
    config.output_bufaddr = (uint32_t)BFLB_PSRAM_BASE + cam_config.resolution_x * 2 * ROW_NUM;
    config.output_bufsize = SIZE_BUFFER - cam_config.resolution_x * 2 * ROW_NUM;
    config.input_yy_table = NULL; /* use default table */
    config.input_uv_table = NULL; /* use default table */

    bflb_mjpeg_init(mjpeg, &config);

    jpg_head_len = JpegHeadCreate(YUV_MODE_422, MJPEG_QUALITY, cam_config.resolution_x, cam_config.resolution_y, jpg_head_buf);
    bflb_mjpeg_fill_jpeg_header_tail(mjpeg, jpg_head_buf, jpg_head_len);

    //bflb_mjpeg_tcint_mask(mjpeg, true);
    //bflb_irq_attach(mjpeg->irq_num, mjpeg_isr, NULL);
    //bflb_irq_enable(mjpeg->irq_num);

    bflb_mjpeg_start(mjpeg);

    //pwm = bflb_device_get_by_name("pwm_v2_0");

    //struct bflb_pwm_v2_config_s cfg = {
    //    .clk_source = BFLB_SYSTEM_PBCLK,
    //    .clk_div = 80,
    //    .period = 100,
    //};
    //设置频率10KHZ=80MKZ/80/100
    //周期为100us
    
    //bflb_pwm_v2_init(pwm, &cfg);
    //Give_Motor_PWM(10,10);
    //xTaskCreate(data_task, (char *)"data_task", 512, NULL, configMAX_PRIORITIES - 3, &data_handle);

    
    //wifi_start_firmware_task();
    //wifi_mgmr_init(&conf);
    //tcpip_init(NULL, NULL);

    xTaskCreate(mjpeg_task, (char *)"mjpeg_task", 2048, NULL, configMAX_PRIORITIES - 2, &mjpeg_handle);
    //xTaskCreate(wifi_task, (char *)"wifi_task", 2048, NULL, configMAX_PRIORITIES - 1, &wifi_handle);	
    //xTaskCreate(run, (char *)"run_task", 2048, NULL, configMAX_PRIORITIES - 3, NULL);
    vTaskStartScheduler();
    while (1) {
    }
}
