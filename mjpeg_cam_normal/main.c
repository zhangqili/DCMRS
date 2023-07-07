// #include "bflb_mtimer.h"
// #include "board.h"
// #include "lcd_conf_user.h"
// #include "lcd.h"

// #include "bflb_irq.h"
// #include "bflb_uart.h"
// #include "bflb_i2c.h"
// #include "bflb_cam.h"
// #include "image_sensor.h"

// #include "bl616_glb.h"
// #include "bflb_dma.h"
// #include "bflb_mjpeg.h"
// #include "jpeg_head.h"

// #define BLOCK_NUM           2 //定义了图像处理中的块数量。每个块可以看作是图像的一部分，用于分块处理图像数据。
// #define ROW_NUM             (160 * BLOCK_NUM)  //定义了每个块中的行数。每个块中的行数决定了处理的图像数据的高度。这个值通过乘以 BLOCK_NUM 计算得到。
// #define CAM_FRAME_COUNT_USE 5 //定义了需要处理的图像帧数量
// #define CROP_WQVGA_X        (240)
// #define CAM_BUFF_NUM        (4)

// static struct bflb_device_s *i2c0;
// static struct bflb_device_s *cam0;

// static struct bflb_device_s *mjpeg;

// volatile uint32_t pic_count = 0;
// volatile uint32_t pic_addr[CAM_FRAME_COUNT_USE] = { 0 };
// volatile uint32_t pic_len[CAM_FRAME_COUNT_USE] = { 0 };

// void mjpeg_isr(int irq, void *arg)
// {
//     uint8_t *pic;
//     uint32_t jpeg_len;

//     uint32_t intstatus = bflb_mjpeg_get_intstatus(mjpeg);//表示中断状态
//     if (intstatus & MJPEG_INTSTS_ONE_FRAME) {
//         bflb_mjpeg_int_clear(mjpeg, MJPEG_INTCLR_ONE_FRAME);//清楚中断标志位
//         jpeg_len = bflb_mjpeg_get_frame_info(mjpeg, &pic);
//         pic_addr[pic_count] = (uint32_t)pic;
//         pic_len[pic_count] = jpeg_len;
//         pic_count++;
//         bflb_mjpeg_pop_one_frame(mjpeg);
//         lcd_draw_picture_nonblocking(0,0,CROP_WQVGA_X,ROW_NUM, pic); // 显示图像到LCD屏幕上
//         if (pic_count == CAM_FRAME_COUNT_USE) {
//            // bflb_cam_stop(cam0);
//             //bflb_mjpeg_stop(mjpeg);
//             //如果长期压缩图像但是只给一定的图像会导致程序死掉因此不能停下来
//             pic_count=0;
//         }

//     }
// }

// // void mjpeg_isr(int irq, void *arg)
// // {
// //     uint8_t *pic;
// //     uint32_t jpeg_len;

// //     uint32_t intstatus = bflb_mjpeg_get_intstatus(mjpeg);//表示中断状态
// //     if (intstatus & MJPEG_INTSTS_ONE_FRAME) {
// //         bflb_mjpeg_int_clear(mjpeg, MJPEG_INTCLR_ONE_FRAME);//清楚中断标志位
// //         jpeg_len = bflb_mjpeg_get_frame_info(mjpeg, &pic);
// //         pic_addr[pic_count] = (uint32_t)pic;
// //         pic_len[pic_count] = jpeg_len;
// //         pic_count++;
// //         bflb_mjpeg_pop_one_frame(mjpeg);
// //         if (pic_count == CAM_FRAME_COUNT_USE) {
// //             bflb_cam_stop(cam0);
// //             bflb_mjpeg_stop(mjpeg);
// //         }
// //     }
// // }

// uint8_t jpg_head_buf[800] = { 0 };
// uint32_t jpg_head_len;

// uint8_t MJPEG_QUALITY = 50;

// #define SIZE_BUFFER (4 * 1024 * 1024)

// void bflb_mjpeg_dump_hex(uint8_t *data, uint32_t len)
// {
//     uint32_t i = 0;

//     for (i = 0; i < len; i++) {
//         if (i % 16 == 0) {
//             printf("\r\n");
//         }

//         printf("%02x ", data[i]);
//     }

//     printf("\r\n");
// }

