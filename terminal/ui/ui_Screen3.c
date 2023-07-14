// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.3.0
// LVGL version: 8.3.6
// Project name: SquareLine_Project

#include "ui.h"

static void back_event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    lv_scr_load(ui_Screen1);
}

void ui_Screen3_screen_init(void)
{
    ui_Screen3 = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_Screen3, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Panel9 = lv_obj_create(ui_Screen3);
    lv_obj_set_width(ui_Panel9, lv_pct(100));
    lv_obj_set_height(ui_Panel9, lv_pct(15));
    lv_obj_set_align(ui_Panel9, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(ui_Panel9, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Label4 = lv_label_create(ui_Panel9);
    lv_obj_set_width(ui_Label4, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label4, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_Label4, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label4, "Car");

    ui_ImgButton1 = lv_btn_create(ui_Panel9);
    lv_obj_set_height(ui_ImgButton1, 32);
    lv_obj_set_width(ui_ImgButton1, 72);
    lv_obj_set_align(ui_ImgButton1, LV_ALIGN_LEFT_MID);
    lv_obj_t * backlabel = lv_label_create(ui_ImgButton1);
    lv_label_set_text(backlabel, "< Back");
    lv_obj_center(backlabel);
    //lv_img_set_src(ui_ImgButton1, LV_SYMBOL_BACKSPACE);
    lv_obj_add_event_cb(ui_ImgButton1,back_event_handler,LV_EVENT_CLICKED,NULL);


}
