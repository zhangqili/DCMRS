/*
 * fezui_homepage.c
 *
 *  Created on: 2023年6月19日
 *      Author: xq123
 */

#include "fezui.h"
#include "fezui_var.h"

#define MARGIN_LEFT           65
#define PADDING_UP            0
#define MARGIN_UP             11
#define MARGIN_DOWN           12
#define HALF_WIDTH            64
#define TILE_WIDTH            10
#define CHART_HEIGHT          (HEIGHT-MARGIN_DOWN-MARGIN_UP)
#define CHART_WIDTH           64
#define TILE1                 1
#define TILE2                 11
#define TILE3                 21
#define TILE4                 31


lefl_page_t homepage={homepage_logic,homepage_draw,homepage_load};

fezui_rolling_number_t key1_num=
{
    .number=0,
    .font=fez_font_6x10_m,
    .digit=5
};

fezui_rolling_number_t key2_num=
{
    .number=0,
    .font=fez_font_6x10_m,
    .digit=5
};

fezui_rolling_number_t key3_num=
{
    .number=0,
    .font=fez_font_6x10_m,
    .digit=5
};

fezui_rolling_number_t key4_num=
{
    .number=0,
    .font=fez_font_6x10_m,
    .digit=5
};

fezui_rolling_number_t kps_num=
{
    .number=0,
    .font=fez_font_6x10_m,
    .digit=2
};

fezui_rolling_number_t max_kps_num=
{
    .number=0,
    .font=fez_font_6x10_m,
    .digit=2
};