// int main(void)
// {
//    // uint8_t *pic_orig;
//     //static uint8_t picture[CROP_WQVGA_X * ROW_NUM * CAM_BUFF_NUM] ATTR_NOINIT_PSRAM_SECTION __attribute__((aligned(64)));
//     struct bflb_cam_config_s cam_config;
//     struct image_sensor_config_s *sensor_config;
//     static struct bflb_device_s *i2c0;
//     static struct bflb_device_s *cam0;
//     board_init();
//     board_dvp_gpio_init();
//     board_i2c0_gpio_init(); // 初始化I2C接口的GPIO引脚
//     lcd_init(); // 初始化LCD屏幕
//     lcd_clear(0x0000); // 清空LCD屏幕

//     i2c0 = bflb_device_get_by_name("i2c0");
//     cam0 = bflb_device_get_by_name("cam0");

//     if (image_sensor_scan(i2c0, &sensor_config)) {
//         printf("\r\nSensor name: %s\r\n", sensor_config->name);
//     } else {
//         printf("\r\nError! Can't identify sensor!\r\n");
//         while (1) {
//         }
//     }
//     /* Crop resolution_x, should be set before init */
//     bflb_cam_crop_hsync(cam0, 112, 112 + CROP_WQVGA_X); // 设置水平方向的裁剪范围
//     /* Crop resolution_y, should be set before init */
//     bflb_cam_crop_vsync(cam0, 120, 120 + ROW_NUM); // 设置垂直方向的裁剪范围
//     memcpy(&cam_config, sensor_config, IMAGE_SENSOR_INFO_COPY_SIZE);
//     cam_config.with_mjpeg = true;
//     cam_config.input_source = CAM_INPUT_SOURCE_DVP;
//     cam_config.output_format = CAM_OUTPUT_FORMAT_AUTO;
//     // cam_config.output_bufaddr = (uint32_t)picture;
//     cam_config.output_bufaddr = BFLB_PSRAM_BASE;//0XA8000000
//     cam_config.output_bufsize = CROP_WQVGA_X *(CAM_BUFF_NUM /2) * ROW_NUM;

//     bflb_cam_init(cam0, &cam_config);
//     bflb_cam_start(cam0);

//     mjpeg = bflb_device_get_by_name("mjpeg");

//     struct bflb_mjpeg_config_s config;

//     config.format = MJPEG_FORMAT_YUV422_YUYV;
//     config.quality = MJPEG_QUALITY;
//     config.rows = ROW_NUM;
//     config.resolution_x = cam_config.resolution_x;
//     config.resolution_y = cam_config.resolution_y;
//     config.input_bufaddr0 = (uint32_t)BFLB_PSRAM_BASE;
//     config.input_bufaddr1 = 0;
//     config.output_bufaddr = (uint32_t)BFLB_PSRAM_BASE + cam_config.resolution_x * 2 * ROW_NUM;
//     config.output_bufsize = SIZE_BUFFER - cam_config.resolution_x * 2 * ROW_NUM;
//     config.input_yy_table = NULL; /* use default table */
//     config.input_uv_table = NULL; /* use default table */

//     bflb_mjpeg_init(mjpeg, &config);
//     lcd_set_dir(2,0); // 设置LCD屏幕方向为180度
//     printf("cam lcd case\r\n"); // 输出信息
    
//     // jpg_head_len = JpegHeadCreate(YUV_MODE_422, MJPEG_QUALITY, cam_config.resolution_x, cam_config.resolution_y, jpg_head_buf);
//     // bflb_mjpeg_fill_jpeg_header_tail(mjpeg, jpg_head_buf, jpg_head_len);

//     bflb_mjpeg_tcint_mask(mjpeg, false);
//     bflb_irq_attach(mjpeg->irq_num, mjpeg_isr, NULL);
//     bflb_irq_enable(mjpeg->irq_num);

//     bflb_mjpeg_start(mjpeg);

//     while (pic_count < CAM_FRAME_COUNT_USE) {
//         printf("pic count:%d\r\n", pic_count);
//         bflb_mtimer_delay_ms(200);
//     }

//     for (uint8_t i = 0; i < CAM_FRAME_COUNT_USE; i++) {
//         printf("jpg addr:%08x ,jpg size:%d\r\n", pic_addr[i], pic_len[i]);
//         //bflb_mjpeg_dump_hex((uint8_t *)pic_addr[i], pic_len[i]);
//     }
//     while (1) 
//     {
//             bflb_mtimer_delay_ms(1000);
//     }
// }


