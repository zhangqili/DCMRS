/*
 * fezui_config.h
 *
 *  Created on: 2023年6月19日
 *      Author: xq123
 */

#ifndef FEZUI_CONFIG_H_
#define FEZUI_CONFIG_H_

#define WIDTH                 128
#define HEIGHT                64
#define BUFFER_LENGTH         64
#define NUMBER_STRING_LENGTH  BUFFER_LENGTH
#define REFRESH_RATE          170
#define KPS_HISTORY_LENGTH    65
//#define TILE_LENGTH         56
#define SCREEN_REST_TIME      60
#define KEY_NUM               12
#define MAIN_KEY_NUM          4
#define ITEM_HEIGHT           14
#define HISTORY_LENGTH        65

#define _SCREEN_REST_ON
#define _FPS_ON

#define roll()                rand()%2;


void Analog_Read();
void fezui_read_counts();
void fezui_save_counts();


extern lefl_link_frame_t mainframe;


extern lefl_page_t homepage;
extern lefl_menu_t mainmenu;
void homepage_logic(lefl_page_t *page);
void homepage_draw(lefl_page_t *page);
void homepage_load(lefl_page_t *page);
void homepage_init();

extern lefl_page_t debugpage;
void debugpage_logic(lefl_page_t *page);
void debugpage_draw(lefl_page_t *page);
void debugpage_load(lefl_page_t *page);

#endif /* FEZUI_CONFIG_H_ */
