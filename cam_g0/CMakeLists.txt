cmake_minimum_required(VERSION 3.15)

include(proj.conf)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

target_sources(app PRIVATE ${sources}  ./wifi/onenet_post.c)

# 
# target_sources(wifi PRIVATE 
#                     ${sources}work/DCMRS/cam_g0/wifi/wifi.c
#                     ${sources}work/DCMRS/cam_g0/wifi/conenet_post.c
#                     ${sources}work/DCMRS/cam_g0/wifi/wifi_http_client.c
#                    )
sdk_add_include_directories(./wifi)      
sdk_add_include_directories(.)             
sdk_set_main_file(main.c)

project(cam_g0)