#include "bflb_mtimer.h"
#include "board.h"
#include "lcd_conf_user.h"
#include "lcd.h"

#include "bflb_irq.h"
#include "bflb_uart.h"
#include "bflb_i2c.h"
#include "bflb_cam.h"
#include "image_sensor.h"

#include "bl616_glb.h"
#include "bflb_dma.h"
#include "bflb_mjpeg.h"
#include "jpeg_head.h"
#include "decompressed.h"
#define BLOCK_NUM           2 //定义了图像处理中的块数量。每个块可以看作是图像的一部分，用于分块处理图像数据。
#define ROW_NUM             (110 * BLOCK_NUM)  //定义了每个块中的行数。每个块中的行数决定了处理的图像数据的高度。这个值通过乘以 BLOCK_NUM 计算得到。
#define CAM_FRAME_COUNT_USE 5 //定义了需要处理的图像帧数量
#define CROP_WQVGA_X        (240)
static struct bflb_device_s *i2c0;
static struct bflb_device_s *cam0;

static struct bflb_device_s *mjpeg;
//volatile告诉编译器这个变量可能会改变
volatile uint32_t pic_count = 0;
volatile uint32_t pic_addr[CAM_FRAME_COUNT_USE] = { 0 };
volatile uint32_t pic_len[CAM_FRAME_COUNT_USE] = { 0 };
static uint8_t picture[CROP_WQVGA_X * 160 * 2] ATTR_NOINIT_PSRAM_SECTION __attribute__((aligned(64)));

uint8_t *pic;
void mjpeg_isr(int irq, void *arg)
{

    //uint8_t *pic;
    uint32_t jpeg_len;

    uint32_t intstatus = bflb_mjpeg_get_intstatus(mjpeg);//表示中断状态
    if (intstatus & MJPEG_INTSTS_ONE_FRAME) {
        bflb_mjpeg_int_clear(mjpeg, MJPEG_INTCLR_ONE_FRAME);//清楚中断标志位
        jpeg_len = bflb_mjpeg_get_frame_info(mjpeg, &pic);
        pic_addr[pic_count] = (uint32_t)pic;
        pic_len[pic_count] = jpeg_len;
        pic_count++;
        //bflb_mjpeg_pop_one_frame(mjpeg);
        //yuyv_to_rgb(pic,picture,CROP_WQVGA_X,ROW_NUM);
       // printf("第%d帧压缩图像的大小: %d\r\n",pic_count,jpeg_len);
        //lcd_draw_picture_nonblocking(0,0,CROP_WQVGA_X,ROW_NUM, pic); // 显示图像到LCD屏幕上
        if (pic_count == CAM_FRAME_COUNT_USE) {
           // bflb_cam_stop(cam0);
            //bflb_mjpeg_stop(mjpeg);
            //如果长期压缩图像但是只给一定的图像会导致程序死掉因此不能停下来
            pic_count=0;
        }

    }
}

// void mjpeg_isr(int irq, void *arg)
// {
//     uint8_t *pic;
//     uint32_t jpeg_len;

//     uint32_t intstatus = bflb_mjpeg_get_intstatus(mjpeg);//表示中断状态
//     if (intstatus & MJPEG_INTSTS_ONE_FRAME) {
//         bflb_mjpeg_int_clear(mjpeg, MJPEG_INTCLR_ONE_FRAME);//清楚中断标志位
//         jpeg_len = bflb_mjpeg_get_frame_info(mjpeg, &pic);
//         pic_addr[pic_count] = (uint32_t)pic;
//         pic_len[pic_count] = jpeg_len;
//         pic_count++;
//         bflb_mjpeg_pop_one_frame(mjpeg);
//         if (pic_count == CAM_FRAME_COUNT_USE) {
//             bflb_cam_stop(cam0);
//             bflb_mjpeg_stop(mjpeg);
//         }
//     }
// }

uint8_t jpg_head_buf[800] = { 0 };
uint32_t jpg_head_len;

