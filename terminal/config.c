#include "main.h"
#include "temt6000.h"
#include "driver_sgp30_advance.h"
#include "driver_sgp30_basic.h"

#include "lv_conf.h"
#include "lvgl.h"

#include "lv_port_disp.h"
#include "lv_port_indev.h"

#include "lcd.h"

#include "bl_fw_api.h"
#include "wifi_mgmr_ext.h"
#include "wifi_mgmr.h"

#define DBG_TAG "MAIN"
#include "log.h"

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"

