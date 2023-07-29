// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.3.0
// LVGL version: 8.3.6
// Project name: SquareLine_Project

#include "ui.h"
#include "main.h"

#define METER_SIZE 80

static void car_btn_event_handler(lv_event_t * e)
{
    lv_scr_load(ui_Screen3);
}

static char tempsubstr[16];
static char tempstr[512];
static void meter_event_cb(lv_event_t * e)
{
    /*The original target of the event. Can be the buttons or the container*/
    lv_obj_t * target = lv_event_get_target(e);

    /*The current target is always the container as the event is added to it*/
    lv_obj_t * cont = lv_event_get_current_target(e);

    memset(tempstr, 0, sizeof(tempstr));
    /*If container was clicked do nothing*/
    //if(target == cont) return;
    strcat(tempstr,"\n\n\n");

    /*Make the clicked buttons red*/
    if(target==tempmeter)
    {
        for(int8_t i = 0; i < 10; i++)
        {
            lv_chart_set_next_value(ui_Chart2, mainser, lefl_loop_array_get(&temp_history,i));
            sprintf(tempsubstr,"%.1f\t",lefl_loop_array_get(&temp_history,i));
            strcat(tempstr,tempsubstr);
            strcat(tempstr,dates[temp_history.index-i>=0?temp_history.index-i:temp_history.index-i+16]);
            strcat(tempstr,"\n");
        //lv_chart_set_next_value(chart1, ser2, lv_rand(30, 70));
        }
        lv_chart_set_range(ui_Chart2, LV_CHART_AXIS_PRIMARY_Y, -40, 55);
        lv_label_set_text(historylabel,tempstr);
    }
    if(target==humimeter)
    {

        for(int8_t i = 0; i < 10; i++)
        {
            lv_chart_set_next_value(ui_Chart2, mainser, lefl_loop_array_get(&humi_history,i));
            sprintf(tempsubstr,"%.1f\n",lefl_loop_array_get(&humi_history,i));
            strcat(tempstr,tempsubstr);
            lv_chart_set_range(ui_Chart2, LV_CHART_AXIS_PRIMARY_Y, 0, 100);
            strcat(tempstr,dates[humi_history.index-i>=0?humi_history.index-i:humi_history.index-i+16]);
            strcat(tempstr,"\n");
        //lv_chart_set_next_value(chart1, ser2, lv_rand(30, 70));
        }
        lv_label_set_text(historylabel,tempstr);
    }
    if(target==sgp30meter)
    {
        for(int8_t i = 0; i < 10; i++)
        {
            lv_chart_set_next_value(ui_Chart2, mainser, lefl_loop_array_get(&co2_history,i));
            sprintf(tempsubstr,"%.1f\n",lefl_loop_array_get(&co2_history,i));
            strcat(tempstr,tempsubstr);
            lv_chart_set_range(ui_Chart2, LV_CHART_AXIS_PRIMARY_Y, 400, 2000);
            strcat(tempstr,dates[co2_history.index-i>=0?co2_history.index-i:co2_history.index-i+16]);
            strcat(tempstr,"\n");
        //lv_chart_set_next_value(chart1, ser2, lv_rand(30, 70));
        }
        lv_label_set_text(historylabel,tempstr);

    }
    if(target==temt6000meter)
    {
        for(int8_t i = 0; i < 10; i++)
        {
            lv_chart_set_next_value(ui_Chart2, mainser, lefl_loop_array_get(&light_history,i));
            sprintf(tempsubstr,"%.1f\n",lefl_loop_array_get(&light_history,i));
            strcat(tempstr,tempsubstr);
            lv_chart_set_range(ui_Chart2, LV_CHART_AXIS_PRIMARY_Y, 0, 10000);
            strcat(tempstr,dates[light_history.index-i>=0?light_history.index-i:light_history.index-i+16]);
            strcat(tempstr,"\n");
        //lv_chart_set_next_value(chart1, ser2, lv_rand(30, 70));
        }
        lv_label_set_text(historylabel,tempstr);

    }
    lv_scr_load(ui_Screen2);
    //lv_obj_set_style_bg_color(target, lv_palette_main(LV_PALETTE_RED), 0);
}
void ui_Screen1_screen_init(void)
{
    ui_Screen1 = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_Screen1, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Panel1 = lv_obj_create(ui_Screen1);
    lv_obj_set_width(ui_Panel1, lv_pct(100));
    lv_obj_set_height(ui_Panel1, lv_pct(15));
    lv_obj_set_align(ui_Panel1, LV_ALIGN_TOP_MID);
    //lv_obj_set_flex_flow(ui_Panel1, LV_FLEX_FLOW_ROW);
    //lv_obj_set_flex_align(ui_Panel1, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_clear_flag(ui_Panel1, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Label2 = lv_label_create(ui_Panel1);
    lv_obj_set_width(ui_Label2, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label2, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_Label2, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label2, "总览");
    lv_obj_set_style_text_font(ui_Label2,&lv_font_Chinese_src_regular,LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Panel2 = lv_obj_create(ui_Screen1);
    lv_obj_set_height(ui_Panel2, 220);
    lv_obj_set_width(ui_Panel2, lv_pct(100));
    lv_obj_set_align(ui_Panel2, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_Panel2, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    tempmeter = lv_meter_create(ui_Panel2);
    lv_obj_set_width(tempmeter, METER_SIZE);
    lv_obj_set_height(tempmeter, METER_SIZE);
    lv_obj_set_y(tempmeter, -10);
    lv_obj_set_align(tempmeter, LV_ALIGN_TOP_LEFT);
    lv_obj_clear_flag(tempmeter, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_meter_scale_t*tempscale=lv_meter_add_scale(tempmeter);

    /*Add a scale first*/
    lv_meter_set_scale_ticks(tempmeter,tempscale, 41, 2, 10, lv_palette_main(LV_PALETTE_GREY));
    //lv_meter_set_scale_major_ticks(tempmeter,tempscale, 8, 4, 15, lv_color_black(), 10);


    /*Add a blue arc to the start*/
    tempindic = lv_meter_add_arc(tempmeter,tempscale, 3, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_meter_set_indicator_start_value(tempmeter, tempindic, 0);
    lv_meter_set_indicator_end_value(tempmeter, tempindic, 20);

    /*Make the tick lines blue at the start of the scale*/
    tempindic = lv_meter_add_scale_lines(tempmeter,tempscale, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_BLUE),
                                     false, 0);
    lv_meter_set_indicator_start_value(tempmeter, tempindic, 0);
    lv_meter_set_indicator_end_value(tempmeter, tempindic, 20);

    /*Add a red arc to the end*/
    tempindic = lv_meter_add_arc(tempmeter,tempscale, 3, lv_palette_main(LV_PALETTE_RED), 0);
    lv_meter_set_indicator_start_value(tempmeter, tempindic, 80);
    lv_meter_set_indicator_end_value(tempmeter, tempindic, 100);

    /*Make the tick lines red at the end of the scale*/
    tempindic = lv_meter_add_scale_lines(tempmeter,tempscale, lv_palette_main(LV_PALETTE_RED), lv_palette_main(LV_PALETTE_RED), false,
                                     0);
    lv_meter_set_indicator_start_value(tempmeter, tempindic, 80);
    lv_meter_set_indicator_end_value(tempmeter, tempindic, 100);

    /*Add a needle line indicator*/
    tempindic = lv_meter_add_needle_line(tempmeter,tempscale, 4, lv_palette_main(LV_PALETTE_GREY), -10);

    humimeter = lv_meter_create(ui_Panel2);
    lv_obj_set_width(humimeter, METER_SIZE);
    lv_obj_set_height(humimeter, METER_SIZE);
    lv_obj_set_y(humimeter, -10);
    lv_obj_set_align(humimeter, LV_ALIGN_TOP_RIGHT);
    lv_obj_clear_flag(humimeter, LV_OBJ_FLAG_SCROLLABLE); 
    lv_meter_scale_t*humiscale=lv_meter_add_scale(humimeter);

    /*Add a scale first*/
    lv_meter_set_scale_ticks(humimeter,humiscale, 41, 2, 10, lv_palette_main(LV_PALETTE_GREY));
    //lv_meter_set_scale_major_ticks(humimeter,humiscale, 8, 4, 15, lv_color_black(), 10);


    /*Add a blue arc to the start*/
    humiindic = lv_meter_add_arc(humimeter,humiscale, 3, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_meter_set_indicator_start_value(humimeter, humiindic, 0);
    lv_meter_set_indicator_end_value(humimeter, humiindic, 20);

    /*Make the tick lines blue at the start of the scale*/
    humiindic = lv_meter_add_scale_lines(humimeter,humiscale, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_BLUE),
                                     false, 0);
    lv_meter_set_indicator_start_value(humimeter, humiindic, 0);
    lv_meter_set_indicator_end_value(humimeter, humiindic, 20);

    /*Add a red arc to the end*/
    humiindic = lv_meter_add_arc(humimeter,humiscale, 3, lv_palette_main(LV_PALETTE_RED), 0);
    lv_meter_set_indicator_start_value(humimeter, humiindic, 80);
    lv_meter_set_indicator_end_value(humimeter, humiindic, 100);

    /*Make the tick lines red at the end of the scale*/
    humiindic = lv_meter_add_scale_lines(humimeter,humiscale, lv_palette_main(LV_PALETTE_RED), lv_palette_main(LV_PALETTE_RED), false,
                                     0);
    lv_meter_set_indicator_start_value(humimeter, humiindic, 80);
    lv_meter_set_indicator_end_value(humimeter, humiindic, 100);

    /*Add a needle line indicator*/
    humiindic = lv_meter_add_needle_line(humimeter,humiscale, 4, lv_palette_main(LV_PALETTE_GREY), -10);
    


    sgp30meter = lv_meter_create(ui_Panel2);
    lv_obj_set_width(sgp30meter, METER_SIZE);
    lv_obj_set_height(sgp30meter, METER_SIZE);
    lv_obj_set_y(sgp30meter, -10);
    lv_obj_set_align(sgp30meter, LV_ALIGN_BOTTOM_LEFT);
    lv_obj_clear_flag(sgp30meter, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_meter_scale_t*sgp30scale=lv_meter_add_scale(sgp30meter);

    /*Add a scale first*/
    lv_meter_set_scale_ticks(sgp30meter,sgp30scale, 41, 2, 10, lv_palette_main(LV_PALETTE_GREY));
    //lv_meter_set_scale_major_ticks(sgp30meter,sgp30scale, 8, 4, 15, lv_color_black(), 10);


    /*Add a blue arc to the start*/
    sgp30indic = lv_meter_add_arc(sgp30meter,sgp30scale, 3, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_meter_set_indicator_start_value(sgp30meter, sgp30indic, 0);
    lv_meter_set_indicator_end_value(sgp30meter, sgp30indic, 20);

    /*Make the tick lines blue at the start of the scale*/
    sgp30indic = lv_meter_add_scale_lines(sgp30meter,sgp30scale, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_BLUE),
                                     false, 0);
    lv_meter_set_indicator_start_value(sgp30meter, sgp30indic, 0);
    lv_meter_set_indicator_end_value(sgp30meter, sgp30indic, 20);

    /*Add a red arc to the end*/
    sgp30indic = lv_meter_add_arc(sgp30meter,sgp30scale, 3, lv_palette_main(LV_PALETTE_RED), 0);
    lv_meter_set_indicator_start_value(sgp30meter, sgp30indic, 80);
    lv_meter_set_indicator_end_value(sgp30meter, sgp30indic, 100);

    /*Make the tick lines red at the end of the scale*/
    sgp30indic = lv_meter_add_scale_lines(sgp30meter,sgp30scale, lv_palette_main(LV_PALETTE_RED), lv_palette_main(LV_PALETTE_RED), false,
                                     0);
    lv_meter_set_indicator_start_value(sgp30meter, sgp30indic, 80);
    lv_meter_set_indicator_end_value(sgp30meter, sgp30indic, 100);

    /*Add a needle line indicator*/
    sgp30indic = lv_meter_add_needle_line(sgp30meter,sgp30scale, 4, lv_palette_main(LV_PALETTE_GREY), -10);

    temt6000meter = lv_meter_create(ui_Panel2);
    lv_obj_set_width(temt6000meter, METER_SIZE);
    lv_obj_set_height(temt6000meter, METER_SIZE);
    lv_obj_set_y(temt6000meter, -10);
    lv_obj_set_align(temt6000meter, LV_ALIGN_BOTTOM_RIGHT);
    lv_obj_clear_flag(temt6000meter, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    lv_meter_scale_t*temt6000scale=lv_meter_add_scale(temt6000meter);

    /*Add a scale first*/
    lv_meter_set_scale_ticks(temt6000meter,temt6000scale, 41, 2, 10, lv_palette_main(LV_PALETTE_GREY));
    //lv_meter_set_scale_major_ticks(temt6000meter,temt6000scale, 8, 4, 15, lv_color_black(), 10);


    /*Add a blue arc to the start*/
    temt6000indic = lv_meter_add_arc(temt6000meter,temt6000scale, 3, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_meter_set_indicator_start_value(temt6000meter, temt6000indic, 0);
    lv_meter_set_indicator_end_value(temt6000meter, temt6000indic, 20);

    /*Make the tick lines blue at the start of the scale*/
    temt6000indic = lv_meter_add_scale_lines(temt6000meter,temt6000scale, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_BLUE),
                                     false, 0);
    lv_meter_set_indicator_start_value(temt6000meter, temt6000indic, 0);
    lv_meter_set_indicator_end_value(temt6000meter, temt6000indic, 20);

    /*Add a red arc to the end*/
    temt6000indic = lv_meter_add_arc(temt6000meter,temt6000scale, 3, lv_palette_main(LV_PALETTE_RED), 0);
    lv_meter_set_indicator_start_value(temt6000meter, temt6000indic, 80);
    lv_meter_set_indicator_end_value(temt6000meter, temt6000indic, 100);

    /*Make the tick lines red at the end of the scale*/
    temt6000indic = lv_meter_add_scale_lines(temt6000meter,temt6000scale, lv_palette_main(LV_PALETTE_RED), lv_palette_main(LV_PALETTE_RED), false,
                                     0);
    lv_meter_set_indicator_start_value(temt6000meter, temt6000indic, 80);
    lv_meter_set_indicator_end_value(temt6000meter, temt6000indic, 100);

    /*Add a needle line indicator*/
    temt6000indic = lv_meter_add_needle_line(temt6000meter,temt6000scale, 4, lv_palette_main(LV_PALETTE_GREY), -10);

    ui_Button2 = lv_btn_create(ui_Screen1);
    lv_obj_set_width(ui_Button2, lv_pct(100));
    lv_obj_set_height(ui_Button2, lv_pct(15));
    lv_obj_set_align(ui_Button2, LV_ALIGN_BOTTOM_MID);
    lv_obj_add_flag(ui_Button2, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_Button2, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_add_event_cb(ui_Button2,car_btn_event_handler,LV_EVENT_CLICKED,NULL);

    ui_Label1 = lv_label_create(ui_Button2);
    lv_obj_set_width(ui_Label1, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label1, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_Label1, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label1, "控制");
    lv_obj_set_style_text_font(ui_Label1,&lv_font_Chinese_src_regular,LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_event_cb(tempmeter,meter_event_cb,LV_EVENT_CLICKED,NULL);
    lv_obj_add_event_cb(humimeter,meter_event_cb,LV_EVENT_CLICKED,NULL);
    lv_obj_add_event_cb(sgp30meter,meter_event_cb,LV_EVENT_CLICKED,NULL);
    lv_obj_add_event_cb(temt6000meter,meter_event_cb,LV_EVENT_CLICKED,NULL);

    
    ui_Label9 = lv_label_create(ui_Screen1);
    lv_obj_set_width(ui_Label9, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label9, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Label9, -64);
    lv_obj_set_y(ui_Label9, -10);
    lv_obj_set_align(ui_Label9, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label9, "温度");
    lv_obj_set_style_text_font(ui_Label9,&lv_font_Chinese_src_regular,LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Label10 = lv_label_create(ui_Screen1);
    lv_obj_set_width(ui_Label10, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label10, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Label10, 64);
    lv_obj_set_y(ui_Label10, -10);
    lv_obj_set_align(ui_Label10, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label10, "湿度");
    lv_obj_set_style_text_font(ui_Label10,&lv_font_Chinese_src_regular,LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Label11 = lv_label_create(ui_Screen1);
    lv_obj_set_width(ui_Label11, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label11, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Label11, -64);
    lv_obj_set_y(ui_Label11, 96);
    lv_obj_set_align(ui_Label11, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label11, "二氧化碳");
    lv_obj_set_style_text_font(ui_Label11,&lv_font_Chinese_src_regular,LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Label12 = lv_label_create(ui_Screen1);
    lv_obj_set_width(ui_Label12, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label12, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Label12, 64);
    lv_obj_set_y(ui_Label12, 96);
    lv_obj_set_align(ui_Label12, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label12, "光照强度");
    lv_obj_set_style_text_font(ui_Label12,&lv_font_Chinese_src_regular,LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Connect_Label = lv_label_create(ui_Panel1);
    lv_obj_set_width(ui_Connect_Label, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Connect_Label, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_Connect_Label, LV_ALIGN_RIGHT_MID);
    lv_obj_set_style_text_font(ui_Connect_Label,&lv_font_Chinese_src_regular,LV_PART_MAIN | LV_STATE_DEFAULT);
}
