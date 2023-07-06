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

    board_init();
    lcd_init();
    lcd_clear(0x0000);

    // Cam test
    static struct bflb_device_s *i2c0;
    static struct bflb_device_s *cam0;

    // static struct bflb_device_s *uart0;
    // uart0 = bflb_device_get_by_name("uart0");

    uint32_t i, j, pic_size;
    struct bflb_cam_config_s cam_config;
    struct image_sensor_config_s *sensor_config;
    board_dvp_gpio_init();
    board_i2c0_gpio_init();

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
    bflb_cam_crop_hsync(cam0, 112, 112 + CROP_WQVGA_X);
    /* Crop resolution_y, should be set before init */
    bflb_cam_crop_vsync(cam0, 120, 120 + CROP_WQVGA_Y);

    memcpy(&cam_config, sensor_config, IMAGE_SENSOR_INFO_COPY_SIZE);
    cam_config.with_mjpeg = false;
    cam_config.input_source = CAM_INPUT_SOURCE_DVP;
    cam_config.output_format = CAM_OUTPUT_FORMAT_AUTO;
    cam_config.output_bufaddr = (uint32_t)picture;
    cam_config.output_bufsize = CROP_WQVGA_X * CROP_WQVGA_Y * (CAM_BUFF_NUM / 2);

    bflb_cam_init(cam0, &cam_config);
    bflb_cam_start(cam0);

	lcd_set_dir(2,0);//显示旋转180度。

	printf("cam lcd case\r\n");

    j = 0;
    while (1) {

        // CAM test
        if (bflb_cam_get_frame_count(cam0) > 0) {
            // bflb_cam_stop(cam0);
            pic_size = bflb_cam_get_frame_info(cam0, &pic);
            bflb_cam_pop_one_frame(cam0);
            printf("pop picture %d: 0x%08x, len: %d\r\n", j, (uint32_t)pic, pic_size);
            j ++;
            // for(uint32_t c = 0; c < pic_size; c ++)  //串口发送视频图像到pc
            // {
            //     bflb_uart_putchar(uart0, pic[c]);
            // }

            lcd_draw_picture_nonblocking(0,0,CROP_WQVGA_X,CROP_WQVGA_Y, pic);
        }
    }

}