void homepage_logic(lefl_page_t *page)
{
    if((lefl_key_is_triggered(keys+0)&&keys[1].state)||(lefl_key_is_triggered(keys+1)&&keys[0].state))
    {
        lefl_link_frame_navigate(&mainframe, &menupage);
        lefl_cursor_set(&cursor ,0 ,0 ,WIDTH ,HEIGHT);
    }
    fezui_rolling_number_set(&key1_num,fezui_keytotalcounts[0]-fezui_keyinitcounts[0]);
    fezui_rolling_number_set(&key2_num,fezui_keytotalcounts[1]-fezui_keyinitcounts[1]);
    fezui_rolling_number_set(&key3_num,fezui_keytotalcounts[2]-fezui_keyinitcounts[2]);
    fezui_rolling_number_set(&key4_num,fezui_keytotalcounts[3]-fezui_keyinitcounts[3]);
    fezui_rolling_number_set(&kps_num,fezui_kps);
    fezui_rolling_number_set(&max_kps_num,KPS_history_max);

    //sprintf(fezui_buffer,"%2d",KPS_history_max);
    fezui_rolling_number_update(&fezui, &key1_num);
    fezui_rolling_number_update(&fezui, &key2_num);
    fezui_rolling_number_update(&fezui, &key3_num);
    fezui_rolling_number_update(&fezui, &key4_num);
    fezui_rolling_number_update(&fezui, &kps_num);
    fezui_rolling_number_update(&fezui, &max_kps_num);
}
void homepage_draw(lefl_page_t *page)
{
    fezui_draw_flowingwater(&(fezui),MARGIN_LEFT,TILE1+MARGIN_UP,HALF_WIDTH,TILE_WIDTH,lines+0);
    fezui_draw_flowingwater(&(fezui),MARGIN_LEFT,TILE2+MARGIN_UP,HALF_WIDTH,TILE_WIDTH,lines+1);
    fezui_draw_flowingwater(&(fezui),MARGIN_LEFT,TILE3+MARGIN_UP,HALF_WIDTH,TILE_WIDTH,lines+2);
    fezui_draw_flowingwater(&(fezui),MARGIN_LEFT,TILE4+MARGIN_UP,HALF_WIDTH,TILE_WIDTH,lines+3);

    fezui_draw_chart(&(fezui), 0, MARGIN_UP, CHART_WIDTH, CHART_HEIGHT, &KPS_history, KPS_history_max);
    u8g2_SetFont(&(fezui.u8g2), u8g2_font_micro_tr);
    u8g2_DrawStr(&(fezui.u8g2),0,MARGIN_UP-1,"KPS:");
    u8g2_DrawStr(&(fezui.u8g2),32,MARGIN_UP-1,"MAX:");
    u8g2_DrawStr(&(fezui.u8g2),95,MARGIN_UP-1,"FPS:");
    u8g2_SetFont(&(fezui.u8g2), fez_font_6x10_m);

    //sprintf(fezui_buffer, "%5ld", fezui_keytotalcounts[0]-fezui_keyinitcounts[0]);
    //u8g2_DrawStr(&(fezui.u8g2),2,63,fezui_buffer);

    //sprintf(fezui_buffer, "%5ld", fezui_keytotalcounts[1]-fezui_keyinitcounts[1]);
    //u8g2_DrawStr(&(fezui.u8g2),34,63,fezui_buffer);

    //sprintf(fezui_buffer, "%5ld", fezui_keytotalcounts[2]-fezui_keyinitcounts[2]);
    //u8g2_DrawStr(&(fezui.u8g2),66,63,fezui_buffer);

    //sprintf(fezui_buffer, "%5ld", fezui_keytotalcounts[3]-fezui_keyinitcounts[3]);
    //u8g2_DrawStr(&(fezui.u8g2),98,63,fezui_buffer);

    fezui_draw_rolling_number(&fezui, 2, 63, &key1_num);
    fezui_draw_rolling_number(&fezui, 34, 63, &key2_num);
    fezui_draw_rolling_number(&fezui, 66, 63, &key3_num);
    fezui_draw_rolling_number(&fezui, 98, 63, &key4_num);

#ifndef _FPS_ON
    u8g2_DrawStr(&(fezui.u8g2),95+15,MARGIN_UP-1,fpsstr);
#endif

    //sprintf(fezui_buffer,"%2d",fezui_kps);
    //u8g2_DrawStr(&(fezui.u8g2),15,MARGIN_UP-1,fezui_buffer);

    //sprintf(fezui_buffer,"%2d",KPS_history_max);
    //u8g2_DrawStr(&(fezui.u8g2),15+32,MARGIN_UP-1,fezui_buffer);

    fezui_draw_rolling_number(&fezui, 15, MARGIN_UP-1, &kps_num);
    fezui_draw_rolling_number(&fezui, 15+32, MARGIN_UP-1, &max_kps_num);

    //sprintf(fezui_tempstr,"%.0f",advanced_keys[0].raw);
    //u8g2_DrawStr(&(fezui.u8g2),64,MARGIN_UP-1,fezui_tempstr);

    u8g2_SetFont(&(fezui.u8g2), u8g2_font_6x13_tf);
    if(keys[0].state)
        u8g2_DrawButtonUTF8(&(fezui.u8g2), 67, MARGIN_UP-1, U8G2_BTN_INV, 2, 1, 1, "S");
    if(keys[1].state)
        u8g2_DrawButtonUTF8(&(fezui.u8g2), 67 + MARGIN_UP, MARGIN_UP-1, U8G2_BTN_INV, 2, 1, 1, "A");

    u8g2_DrawHLine(&(fezui.u8g2),0,MARGIN_UP,128);
    u8g2_DrawHLine(&(fezui.u8g2),0,CHART_HEIGHT+MARGIN_UP,128);
    u8g2_DrawVLine(&(fezui.u8g2),64,MARGIN_UP,CHART_HEIGHT+MARGIN_DOWN);
    u8g2_DrawVLine(&(fezui.u8g2),32,HEIGHT-MARGIN_DOWN,MARGIN_DOWN);
    u8g2_DrawVLine(&(fezui.u8g2),96,HEIGHT-MARGIN_DOWN,MARGIN_DOWN);
}

void homepage_load(lefl_page_t *page)
{
    keys[0].key_cb=lambda(void,(lefl_key_t*k){;});
    keys[1].key_cb=lambda(void,(lefl_key_t*k){;});
    keys[2].key_cb=lambda(void,(lefl_key_t*k){;});
    keys[3].key_cb=lambda(void,(lefl_key_t*k){;});
    keys[4].key_cb=lambda(void,(lefl_key_t*k){;});
    keys[5].key_cb=lambda(void,(lefl_key_t*k){;});
    keys[6].key_cb=lambda(void,(lefl_key_t*k){;});
    keys[7].key_cb=lambda(void,(lefl_key_t*k){;});

}
