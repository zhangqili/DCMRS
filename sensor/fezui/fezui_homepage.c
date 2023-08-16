/*
 * fezui_homepage.c
 *
 *  Created on: 2023年6月19日
 *      Author: xq123
 */

#include "fezui.h"
#include "fezui_var.h"
#include "main.h"

#define PADDING_UP            0
#define MARGIN_LEFT           5
#define MARGIN_UP             11
#define MARGIN_DOWN           4
#define HALF_WIDTH            64
#define TILE_WIDTH            10
#define CHART_HEIGHT          (HEIGHT-MARGIN_DOWN-MARGIN_UP)
#define CHART_WIDTH           64
#define TILE1                 1
#define TILE2                 11
#define TILE3                 21
#define TILE4                 31

lefl_page_t homepage={homepage_logic,homepage_draw,homepage_load};

#define UTF8_TO_CHAR_ARRAY(x) {0xE0|(0x000F&(x>>12)),0x80|((x&0x0FC0)>>6),0x80|(x&0x003F),0}
static char wifi_disconnected_icon[] = UTF8_TO_CHAR_ARRAY(0xE217);
static char wifi_connected_icon[] = UTF8_TO_CHAR_ARRAY(0xE21A);

lefl_menu_t mainmenu;
const char* mainmenu_items[] = {"当前温度","当前湿度","光照强度","二氧化碳"};

fezui_rolling_number_t main_num=
{
    .number=0,
    .font=u8g2_font_helvB18_tn,
    .digit=5
};

fezui_rolling_number_t sub_num=
{
    .number=0,
    .font=u8g2_font_helvB18_tn,
    .digit=1
};

void homepage_logic(lefl_page_t *page)
{
    fezui_rolling_number_set(&main_num,114);

    //sprintf(fezui_buffer,"%2d",KPS_history_max);
    switch (mainmenu.selected_index)
    {
    case 0:
        fezui_rolling_number_set(&main_num,(u8g2_long_t)27.2);
        fezui_rolling_number_set(&sub_num,((u8g2_long_t)27.2*10)%10);
        break;
    case 1:
        fezui_rolling_number_set(&main_num,(u8g2_long_t)57.0);
        fezui_rolling_number_set(&sub_num,0);
        break;
    case 2:
        fezui_rolling_number_set(&main_num,(u8g2_long_t)(3.333 * (float)(adc_result.millivolt)));
        fezui_rolling_number_set(&sub_num,((u8g2_long_t)(3.333 * (float)(adc_result.millivolt)*10))%10);
        break;
    case 3:
        fezui_rolling_number_set(&main_num,co2);
        fezui_rolling_number_set(&sub_num,0);
        break;
    default:
        break;
    }
    fezui_rolling_number_update(&fezui, &main_num);
    fezui_rolling_number_update(&fezui, &sub_num);
}
void homepage_draw(lefl_page_t *page)
{
    u8g2_SetFont(&(fezui.u8g2),u8g2_font_wqy16_t_gb2312);
    u8g2_DrawUTF8(&(fezui.u8g2),MARGIN_LEFT,MARGIN_UP+16,mainmenu.items[mainmenu.selected_index]);
    u8g2_SetFont(&(fezui.u8g2),u8g2_font_siji_t_6x10);
    if(connect_status)
    {
        u8g2_DrawUTF8(&(fezui.u8g2),128-12*1,12,wifi_connected_icon);
    }
    else
    {
        u8g2_DrawUTF8(&(fezui.u8g2),128-12*1,12,wifi_disconnected_icon);
    }
    u8g2_SetFont(&(fezui.u8g2),u8g2_font_helvB18_tn);
    u8g2_DrawStr(&(fezui.u8g2),MARGIN_LEFT+13*5,64-MARGIN_DOWN,".");
    fezui_draw_rolling_number(&fezui,MARGIN_LEFT,64-MARGIN_DOWN,&main_num);
    fezui_draw_rolling_number(&fezui,MARGIN_LEFT+13*6,64-MARGIN_DOWN,&sub_num);
}

void homepage_load(lefl_page_t *page)
{
    key_up.key_cb=lambda(void,(lefl_key_t*k){lefl_menu_index_increase(&mainmenu,1);});
    key_down.key_cb=lambda(void,(lefl_key_t*k){lefl_menu_index_increase(&mainmenu,-1);});
}

void homepage_init()
{
    key_up.key_cb=lambda(void,(lefl_key_t*k){lefl_menu_index_increase(&mainmenu,1);});
    key_down.key_cb=lambda(void,(lefl_key_t*k){lefl_menu_index_increase(&mainmenu,-1);});
    lefl_menu_init(&mainmenu, mainmenu_items, sizeof(mainmenu_items)/sizeof(const char*), NULL);
}
