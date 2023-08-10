/*
 * fezui_menupage.c
 *
 *  Created on: 2023年6月19日
 *      Author: xq123
 */

#include "fezui.h"
#include "fezui_var.h"

lefl_page_t menupage={menupage_logic,menupage_draw,menupage_load};

lefl_menu_t mainmenu;
const char* mainmenu_items[] = {"Home","Oscilloscope","Statistic","Settings"};


float menu_offset;

lefl_animation_base_t menuanimation={
        .easing_func=lefl_animation_cubic_ease,
        .from=100,
        .to=0,
        .end=100,
        .tick=0,
        .parameter1=3.0,
        .parameter2=3.0,
        .mode=LEFL_ANIMATION_EASE_OUT,
        .target=&menu_offset,
};

void menupage_init()
{
    lefl_menu_init(&mainmenu, mainmenu_items, sizeof(mainmenu_items)/sizeof(const char*), main_menu_cb);
}

void menupage_logic(lefl_page_t *page)
{
    lefl_animation_tick(&menuanimation);
    //fezui_menu_update_selection(&mainmenu);
    /*
    if(lefl_key_is_triggered(keys+3))
    {
        lefl_menu_click(&mainmenu);
        //Keyboard_Triggers[7]=0;
    }
    if(lefl_key_is_triggered(keys+2))
    {
        lefl_link_frame_go_back(&mainframe);
        lefl_cursor_set(&cursor ,0 ,0 ,WIDTH ,HEIGHT);
    }
    */
    lefl_cursor_set(
            &target_cursor ,
            3+(int16_t)menu_offset,
            mainmenu.selected_index*ITEM_HEIGHT+3 ,
            strlen(mainmenu.items[mainmenu.selected_index])*6+6,
            ITEM_HEIGHT);
}
void menupage_draw(lefl_page_t *page)
{
    u8g2_SetFont(&(fezui.u8g2), u8g2_font_6x13_tf);
    for(uint8_t i=0;i<mainmenu.len;i++)
    {
        u8g2_DrawStr(&(fezui.u8g2),5 + (int16_t)menu_offset,ITEM_HEIGHT*(i+1),mainmenu.items[i]);
    }
    fezui_draw_cursor(&fezui, &cursor);

}

void main_menu_cb(lefl_menu_t *menu)
{

    switch (menu->selected_index)
    {
    case 0:
        lefl_link_frame_go_back(&mainframe);
        break;
    case 1:
        lefl_link_frame_navigate(&mainframe, &oscilloscopepage);
        break;
    case 2:
    	lefl_link_frame_navigate(&mainframe, &statisticpage);
        break;
    case 3:
        lefl_link_frame_navigate(&mainframe, &settingspage);
        break;
    default:
        break;
    }
}

void menupage_load(lefl_page_t *page)
{
    lefl_animation_begin(&menuanimation);

    keys[2].key_cb=lambda(void,(lefl_key_t*k){lefl_link_frame_go_back(&mainframe);lefl_cursor_set(&cursor ,0 ,0 ,WIDTH ,HEIGHT);});
    keys[3].key_cb=lambda(void,(lefl_key_t*k){lefl_menu_click(&mainmenu);});
    keys[4].key_cb=lambda(void,(lefl_key_t*k){lefl_menu_index_increase(&mainmenu, 1);});
    keys[5].key_cb=lambda(void,(lefl_key_t*k){lefl_menu_index_increase(&mainmenu, -1);});
    keys[6].key_cb=lambda(void,(lefl_key_t*k){lefl_menu_index_increase(&mainmenu, 1);});
    keys[7].key_cb=lambda(void,(lefl_key_t*k){lefl_menu_index_increase(&mainmenu, -1);});
}