uint8_t MJPEG_QUALITY = 50;

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
    static struct bflb_device_s *i2c0;
    static struct bflb_device_s *cam0;
    board_init();
    board_dvp_gpio_init();
    board_i2c0_gpio_init(); // 初始化I2C接口的GPIO引脚
    lcd_init(); // 初始化LCD屏幕
    lcd_clear(0x0000); // 清空LCD屏幕

    i2c0 = bflb_device_get_by_name("i2c0");
    cam0 = bflb_device_get_by_name("cam0");

    if (image_sensor_scan(i2c0, &sensor_config)) {
        printf("\r\nSensor name: %s\r\n", sensor_config->name);
    } else {
        printf("\r\nError! Can't identify sensor!\r\n");
        while (1) {
        }
    }
    /* Crop resolution_x, should be set before init */
    bflb_cam_crop_hsync(cam0, 112, 112 + CROP_WQVGA_X); // 设置水平方向的裁剪范围
    /* Crop resolution_y, should be set before init */
    bflb_cam_crop_vsync(cam0, 120, 120 + 160); // 设置垂直方向的裁剪范围
    memcpy(&cam_config, sensor_config, IMAGE_SENSOR_INFO_COPY_SIZE);
    cam_config.with_mjpeg = true;
    cam_config.input_source = CAM_INPUT_SOURCE_DVP;
    cam_config.output_format = CAM_OUTPUT_FORMAT_AUTO;
    cam_config.output_bufaddr = BFLB_PSRAM_BASE;//0XA8000000
    cam_config.output_bufsize = CROP_WQVGA_X * 2 * ROW_NUM;

    bflb_cam_init(cam0, &cam_config);
    bflb_cam_start(cam0);

    mjpeg = bflb_device_get_by_name("mjpeg");

    struct bflb_mjpeg_config_s config;

    config.format = MJPEG_FORMAT_YUV422_YUYV;
    config.quality = MJPEG_QUALITY;
    config.rows = ROW_NUM;
    config.resolution_x = CROP_WQVGA_X;
    config.resolution_y = ROW_NUM;
    config.input_bufaddr0 = (uint32_t)BFLB_PSRAM_BASE;
    config.input_bufaddr1 = 0;
    config.output_bufaddr = (uint32_t)BFLB_PSRAM_BASE + cam_config.resolution_x * 2 * ROW_NUM;
    config.output_bufsize = SIZE_BUFFER - cam_config.resolution_x * 2 * ROW_NUM;
    config.input_yy_table = NULL; /* use default table */
    config.input_uv_table = NULL; /* use default table */

    bflb_mjpeg_init(mjpeg, &config);
    lcd_set_dir(2,0); // 设置LCD屏幕方向为180度

    printf("cam lcd case\r\n"); // 输出信息
    //jpg_head_len = JpegHeadCreate(YUV_MODE_422, MJPEG_QUALITY, cam_config.resolution_x, cam_config.resolution_y, jpg_head_buf);
    //bflb_mjpeg_fill_jpeg_header_tail(mjpeg, jpg_head_buf, jpg_head_len);

    bflb_mjpeg_tcint_mask(mjpeg, false);
    bflb_irq_attach(mjpeg->irq_num, mjpeg_isr, NULL);
    bflb_irq_enable(mjpeg->irq_num);

    bflb_mjpeg_start(mjpeg);

    // while (pic_count < CAM_FRAME_COUNT_USE) {
    //     printf("pic count:%d\r\n", pic_count);
    //     bflb_mtimer_delay_ms(200);
    // }
    uint8_t *pic_now =(uint8_t *)picture;
    while (1)
    {
        if(bflb_mjpeg_get_frame_count(mjpeg)>0)
        {
            bflb_mjpeg_pop_one_frame(mjpeg);
            printf("正在显示");
            // for(int i=0;i<CROP_WQVGA_X * ROW_NUM * 2;i++)
            // printf("%d :%d\r\n",i,pic_now[i]);
            yuyv_to_rgb(pic,picture,CROP_WQVGA_X,ROW_NUM);
            //bflb_mtimer_delay_ms(200);
            lcd_draw_picture_nonblocking(0,0,CROP_WQVGA_X,160, pic_now);
        }

        /* code */
    }
    

    // while (1) {
    //     for (uint8_t i = 0; i < CAM_FRAME_COUNT_USE; i++) {
    //         printf("pic count:%d jpg addr:%08x ,jpg size:%d\r\n", pic_count,pic_addr[i], pic_len[i]);
    //         //bflb_mjpeg_dump_hex((uint8_t *)pic_addr[i], pic_len[i]);
    //         bflb_mtimer_delay_ms(10);
    //     }

    //     //bflb_mtimer_delay_ms(1000);
    // }
}
