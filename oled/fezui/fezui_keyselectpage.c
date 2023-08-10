/*
 * fezui_keyselectpage.c
 *
 *  Created on: 2023年6月19日
 *      Author: xq123
 */
#include "fezui.h"
#include "fezui_var.h"

static float Cursor_X = 0;
static float Cursor_TargetX = 0;
static float target_abscissa = 0;
// width: 5, height: 5
const unsigned char win_icon[] U8X8_PROGMEM = { 0x1b, 0x1b, 0x00, 0x1b, 0x1b };

lefl_keyboard_t keyboard = { .x = 0, .y = 0 };

static fezui_scrollview_t scrollview = { .content_height = 64, .content_width =
        433, .abscissa = 0, .ordinate = 0 };

lefl_page_t keyselectpage = { keyselectpage_logic, keyselectpage_draw,
        keyselectpage_load };
void keyselectpage_logic(lefl_page_t *page)
{

    Cursor_TargetX = 0;
    for (uint8_t i = 0; i < keyboard.x; i++)
    {
        Cursor_TargetX += strlen(ansi_104_keymap[keyboard.y][i]) * 4 + 4;
    }
    target_abscissa = Cursor_TargetX
            + strlen(ansi_104_keymap[keyboard.y][keyboard.x]) * 4 / 2 - 64;
    if (target_abscissa >= 0)
    {
        lefl_cursor_set(&target_cursor,
                64 - strlen(ansi_104_keymap[keyboard.y][keyboard.x]) * 4 / 2,
                keyboard.y * 10,
                strlen(ansi_104_keymap[keyboard.y][keyboard.x]) * 4 + 1, 7);
    }
    else
    {
        target_abscissa = 0;
        lefl_cursor_set(&target_cursor, Cursor_TargetX, keyboard.y * 10,
                strlen(ansi_104_keymap[keyboard.y][keyboard.x]) * 4 + 1, 7);
    }
    lefl_easing_pid(&(scrollview.abscissa), target_abscissa);
}
void keyselectpage_draw(lefl_page_t *page)
{
    uint16_t delta_x = 0;
    u8g2_SetFont(&(fezui.u8g2), u8g2_font_micro_mr);

    for (uint8_t i = 0; i < 6; i++)
    {
        delta_x = 1;
        for (uint8_t j = 0; j < 17; j++)
        {
            u8g2_DrawStr(&(fezui.u8g2),
                    delta_x - (u8g2_int_t)scrollview.abscissa, 10 * (i + 1) - 4,
                    ansi_104_keymap[i][j]);
            delta_x += strlen(ansi_104_keymap[i][j]) * 4 + 4;
            if (delta_x > WIDTH + target_abscissa)
                break;
        }
        u8g2_DrawXBMP(&(fezui.u8g2),
                4 * 5 + 1 - (u8g2_int_t)scrollview.abscissa, 10 * 6 - 5 - 4, 5,
                5, win_icon);
        u8g2_DrawXBMP(&(fezui.u8g2),
                4 * 40 - 3 - (u8g2_int_t)scrollview.abscissa, 10 * 6 - 5 - 4, 5,
                5, win_icon);
    }
    sprintf(fezui_buffer,"%d",delta_x);
    u8g2_DrawStr(&(fezui.u8g2), 96, 48, fezui_buffer);

    fezui_draw_cursor(&fezui, &cursor);
    fezui_draw_scrollview(&fezui, 0, 0, 128, 64, &scrollview);
}

void keyselectpage_load(lefl_page_t *page)
{
    Cursor_X = 0;
    scrollview.abscissa = 0;
    keys[2].key_cb = lambda(void,
            (lefl_key_t*k){lefl_link_frame_go_back(&mainframe);});
    keys[3].key_cb =
            lambda(void,
                    (lefl_key_t*k){lefl_cursor_set(&cursor ,cursor.x-3 ,cursor.y-3 ,cursor.w+6 ,cursor.h+6);});
    keys[4].key_cb = lambda(void,
            (lefl_key_t*k){lefl_keyboard_x_increase(&keyboard, 1);});
    keys[5].key_cb = lambda(void,
            (lefl_key_t*k){lefl_keyboard_x_increase(&keyboard, -1);});
    keys[6].key_cb = lambda(void,
            (lefl_key_t*k){lefl_keyboard_y_increase(&keyboard, 1);});
    keys[7].key_cb = lambda(void,
            (lefl_key_t*k){lefl_keyboard_y_increase(&keyboard, -1);});
}
