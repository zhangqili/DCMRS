/*
 * fezui_settingspage.c
 *
 *  Created on: 2023年6月19日
 *      Author: xq123
 */
#include "fezui.h"
#include "fezui_var.h"
lefl_menu_t settingsmenu;
const char* settingsmenu_items[] = {"Keys Configuration","Calibration","Debug","About"};
lefl_page_t settingspage={settingspage_logic,settingspage_draw,settingspage_load};


void settingspage_init()
{
    lefl_menu_init(&settingsmenu, settingsmenu_items, sizeof(settingsmenu_items)/sizeof(const char*), settings_menu_cb);
}

void settingspage_logic(lefl_page_t *page)
{
    lefl_cursor_set(&target_cursor, 3,
            settingsmenu.selected_index * ITEM_HEIGHT + 3,
            strlen(settingsmenu.items[settingsmenu.selected_index]) * 6 + 6,
            ITEM_HEIGHT);
}
void settingspage_draw(lefl_page_t *page)
{
    u8g2_SetFont(&(fezui.u8g2), u8g2_font_6x13_tf);
    for(uint8_t i=0;i<settingsmenu.len;i++)
    {
        u8g2_DrawStr(&(fezui.u8g2),5,ITEM_HEIGHT*(i+1),settingsmenu.items[i]);
    }
    fezui_draw_cursor(&fezui, &cursor);
}
void settings_menu_cb(lefl_menu_t *menu)
{

    switch (menu->selected_index)
    {
    case 0:
        lefl_link_frame_navigate(&mainframe, &keyconfigpage);
        break;
    case 1:
        lefl_link_frame_navigate(&mainframe, &calibrationpage);
        break;
    case 2:
        lefl_link_frame_navigate(&mainframe, &debugpage);
        break;
    case 3:
        lefl_link_frame_navigate(&mainframe, &aboutpage);
        break;
    default:
        break;
    }
}
void settingspage_load(lefl_page_t *page)
{
    keys[2].key_cb=lambda(void,(lefl_key_t*k){lefl_link_frame_go_back(&mainframe);});
    keys[3].key_cb=lambda(void,(lefl_key_t*k){lefl_menu_click(&settingsmenu);});
    keys[4].key_cb=lambda(void,(lefl_key_t*k){lefl_menu_index_increase(&settingsmenu, 1);});
    keys[5].key_cb=lambda(void,(lefl_key_t*k){lefl_menu_index_increase(&settingsmenu, -1);});
    keys[6].key_cb=lambda(void,(lefl_key_t*k){lefl_menu_index_increase(&settingsmenu, 1);});
    keys[7].key_cb=lambda(void,(lefl_key_t*k){lefl_menu_index_increase(&settingsmenu, -1);});
}
