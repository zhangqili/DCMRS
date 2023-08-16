/*
 * fezui_debugpage.c
 *
 *  Created on: 2023年6月24日
 *      Author: xq123
 */
#include "fezui.h"
#include "fezui_var.h"

lefl_page_t debugpage={debugpage_logic,debugpage_draw,debugpage_load};
static float target_ordinate=0;
static float target_abscissa=0;

static float targetnum = 50;
fezui_slider_t slider =
{
	.interval=1,
	.max=256,
	.min=0,
	.f_ptr=&targetnum,
	.orientation=ORIENTATION_VERTICAL
};

static fezui_scrollview_t scrollview =
{
    .content_height=120,
    .content_width=250,
    .abscissa=0,
    .ordinate=0
};

static fezui_rolling_number_t rolling_number=
{
    .digit=5,
    .number=0,
    .font = u8g2_font_micro_mn,
};

void debugpage_logic(lefl_page_t *page)
{
    lefl_easing_pid(&(scrollview.abscissa), target_abscissa);
    lefl_easing_pid(&(scrollview.ordinate), target_ordinate);
    fezui_rolling_number_update(&fezui, &rolling_number);
}
void debugpage_draw(lefl_page_t *page)
{
	//fezui_veil(&(fezui), 0, 0, 128, 64, 4, 0);
	//fezui_draw_dialog(&(fezui), 32, 16, 64, 32, NULL);
	fezui_draw_slider(&(fezui), 64-(u8g2_int_t)scrollview.abscissa, 16+2-(u8g2_int_t)scrollview.ordinate , 5, 20, &slider);
    u8g2_SetFont(&(fezui.u8g2), u8g2_font_freedoomr10_mu);
    sprintf(fezui_buffer,"%f", targetnum);
    u8g2_DrawStr(&(fezui.u8g2), 0-(u8g2_int_t)scrollview.abscissa, 64- (u8g2_int_t)scrollview.ordinate, fezui_buffer);
    u8g2_DrawStr(&(fezui.u8g2), 10-(u8g2_int_t)scrollview.abscissa, 10- (u8g2_int_t)scrollview.ordinate, "123");
    u8g2_DrawBox(&(fezui.u8g2), 70-(u8g2_int_t)scrollview.abscissa, 5- (u8g2_int_t)scrollview.ordinate, 10, 10);
    u8g2_DrawHLine(&(fezui.u8g2), 3-(u8g2_int_t)scrollview.abscissa, 40- (u8g2_int_t)scrollview.ordinate, 100);
    fezui_draw_scrollview(&fezui, 0, 0, 128, 64, &scrollview);
    //u8g2_SetFont(&(fezui.u8g2), fez_font_6x10_m);
    fezui_draw_rolling_number(&fezui, 96-(u8g2_int_t)scrollview.abscissa, 50- (u8g2_int_t)scrollview.ordinate, &rolling_number);
    //u8g2_DrawStr(&(fezui.u8g2), 96-(u8g2_int_t)scrollview.abscissa, 63- (u8g2_int_t)scrollview.ordinate, "1");
    //u8g2_DrawStr(&(fezui.u8g2), 96+u8g2_GetMaxCharWidth(&(fezui.u8g2))*1-(u8g2_int_t)scrollview.abscissa, 63- (u8g2_int_t)scrollview.ordinate, "2");
    //u8g2_DrawStr(&(fezui.u8g2), 96+u8g2_GetMaxCharWidth(&(fezui.u8g2))*2-(u8g2_int_t)scrollview.abscissa, 63- (u8g2_int_t)scrollview.ordinate, "3");
    //u8g2_SetContrast(&(fezui.u8g2), targetnum);
}

void debugpage_load(lefl_page_t *page)
{
}

