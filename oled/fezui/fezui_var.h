/*
 * fezui_var.h
 *
 *  Created on: Jul 21, 2023
 *      Author: xq123
 */

#ifndef FEZUI_VAR_H_
#define FEZUI_VAR_H_
#include "fezui.h"

extern const uint8_t fez_font_6x10_m[204] U8G2_FONT_SECTION("fez_font_6x10_m");
extern fezui_t fezui;
extern uint8_t key_buffer[KEY_NUM];
extern lefl_advanced_key_t Keyboard_AdvancedKeys[MAIN_KEY_NUM];
extern lefl_key_t keys[KEY_NUM-MAIN_KEY_NUM];

extern uint32_t fezui_keytotalcounts[MAIN_KEY_NUM];
extern uint32_t fezui_keyinitcounts[MAIN_KEY_NUM];
extern uint32_t fezui_fps;
extern uint8_t fezui_kps;
extern uint8_t KPS_history_max;
extern char fpsstr[8];
extern char comstr[8];
extern lefl_loop_array_t KPS_history;
extern lefl_array_elm_t KPS_history_data[KPS_HISTORY_LENGTH];
extern lefl_loop_array_t KPS_queue;
extern lefl_array_elm_t KPS_queue_data[REFRESH_RATE];
extern uint8_t UI_KPSMaximumPerSecond;
extern uint8_t fezui_rest_countdown;
extern uint8_t fezui_debug;
extern uint8_t cmd_buffer;

extern uint32_t fezui_run_time;
extern uint32_t fezui_temp_raw;

extern lefl_loop_array_t analog_historys[4];
extern lefl_array_elm_t analog_history1_data[HISTORY_LENGTH];
extern lefl_array_elm_t analog_history2_data[HISTORY_LENGTH];
extern lefl_array_elm_t analog_history3_data[HISTORY_LENGTH];
extern lefl_array_elm_t analog_history4_data[HISTORY_LENGTH];

extern lefl_bit_array_t lines[4];
extern lefl_bit_array_unit_t lines1_data[2];
extern lefl_bit_array_unit_t lines2_data[2];
extern lefl_bit_array_unit_t lines3_data[2];
extern lefl_bit_array_unit_t lines4_data[2];
extern lefl_loop_array_t KPS_queue;


#endif /* FEZUI_VAR_H_ */
