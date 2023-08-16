cmake_minimum_required(VERSION 3.15)

include(proj.conf)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

aux_source_directory(./u8g2/ u8g2_LIST)
aux_source_directory(./fezui/ fezui_LIST)
aux_source_directory(./lefl/ lefl_LIST)

target_sources(app PRIVATE ${u8g2_LIST})
target_sources(app PRIVATE ${fezui_LIST})
target_sources(app PRIVATE ${lefl_LIST})

target_sources(app PRIVATE 
./temt6000/temt6000.c)
target_sources(app PRIVATE
./sgp30/driver_sgp30.c 
./sgp30/driver_sgp30_advance.c 
./sgp30/driver_sgp30_basic.c 
./sgp30/driver_sgp30_interface.c) 
target_sources(app PRIVATE ./Display/display.c)
target_sources(app PRIVATE ./wifi_tcp_server.c)
target_sources(app PRIVATE ./dht22/dht22.c)
target_sources(app PRIVATE ./onenet_mqtt_pub.c)
target_sources(app PRIVATE ./RGB/rgb.c)
target_sources(app PRIVATE 
./cjson/cJSON.c
./cjson/cJSON_Utils.c)
#sdk_library_add_sources(app PRIVATE)

sdk_add_include_directories(./temt6000/)
sdk_add_include_directories(./sgp30/)
sdk_add_include_directories(./dht22/)
sdk_add_include_directories(./cjson/)
sdk_add_include_directories(./u8g2/)
sdk_add_include_directories(./RGB/)
sdk_add_include_directories(./Display/)
sdk_add_include_directories(./fezui/)
sdk_add_include_directories(./lefl/)
sdk_add_include_directories(./)

sdk_set_main_file(main.c)

project(sensor)
