cmake_minimum_required(VERSION 3.15)

include(proj.conf)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

target_sources(app PRIVATE ./jpeg_head.c)
target_sources(app PRIVATE ./onenet_mqtt_sub.c)
target_sources(app PRIVATE ./car/car.c)
target_sources(app PRIVATE
./cJSON/cJSON.c
./cJSON/cJSON_Utils.c)
sdk_set_main_file(main.c)

sdk_add_include_directories(./)
sdk_add_include_directories(./cjson/)
sdk_add_include_directories(./car/)

project(mjpeg_cam_normal)
