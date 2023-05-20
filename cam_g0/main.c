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

#define CROP_WQVGA_X        (240)
#define CROP_WQVGA_Y        (320)
#define CAM_BUFF_NUM        (4)

int main(void)
{
    // cam
    uint8_t *pic;
    static uint8_t picture[CROP_WQVGA_X * CROP_WQVGA_Y * CAM_BUFF_NUM] ATTR_NOINIT_PSRAM_SECTION __attribute__((aligned(64)));

    board_init(); // 初始化开发板
    lcd_init(); // 初始化LCD屏幕
    lcd_clear(0x0000); // 清空LCD屏幕

    // Cam test
    static struct bflb_device_s *i2c0;
    static struct bflb_device_s *cam0;

    // static struct bflb_device_s *uart0;
    // uart0 = bflb_device_get_by_name("uart0");

    uint32_t i, j, pic_size;
    struct bflb_cam_config_s cam_config;
    struct image_sensor_config_s *sensor_config;
    board_dvp_gpio_init(); // 初始化DVP接口的GPIO引脚
    board_i2c0_gpio_init(); // 初始化I2C接口的GPIO引脚

    i2c0 = bflb_device_get_by_name("i2c0"); // 获取I2C设备
    cam0 = bflb_device_get_by_name("cam0"); // 获取相机设备

    if (image_sensor_scan(i2c0, &sensor_config)) { // 扫描相机传感器
        printf("\r\nSensor name: %s\r\n", sensor_config->name); // 输出传感器名称
    } else {
        printf("\r\nError! Can't identify sensor!\r\n"); // 输出错误信息
        while (1) {
        }
    }

    /* Crop resolution_x, should be set before init */
    bflb_cam_crop_hsync(cam0, 112, 112 + CROP_WQVGA_X); // 设置水平方向的裁剪范围
    /* Crop resolution_y, should be set before init */
    bflb_cam_crop_vsync(cam0, 120, 120 + CROP_WQVGA_Y); // 设置垂直方向的裁剪范围

    memcpy(&cam_config, sensor_config, IMAGE_SENSOR_INFO_COPY_SIZE); // 复制传感器配置信息到相机配置信息结构体中
    cam_config.with_mjpeg = true; // 不使用MJPEG格式
    cam_config.input_source = CAM_INPUT_SOURCE_DVP; // 输入源为DVP接口
    cam_config.output_format = CAM_OUTPUT_FORMAT_AUTO; // 输出格式为自动识别
    cam_config.output_bufaddr = (uint32_t)picture; // 输出缓冲区地址
    cam_config.output_bufsize = CROP_WQVGA_X * CROP_WQVGA_Y * (CAM_BUFF_NUM / 2); // 输出缓冲区大小

    bflb_cam_init(cam0, &cam_config); // 初始化相机
    bflb_cam_start(cam0); // 启动相机

    lcd_set_dir(2,0); // 设置LCD屏幕方向为180度

    printf("cam lcd case\r\n"); // 输出信息

    j = 0;
   while (1) {
        // CAM test     -- 相机测试
        if (bflb_cam_get_frame_count(cam0) > 0) {   // 判断是否接收到相机采集的图像
            // bflb_cam_stop(cam0);
            pic_size = bflb_cam_get_frame_info(cam0, &pic);   // 获取图像信息和地址
            bflb_cam_pop_one_frame(cam0);   // 从相机缓冲区中弹出一张图像
            printf("pop picture %d: 0x%08x, len: %d\r\n", j, (uint32_t)pic, pic_size); // 输出图像信息和地址
            j ++;

            // for(uint32_t c = 0; c < pic_size; c ++)  //串口发送视频图像到pc
            // {
            //     bflb_uart_putchar(uart0, pic[c]);
            // }

            lcd_draw_picture_nonblocking(0,0,CROP_WQVGA_X,CROP_WQVGA_Y, pic); // 显示图像到LCD屏幕上
        }
    }
}