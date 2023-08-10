/*
 * fezui_keyconfigpage.c
 *
 *  Created on: 2023年6月19日
 *      Author: xq123
 */
#include "fezui.h"
#include "fezui_var.h"

#define ROW_HEIGHT 8

bool configing = false;

lefl_menu_t keyconfigmenu;
const char* keyconfigmenu_items[] = {"KEY1","KEY2","KEY3","KEY4","SHIFT","ALPHA","KNOB","WHEEL"};

lefl_menu_t keyconfig_digital_mode_menu;
const char* keyconfig_digital_mode_menu_items[] = {"Bindings","Mode"};

lefl_menu_t keyconfig_analog_normal_mode_menu;
const char* keyconfig_analog_normal_mode_menu_items[] = {"Bindings","Mode","Trigger distance","Schmitt parameter","Upper deadzone","Lower deadzone"};

lefl_menu_t keyconfig_analog_rapid_mode_menu;
const char* keyconfig_analog_rapid_mode_menu_items[] = {"Bindings","Mode","Trigger distance","Release distance","Schmitt parameter","Upper deadzone","Lower deadzone"};

lefl_menu_t keyconfig_analog_speed_mode_menu;
const char* keyconfig_analog_speed_mode_menu_items[] = {"Bindings","Mode","Trigger speed","Release speed","Upper deadzone","Lower deadzone"};

lefl_menu_t *current_menu=&keyconfig_digital_mode_menu;

static float deltay=0;
static float contentDeltaY=0;
static float contentTargetDeltaY=0;
//static float targety=0;
static float deltax=64;
//static float targetx=0;
static uint8_t selection1=0;
static lefl_animation_base_t keyconfiganimationx={
        .easing_func=lefl_animation_bounce_ease,
        .from=100,
        .to=0,
        .end=100,
        .tick=0,
        .parameter1=3.0,
        .parameter2=3.0,
        .mode=LEFL_ANIMATION_EASE_OUT,
        .target=&deltax,
};

static lefl_animation_base_t keyconfiganimationy={
        .easing_func=lefl_animation_elastic_ease,
        .end=100,
        .tick=0,
        .parameter1=3.0,
        .parameter2=3.0,
        .mode=LEFL_ANIMATION_EASE_OUT,
        .target=&deltay,
};


static lefl_cursor_t selectedkey={0,0,0,0};
static lefl_cursor_t targetselectedkey={27,32-4,WIDTH-27,ROW_HEIGHT};

void draw_pad()
{
    u8g2_DrawRFrame(&(fezui.u8g2), (uint8_t)deltax+40, 6, 86, 54, 3);
    u8g2_DrawFrame(&(fezui.u8g2), (uint8_t)deltax+40+30, 9, 50, 22);
    u8g2_DrawFrame(&(fezui.u8g2), (uint8_t)deltax+40+73, 3, 6, 4);
    u8g2_DrawFrame(&(fezui.u8g2), (uint8_t)deltax+40+60, 3, 6, 4);
    if(keys[1].state)
        u8g2_DrawBox(&(fezui.u8g2), (uint8_t)deltax+40+73, 59, 6, 4);
    else
        u8g2_DrawFrame(&(fezui.u8g2), (uint8_t)deltax+40+73, 59, 6, 4);
    if(keys[0].state)
        u8g2_DrawBox(&(fezui.u8g2), (uint8_t)deltax+40+60, 59, 6, 4);
    else
        u8g2_DrawFrame(&(fezui.u8g2), (uint8_t)deltax+40+60, 59, 6, 4);
    if(Keyboard_AdvancedKeys[0].key.state)
        u8g2_DrawBox(&(fezui.u8g2), (uint8_t)deltax+44+20*0, 6+32, 19, 19);
    else
        u8g2_DrawFrame(&(fezui.u8g2), (uint8_t)deltax+44+20*0, 6+32, 19, 19);
    if(Keyboard_AdvancedKeys[1].key.state)
        u8g2_DrawBox(&(fezui.u8g2), (uint8_t)deltax+44+20*1, 6+32, 19, 19);
    else
        u8g2_DrawFrame(&(fezui.u8g2), (uint8_t)deltax+44+20*1, 6+32, 19, 19);
    if(Keyboard_AdvancedKeys[2].key.state)
        u8g2_DrawBox(&(fezui.u8g2), (uint8_t)deltax+44+20*2, 6+32, 19, 19);
    else
        u8g2_DrawFrame(&(fezui.u8g2), (uint8_t)deltax+44+20*2, 6+32, 19, 19);
    if(Keyboard_AdvancedKeys[3].key.state)
        u8g2_DrawBox(&(fezui.u8g2), (uint8_t)deltax+44+20*3, 6+32, 19, 19);
    else
        u8g2_DrawFrame(&(fezui.u8g2), (uint8_t)deltax+44+20*3, 6+32, 19, 19);
    u8g2_DrawCircle(&(fezui.u8g2), (uint8_t)deltax+44, 32, 7, U8G2_DRAW_ALL);
    u8g2_DrawCircle(&(fezui.u8g2), (uint8_t)deltax+55, 20, 7, U8G2_DRAW_ALL);
}

