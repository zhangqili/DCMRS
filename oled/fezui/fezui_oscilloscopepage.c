/*
 * fezui_oscilloscopepage.c
 *
 *  Created on: 2023年6月19日
 *      Author: xq123
 */
#include "fezui.h"
#include "fezui_var.h"

lefl_page_t oscilloscopepage={oscilloscopepage_logic,oscilloscopepage_draw,oscilloscopepage_load};
void oscilloscopepage_logic(lefl_page_t *page)
{
    lefl_loop_array_push_back(analog_historys+0, (uint16_t)Keyboard_AdvancedKeys[0].raw);
    lefl_loop_array_push_back(analog_historys+1, (uint16_t)Keyboard_AdvancedKeys[1].raw);
    lefl_loop_array_push_back(analog_historys+2, (uint16_t)Keyboard_AdvancedKeys[2].raw);
    lefl_loop_array_push_back(analog_historys+3, (uint16_t)Keyboard_AdvancedKeys[3].raw);
}
void oscilloscopepage_draw(lefl_page_t *page)
{
    fezui_draw_wave(&(fezui), 0, 0, 64, 32, analog_historys+0, lines+0);
    fezui_draw_wave(&(fezui), 64, 0, 64, 32, analog_historys+1, lines+1);
    fezui_draw_wave(&(fezui), 0, 32, 64, 32, analog_historys+2, lines+2);
    fezui_draw_wave(&(fezui), 64, 32, 64, 32, analog_historys+3, lines+3);
}

void oscilloscopepage_load(lefl_page_t *page)
{
    keys[2].key_cb=lambda(void,(lefl_key_t*k){lefl_link_frame_go_back(&mainframe);lefl_cursor_set(&cursor ,0 ,0 ,WIDTH ,HEIGHT);});
    keys[3].key_cb=lambda(void,(lefl_key_t*k){;});
    keys[4].key_cb=lambda(void,(lefl_key_t*k){;});
    keys[5].key_cb=lambda(void,(lefl_key_t*k){;});
    keys[6].key_cb=lambda(void,(lefl_key_t*k){;});
    keys[7].key_cb=lambda(void,(lefl_key_t*k){;});
}
