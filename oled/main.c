#include "main.h"
#include "display.h"
#include "fezui.h"
#include "fezui_var.h"


struct bflb_device_s *i2c0;

u8g2_t u8g2;
int32_t count=0;

int main(void)
{
    board_init();
    board_i2c0_gpio_init();

    i2c0 = bflb_device_get_by_name("i2c0");

    bflb_i2c_init(i2c0, 1000000);

    fezui_init(&fezui);

    while (1) {
        fezui_timer_handler();
        u8g2_ClearBuffer(&(fezui.u8g2));
        //sprintf(fezui_tempstr,"%d",UI_IsDisplayOn);
        //u8g2_DrawStr(&(fezui.u8g2), 64, 10, fezui_tempstr);
        if (fezui.invert) {
            u8g2_SendF(&(fezui.u8g2), "c", 0xA7);
        } else {
        }
        u8g2_SetDrawColor(&(fezui.u8g2), 1);
        lefl_link_frame_draw(&mainframe);
        u8g2_SetFont(&(fezui.u8g2), fez_font_6x10_m);
        u8g2_DrawStr(&(fezui.u8g2), 64, 10, comstr);
#ifdef _SCREEN_REST_ON
        if (fezui.invert) {
            fezui_veil(&(fezui), 0, 0, 128, 64, 7 - fezui_rest_countdown, 1);
        } else {
            fezui_veil(&(fezui), 0, 0, 128, 64, 7 - fezui_rest_countdown, 0);
        }
        u8g2_SetPowerSave(&(fezui.u8g2), !fezui_rest_countdown);
#endif
#ifdef _FPS_ON
        u8g2_SetDrawColor(&(fezui.u8g2), 2);
        u8g2_SetFont(&(fezui.u8g2), fez_font_6x10_m);
        u8g2_DrawStr(&(fezui.u8g2), 95 + 15, 10, fpsstr);
#endif
        u8g2_SendBuffer(&(fezui.u8g2));
        fezui_fps++;
        count++;
        if(count>=50)
        {
            count=0;
            lefl_menu_index_increase(&mainmenu,1);
        }
    }
}
