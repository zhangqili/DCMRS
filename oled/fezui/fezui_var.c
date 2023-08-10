/*
 * fezui_var.c
 *
 *  Created on: 2023年6月19日
 *      Author: xq123
 */
#include "fezui_var.h"

char fpsstr[8] = {0};
char comstr[8] = {0};

fezui_t fezui =
{
    .invert=1
};

uint8_t key_buffer[KEY_NUM] = {0};
uint8_t cmd_buffer = 0;
lefl_advanced_key_t Keyboard_AdvancedKeys[MAIN_KEY_NUM]=
{
    {

    }
    ,
    {

    }
    ,
    {

    }
    ,
    {

    }
};
lefl_key_t keys[KEY_NUM-MAIN_KEY_NUM];

uint32_t fezui_keytotalcounts[MAIN_KEY_NUM]={0};
uint32_t fezui_keyinitcounts[MAIN_KEY_NUM]={0};

lefl_loop_array_t KPS_history;
lefl_array_elm_t KPS_history_data[KPS_HISTORY_LENGTH];
lefl_loop_array_t KPS_queue;
lefl_array_elm_t KPS_queue_data[REFRESH_RATE];
uint8_t KPS_history_max=0;
uint32_t fezui_fps = 0;
uint8_t fezui_kps = 0;
uint8_t UI_KPSMaximumPerSecond = 0;
uint8_t fezui_rest_countdown = SCREEN_REST_TIME;
uint8_t fezui_debug;
uint32_t fezui_run_time;
uint32_t fezui_temp_raw;
/*
enum USB_HID_KEYBOARD_SCAN_CODES *ansi_104_keycode[6][17] = {
        {KEY_ESC,KEY_F1,KEY_F2,KEY_F3,KEY_F4,KEY_F5,KEY_F6,KEY_F7,KEY_F8,KEY_F9,KEY_F10,KEY_F11,KEY_F12,KEY_PRINT_SCREEN,KEY_SCROLL_LOCK,KEY_PAUSE},
        {KEY_GRAVE,KEY_1,KEY_2,KEY_3,KEY_4,KEY_5,KEY_6,KEY_7,KEY_8,KEY_9,KEY_0,KEY_MINUS,KEY_EQUAL,KEY_BACKSPACE,KEY_INSERT,KEY_HOME,KEY_PAGE_UP},
        {KEY_TAB,KEY_Q,KEY_W,KEY_E,KEY_R,KEY_T,KEY_Y,KEY_U,KEY_I,KEY_O,KEY_P,KEY_LEFT_BRACE,KEY_RIGHT_BRACE,KEY_BACKSLASH,KEY_DELETE,KEY_END,KEY_PAGE_DOWN},
        {KEY_CAPS_LOCK,KEY_A,KEY_S,KEY_D,KEY_F,KEY_G,KEY_H,KEY_J,KEY_K,KEY_L,KEY_SEMICOLON,KEY_APOSTROPHE,KEY_ENTER},
        {KEY_LEFT_SHIFT,KEY_Z,KEY_X,KEY_C,KEY_V,KEY_B,KEY_N,KEY_M,KEY_COMMA,KEY_DOT,KEY_SLASH,KEY_RIGHT_SHIFT,NULL,KEY_UP_ARROW,NULL},
        {KEY_LEFT_CONTROL,KEY_LEFT_GUI,KEY_LEFT_ALT,KEY_SPACEBAR,KEY_RIGHT_ALT,KEY_RIGHT_GUI,KEY_MENU,KEY_RIGHT_CONTROL,KEY_LEFT_ARROW,KEY_DOWN_ARROW,KEY_RIGHT_ARROW}
};
*/

/*
  Fontname: fez_font_6x10
  Copyright: Created with Fony 1.4.6
  Glyphs: 26/26
  BBX Build Mode: 0x
*/
const uint8_t fez_font_6x10_m[204] U8G2_FONT_SECTION("fez_font_6x10_m") =
  "\32\0\3\3\3\4\4\5\4\6\12\0\0\11\0\12\12\0\0\0\0\0\257 \5\0\324\16!\5\0\324"
  "\16\42\5\0\324\16#\5\0\324\16$\5\0\324\16%\5\0\324\16&\5\0\324\16'\5\0\324\16("
  "\5\0\324\16)\5\0\324\16*\5\0\324\16+\5\0\324\16,\5\0\324\16-\6\16\204\16\3.\5"
  "\0\324\16/\5\0\324\16\60\10M\204\16\233\276\31\61\7I\206\216\203\0\62\12M\204\216b\342\61\261"
  "\0\63\12M\204\216b\242\61\321\0\64\11M\204\216\230\66c\16\65\12M\204\16cb\61\321\0\66\12"
  "M\204\16c\242M\315\0\67\10M\204\216b\376\0\70\12M\204\16\233\332M\315\0\71\12M\204\16\233"
  "\232\61\321\0\0\0\0\4\377\377\0";


lefl_link_frame_t mainframe = {
        .current_page=&homepage
    };

lefl_loop_array_t analog_historys[4];
lefl_array_elm_t analog_history1_data[HISTORY_LENGTH];
lefl_array_elm_t analog_history2_data[HISTORY_LENGTH];
lefl_array_elm_t analog_history3_data[HISTORY_LENGTH];
lefl_array_elm_t analog_history4_data[HISTORY_LENGTH];
lefl_cursor_t cursor={0,0,128,64};
lefl_cursor_t target_cursor={0,0,0,0};

lefl_bit_array_t lines[4];
lefl_bit_array_unit_t lines1_data[2];
lefl_bit_array_unit_t lines2_data[2];
lefl_bit_array_unit_t lines3_data[2];
lefl_bit_array_unit_t lines4_data[2];





