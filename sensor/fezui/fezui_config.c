/*
 * fezui_config.c
 *
 *  Created on: 2023年6月19日
 *      Author: xq123
 */
#include "main.h"
#include "fezui.h"
#include "fezui_var.h"
#include "lefl.h"
#include "display.h"

uint8_t tempuint;

void fezui_init()
{
    u8g2_Setup_ssd1306_i2c_128x64_noname_f(&(fezui.u8g2), U8G2_R0, u8x8_byte_hw_i2c,
                                           u8x8_gpio_and_delay);
    u8g2_InitDisplay(&(fezui.u8g2));
    u8g2_SetPowerSave(&(fezui.u8g2), 0);
    //u8g2_SetBitmapMode(&(fezui.u8g2), 1);
    u8g2_SetFontMode(&(fezui.u8g2), 1);
    u8g2_ClearBuffer(&(fezui.u8g2));
    u8g2_SendBuffer(&(fezui.u8g2));

    homepage_init();

    lefl_link_frame_navigate(&mainframe, &homepage);
}

void fezui_timer_handler()
{
    lefl_link_frame_logic(&mainframe);
    lefl_cursor_move(&cursor, &target_cursor);
}

void fezui_next_frame()
{
    lefl_link_frame_logic(&mainframe);
    lefl_cursor_move(&cursor, &target_cursor);
    u8g2_ClearBuffer(&(fezui.u8g2));
    lefl_link_frame_draw(&mainframe);
    u8g2_SendBuffer(&(fezui.u8g2));
}
