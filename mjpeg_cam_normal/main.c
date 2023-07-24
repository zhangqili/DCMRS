#include "bflb_mtimer.h"
#include "board.h"
#include "lcd_conf_user.h"
#include "lcd.h"
#include "stdio.h"
#include "bflb_irq.h"
#include "bflb_uart.h"
#include "bflb_i2c.h"
#include "bflb_cam.h"
#include "image_sensor.h"

#include "bl616_glb.h"
#include "bflb_dma.h"
#include "bflb_mjpeg.h"
#include "jpeg_head.h"

#include "bflb_mtimer.h"
#include "board.h"
#include "fatfs_diskio_register.h"
#include "ff.h"

#define DBG_TAG "MAIN"
#include "log.h"

#define BLOCK_NUM           2
#define ROW_NUM             (240 * BLOCK_NUM)
#define CAM_FRAME_COUNT_USE 5
#define CROP_WQVGA_X        (640)


// #define BLOCK_NUM           2
// #define ROW_NUM             (36 * BLOCK_NUM)
// #define CAM_FRAME_COUNT_USE 50

// static struct bflb_device_s *i2c0;
// static struct bflb_device_s *cam0;

// static struct bflb_device_s *mjpeg;

// volatile uint32_t pic_count = 0;
// volatile uint32_t pic_addr[CAM_FRAME_COUNT_USE] = { 0 };
// volatile uint32_t pic_len[CAM_FRAME_COUNT_USE] = { 0 };

static struct bflb_device_s *i2c0;
static struct bflb_device_s *cam0;
static struct bflb_device_s *mjpeg;
volatile int pic_count = 0;
volatile int pic_count_sum = 0;
volatile uint32_t pic_addr[16] = { 0 };
volatile uint32_t pic_len[16] = { 0 };
//static uint8_t picture[75 * 1024] ATTR_NOINIT_PSRAM_SECTION __attribute__((aligned(64)));
//    static uint8_t picture[CROP_WQVGA_X * 160 *2] ATTR_NOINIT_PSRAM_SECTION __attribute__((aligned(64)));
//uint8_t *pic=(uint8_t *)picture;

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

char file_name[30]; // 定义一个足够大的字符数组用于存储文件名
// 使用sprintf函数生成新的文件名
BYTE RW_Buffer[45* 1024] = { 0 };
#define SDU_DATA_CHECK 1
#if SDU_DATA_CHECK
BYTE Check_Buffer[sizeof(RW_Buffer)] = { 0 };//校验缓冲区
#endif
void fatfs_write_read_test()
{
    FRESULT ret;
    FIL fnew;
    UINT fnum;

    uint32_t time_node, i;
    //picture=pic;
        /* full test data to buff */
//     for (uint32_t size = 0; size < (sizeof(RW_Buffer) - sizeof(test_data)); size += sizeof(test_data)) {
//         memcpy(&RW_Buffer[size], test_data, sizeof(test_data));
// #if SDU_DATA_CHECK
//         memcpy(&Check_Buffer[size], test_data, sizeof(test_data));
// #endif
//     }
    
    sprintf(file_name, "/sd/photo_%d.jpg", pic_count_sum);
    memcpy(&RW_Buffer, (uint8_t *)pic_addr[pic_count], pic_len[pic_count]);

    LOG_F("\r\n  size of pic %d, size of rw_buffer %d", pic_len[pic_count], sizeof(RW_Buffer));

    /* write test */
    LOG_I("\r\n******************** be about to write test... **********************\r\n");
    ret = f_open(&fnew, file_name, FA_CREATE_ALWAYS | FA_WRITE);

    if (ret == FR_OK) {
        time_node = (uint32_t)bflb_mtimer_get_time_ms();
        /*write into file*/
        for (i = 0; i < 1024; i++) {
            ret = f_write(&fnew, RW_Buffer, sizeof(RW_Buffer), &fnum);
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
            LOG_I("Single data size:%d Byte, Write the number:%d, Total size:%d KB\r\n", sizeof(RW_Buffer), i, sizeof(RW_Buffer) * i >> 10);
            LOG_I("Time:%dms, Write Speed:%d KB/s \r\n", time_node, ((sizeof(RW_Buffer) * i) >> 10) * 1000 / time_node);
        } else {
            LOG_F("Fail to write files(%d) num:%d\n", ret, i);
            return;
        }
    } else {
        LOG_F("Fail to open or create files: %d.\r\n", ret);
        return;
    }

    /* read test */
    LOG_I("\r\n******************** be about to read test... **********************\r\n");
    ret = f_open(&fnew, file_name, FA_OPEN_EXISTING | FA_READ);
    if (ret == FR_OK) {
        time_node = (uint32_t)bflb_mtimer_get_time_ms();

        ret = f_read(&fnew, RW_Buffer, 1024, &fnum);
        for (i = 0; i < 1024; i++) {
            ret = f_read(&fnew, RW_Buffer, sizeof(RW_Buffer), &fnum);
            if (ret) {
            } else {
                break;
            }
        }
        /* close file */
        ret |= f_close(&fnew);
        time_node = (uint32_t)bflb_mtimer_get_time_ms() - time_node;
        if (ret == FR_OK) {
            LOG_I("Read Test Succeed! \r\n");
            LOG_I("Single data size:%dByte, Read the number:%d, Total size:%d KB\r\n", sizeof(RW_Buffer), i, sizeof(RW_Buffer) * i >> 10);
            LOG_I("Time:%dms, Read Speed:%d KB/s \r\n", time_node, ((sizeof(RW_Buffer) * i) >> 10) * 1000 / time_node);
        } else {
            LOG_F("Fail to read file: (%d), num:%d\n", ret, i);
            return;
        }
    } else {
        LOG_F("Fail to open files.\r\n");
        return;
    }
}

