cmake_minimum_required(VERSION 3.15)

include(proj.conf)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

target_sources(app PRIVATE 
./temt6000/temt6000.c)
target_sources(app PRIVATE
./sgp30/driver_sgp30.c 
./sgp30/driver_sgp30_advance.c 
./sgp30/driver_sgp30_basic.c 
./sgp30/driver_sgp30_interface.c) 
target_sources(app PRIVATE ./wifi_tcp_server.c)
target_sources(app PRIVATE ./onenet_mqtt_sub.c)
target_sources(app PRIVATE
./ui/ui_Screen1.c
./ui/ui_Screen2.c
./ui/ui_Screen3.c
./ui/ui.c
./ui/ui_helpers.c)
target_sources(app PRIVATE
./cJSON/cJSON.c
./cJSON/cJSON_Utils.c)
target_sources(app PRIVATE
./dht22/dht22.c)
target_sources(app PRIVATE
./lefl/lefl_datastruct.c)
#sdk_library_add_sources(app PRIVATE)

sdk_add_include_directories(./temt6000/)
sdk_add_include_directories(./sgp30/)
sdk_add_include_directories(./)
sdk_add_include_directories(./ui/)
sdk_add_include_directories(./cJSON/)
sdk_add_include_directories(./dht22/)
sdk_add_include_directories(./lefl/)

sdk_set_main_file(main.c)

project(terminal)