void keyconfigpage_init()
{
    lefl_menu_init(&keyconfigmenu, keyconfigmenu_items, sizeof(keyconfigmenu_items)/sizeof(const char*), NULL);
    lefl_menu_init(&keyconfig_digital_mode_menu, keyconfig_digital_mode_menu_items, sizeof(keyconfig_digital_mode_menu_items)/sizeof(const char*), NULL);
    lefl_menu_init(&keyconfig_analog_normal_mode_menu, keyconfig_analog_normal_mode_menu_items, sizeof(keyconfig_analog_normal_mode_menu_items)/sizeof(const char*), NULL);
    lefl_menu_init(&keyconfig_analog_rapid_mode_menu, keyconfig_analog_rapid_mode_menu_items, sizeof(keyconfig_analog_rapid_mode_menu_items)/sizeof(const char*), NULL);
    lefl_menu_init(&keyconfig_analog_speed_mode_menu, keyconfig_analog_speed_mode_menu_items, sizeof(keyconfig_analog_speed_mode_menu_items)/sizeof(const char*), NULL);
}

void keyconfigpage_logic(lefl_page_t *page)
{
    lefl_animation_tick(&keyconfiganimationx);
    lefl_animation_tick(&keyconfiganimationy);
    if(selection1!=keyconfigmenu.selected_index)
    {
        keyconfiganimationy.from = deltay;
        keyconfiganimationy.to = keyconfigmenu.selected_index*ROW_HEIGHT;
        lefl_animation_begin(&keyconfiganimationy);
    }
    selection1=keyconfigmenu.selected_index;
    contentTargetDeltaY = current_menu->selected_index*ROW_HEIGHT;
    lefl_easing_pid(&contentDeltaY,contentTargetDeltaY);
    lefl_cursor_move(&selectedkey, &targetselectedkey);
}

void keyconfigpage_draw(lefl_page_t *page)
{
    u8g2_SetFont(&(fezui.u8g2), u8g2_font_5x8_mr);
    for(uint8_t i=0;i<keyconfigmenu.len;i++)
    {
        u8g2_DrawStr(&(fezui.u8g2),1,32+3-(int16_t)deltay+i*ROW_HEIGHT,keyconfigmenu.items[i]);
    }
    fezui_draw_cursor(&fezui, &cursor);
    u8g2_DrawVLine(&(fezui.u8g2), 26, 0, 64);
    if(configing)
    {
        for(uint8_t i=0;i<(*current_menu).len;i++)
        {
            u8g2_DrawStr(&(fezui.u8g2),28,32+3-(int16_t)contentDeltaY+i*ROW_HEIGHT,(*current_menu).items[i]);
        }
        fezui_draw_cursor(&fezui, &selectedkey);
    }
    else
    {
        draw_pad();
    }
}

void keyconfigpage_load(lefl_page_t *page)
{
    keyconfiganimationy.from = 64;
    keyconfiganimationy.to = keyconfigmenu.selected_index*ROW_HEIGHT;
    keys[2].key_cb=lambda(void,(lefl_key_t*k)
        {
            if(configing)
            {
                configing = false;
            }
            else
            {
                lefl_link_frame_go_back(&mainframe);
            }
        });
    keys[3].key_cb=lambda(void,(lefl_key_t*k)
        {
            if(configing)
            {
                switch (current_menu->selected_index)
                {
                    case 0:

                        break;
                    default:
                        break;
                }
                lefl_link_frame_navigate(&mainframe, &keyselectpage);
            }
            else
            {
                configing=true;
                lefl_cursor_set(&selectedkey, 26, 0, 1, 64);
                if(keyconfigmenu.selected_index<MAIN_KEY_NUM)
                {
                    switch (Keyboard_AdvancedKeys[keyconfigmenu.selected_index].mode)
                    {
                        case LEFL_KEY_ANALOG_NORMAL_MODE:
                            current_menu = &keyconfig_analog_normal_mode_menu;
                            break;
                        case LEFL_KEY_ANALOG_RAPID_MODE:
                            current_menu = &keyconfig_analog_rapid_mode_menu;
                            break;
                        case LEFL_KEY_ANALOG_SPEED_MODE:
                            current_menu = &keyconfig_analog_speed_mode_menu;
                            break;
                        default:
                            break;
                    }
                }
                else
                {
                    current_menu = &keyconfig_digital_mode_menu;
                }
            }
        });
    keys[4].key_cb=lambda(void,(lefl_key_t*k)
        {
            if(configing)
            {
                lefl_menu_index_increase(current_menu, 1);
            }
            else
            {
                lefl_menu_index_increase(&keyconfigmenu, 1);
            }
        });

    keys[5].key_cb=lambda(void,(lefl_key_t*k)
        {
            if(configing)
            {
                lefl_menu_index_increase(current_menu, -1);
            }
            else
            {
                lefl_menu_index_increase(&keyconfigmenu, -1);
            }
        });
    keys[6].key_cb=lambda(void,(lefl_key_t*k)
        {
            if(configing)
            {
                lefl_menu_index_increase(current_menu, 1);
            }
            else
            {
                lefl_menu_index_increase(&keyconfigmenu, 1);
            }
        });
    keys[7].key_cb=lambda(void,(lefl_key_t*k)
        {
            if(configing)
            {
                lefl_menu_index_increase(current_menu, -1);
            }
            else
            {
                lefl_menu_index_increase(&keyconfigmenu, -1);
            }
        });
    lefl_animation_begin(&keyconfiganimationy);
    lefl_animation_begin(&keyconfiganimationx);
    lefl_cursor_set(&target_cursor, 0,32-4,26,ROW_HEIGHT);
}

lefl_page_t keyconfigpage={keyconfigpage_logic,keyconfigpage_draw,keyconfigpage_load};