int length=0;

void mjpeg_isr(int irq, void *arg)
{
    uint8_t *pic;
    uint32_t jpeg_len;

    uint32_t intstatus = bflb_mjpeg_get_intstatus(mjpeg);
    if (intstatus & MJPEG_INTSTS_ONE_FRAME) {
        bflb_mjpeg_int_clear(mjpeg, MJPEG_INTCLR_ONE_FRAME);
        jpeg_len = bflb_mjpeg_get_frame_info(mjpeg, &pic);
        pic_addr[pic_count] = (uint32_t)pic;
        pic_len[pic_count] = jpeg_len;
        length+=jpeg_len;
        pic_count_sum++;
        printf("pic_count %d jpeg_len%d\r\n",pic_count,jpeg_len);
        bflb_mjpeg_pop_one_frame(mjpeg);
        // if (pic_count == CAM_FRAME_COUNT_USE) {
        bflb_cam_stop(cam0);
        bflb_mjpeg_stop(mjpeg);
        board_sdh_gpio_init();
        //memcpy(&RW_Buffer[length-jpeg_len], (uint8_t *)pic_addr[pic_count-1], pic_len[pic_count-1]);
        fatfs_write_read_test();
        board_i2c0_gpio_init();
        bflb_cam_start(cam0);
        bflb_mjpeg_start(mjpeg);
        printf("go on\r\n");
        pic_count++;
        if(pic_count==16)
        {
            pic_count=0;
        }
        // }
    }
}

uint8_t jpg_head_buf[800] = { 0 };
uint32_t jpg_head_len;

uint8_t MJPEG_QUALITY = 70;

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

int main(void)
{
    struct bflb_cam_config_s cam_config;
    struct image_sensor_config_s *sensor_config;

    board_init();
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

    bflb_mjpeg_tcint_mask(mjpeg, false);
    bflb_irq_attach(mjpeg->irq_num, mjpeg_isr, NULL);
    bflb_irq_enable(mjpeg->irq_num);

    bflb_mjpeg_start(mjpeg);

    // while (pic_count < CAM_FRAME_COUNT_USE) {
    //     printf("pic count:%d\r\n", pic_count);
    //     bflb_mtimer_delay_ms(200);
    // }

    // for (uint8_t i = 0; i < CAM_FRAME_COUNT_USE; i++) {
    //     printf("jpg addr:%08x ,jpg size:%d\r\n", pic_addr[i], pic_len[i]);
    //     bflb_mjpeg_dump_hex((uint8_t *)pic_addr[i], pic_len[i]);
    // }

    while (1) {
        bflb_mtimer_delay_ms(1000);
    }
}
