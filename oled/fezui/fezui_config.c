/*
 * fezui_config.c
 *
 *  Created on: 2023年6月19日
 *      Author: xq123
 */

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
    u8g2_DrawLine(&(fezui.u8g2), 15, 15, 20, 18);
    u8g2_DrawLine(&(fezui.u8g2), 15, 22, 20, 18);
    u8g2_DrawLine(&(fezui.u8g2), 26, 16, 20, 18);
    u8g2_DrawLine(&(fezui.u8g2), 26, 26, 20, 18);
    u8g2_DrawLine(&(fezui.u8g2), 26, 16, 31, 12);
    u8g2_DrawLine(&(fezui.u8g2), 21, 8, 31, 12);
    u8g2_DrawLine(&(fezui.u8g2), 39, 4, 31, 12);
    u8g2_DrawLine(&(fezui.u8g2), 42, 21, 31, 12);
    u8g2_DrawLine(&(fezui.u8g2), 26, 26, 33, 30);
    u8g2_DrawLine(&(fezui.u8g2), 46, 39, 33, 30);
    u8g2_DrawLine(&(fezui.u8g2), 26, 26, 26, 38);
    u8g2_DrawLine(&(fezui.u8g2), 42, 44, 26, 38);
    u8g2_DrawLine(&(fezui.u8g2), 42, 21, 52, 27);
    u8g2_DrawLine(&(fezui.u8g2), 55, 26, 52, 27);
    u8g2_DrawLine(&(fezui.u8g2), 42, 21, 50, 32);
    u8g2_SendBuffer(&(fezui.u8g2));

    lefl_loop_array_init(&KPS_history, KPS_history_data, sizeof(KPS_history_data)/sizeof(lefl_array_elm_t));
    lefl_loop_array_init(&KPS_queue, KPS_queue_data, sizeof(KPS_queue_data)/sizeof(lefl_array_elm_t));
    lefl_loop_array_init(analog_historys+0, analog_history1_data, sizeof(analog_history1_data)/sizeof(lefl_array_elm_t));
    lefl_loop_array_init(analog_historys+1, analog_history2_data, sizeof(analog_history2_data)/sizeof(lefl_array_elm_t));
    lefl_loop_array_init(analog_historys+2, analog_history3_data, sizeof(analog_history3_data)/sizeof(lefl_array_elm_t));
    lefl_loop_array_init(analog_historys+3, analog_history4_data, sizeof(analog_history4_data)/sizeof(lefl_array_elm_t));

    lefl_bit_array_init(lines+0, lines1_data, sizeof(lines1_data)*8);
    lefl_bit_array_init(lines+1, lines2_data, sizeof(lines2_data)*8);
    lefl_bit_array_init(lines+2, lines3_data, sizeof(lines3_data)*8);
    lefl_bit_array_init(lines+3, lines4_data, sizeof(lines4_data)*8);

    homepage_init();

    lefl_link_frame_navigate(&mainframe, &homepage);
}

void fezui_timer_handler()
{
    //for (uint8_t i = 0; i < MAIN_KEY_NUM; i++)
    //{
    //    lefl_advanced_key_update_state(advanced_keys+i, key_buffer[i]);
    //}
    for (uint8_t i = MAIN_KEY_NUM; i < KEY_NUM; i++)
    {
        lefl_key_update(keys+i-4, key_buffer[i]);
    }

    lefl_link_frame_logic(&mainframe);
    lefl_cursor_move(&cursor, &target_cursor);

    uint8_t n=0;
    for (uint8_t i = 0; i < MAIN_KEY_NUM; i++)
    {
        if (lefl_advanced_key_is_triggered(Keyboard_AdvancedKeys+i))
        {
            fezui_keytotalcounts[i]++;
            n++;
        }
        lefl_bit_array_shift(lines+i, 1);
        if (Keyboard_AdvancedKeys[i].key.state)
        {
            lefl_bit_array_set(lines+i, 0, true);
        }
    }
    lefl_loop_array_push_back(&KPS_queue, n);

    fezui_kps = 0;
    for (uint8_t i = 0; i < REFRESH_RATE; i++)
    {
        fezui_kps += KPS_queue.data[i];
    }
    if (fezui_kps > UI_KPSMaximumPerSecond)
    {
        UI_KPSMaximumPerSecond = fezui_kps;
    }
    KPS_history_max=lefl_loop_array_max(&KPS_history);
    if(fezui_kps>KPS_history_max)
    {
        KPS_history_max=fezui_kps;
        KPS_history.data[KPS_history.index]=fezui_kps;
    }

    if (KPS_history_max || keys[0].state||keys[1].state||keys[2].state||keys[3].state||keys[4].state||keys[5].state||keys[6].state||keys[7].state)
    {
        //tempuint=keys[0].state+(keys[1].state<<1)+(keys[2].state<<2)+(keys[3].state<<3)+(keys[4].state<<4)+(keys[5].state<<5)+(keys[6].state<<6)+(keys[7].state<<7);

        fezui_rest_countdown = SCREEN_REST_TIME;
    }
    /*
    u8g2_ClearBuffer(&(fezui.u8g2));
    //sprintf(fezui_tempstr,"%d",UI_IsDisplayOn);
    //u8g2_DrawStr(&(fezui.u8g2), 64, 10, fezui_tempstr);
    lefl_link_frame_draw(&mainframe);
#ifdef _SCREEN_REST_ON
    u8g2_SetPowerSave(&(fezui.u8g2),!UI_IsDisplayOn);
#endif
    u8g2_SendBuffer(&(fezui.u8g2));

    UI_FPS++;
    */
}

extern fezui_t fezui;

void fezui_menu_update_selection(lefl_menu_t *menu)
{
    if (lefl_key_is_triggered(keys+4))
    {
        lefl_menu_index_increase(menu, 1);
    }
    if (lefl_key_is_triggered(keys+5))
    {
        lefl_menu_index_increase(menu, -1);
    }
    if (lefl_key_is_triggered(keys+6))
    {
        lefl_menu_index_increase(menu, 1);
    }
    if (lefl_key_is_triggered(keys+7))
    {
        lefl_menu_index_increase(menu, -1);
    }
}

