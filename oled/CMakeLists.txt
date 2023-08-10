cmake_minimum_required(VERSION 3.15)

include(proj.conf)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

aux_source_directory(./u8g2/ u8g2_LIST)
aux_source_directory(./fezui/ fezui_LIST)
aux_source_directory(./lefl/ lefl_LIST)

target_sources(app PRIVATE ${SRC_LIST})
target_sources(app PRIVATE ${u8g2_LIST})
target_sources(app PRIVATE ${fezui_LIST})
target_sources(app PRIVATE ${lefl_LIST})
target_sources(app PRIVATE ./Display/display.c)


sdk_add_include_directories(./)
sdk_add_include_directories(./u8g2/)
sdk_add_include_directories(./Display/)
sdk_add_include_directories(./fezui/)
sdk_add_include_directories(./lefl/)

sdk_set_main_file(main.c)

project(oled)
