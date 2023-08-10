/*
 * fezui_aboutpage.c
 *
 *  Created on: 2023年6月26日
 *      Author: xq123
 */
#include "fezui.h"
#include "fezui_var.h"

#define PROJECT_URL "https://github.com/zhangqili/gemini-pad"

lefl_page_t aboutpage={aboutpage_logic,aboutpage_draw,aboutpage_load};

void aboutpage_logic(lefl_page_t *page)
{

}
void aboutpage_draw(lefl_page_t *page)
{
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
}

void aboutpage_load(lefl_page_t *page)
{
    keys[2].key_cb=lambda(void,(lefl_key_t*k){lefl_link_frame_go_back(&mainframe);lefl_cursor_set(&cursor ,0 ,0 ,WIDTH ,HEIGHT);});
    keys[3].key_cb=NULL;
    keys[4].key_cb=NULL;
    keys[5].key_cb=NULL;
    keys[6].key_cb=NULL;
    keys[7].key_cb=NULL;
}


