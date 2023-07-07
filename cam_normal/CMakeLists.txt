cmake_minimum_required(VERSION 3.15)

include(proj.conf)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})
#add directories 一定要加
sdk_add_include_directories(.)
sdk_set_main_file(main.c)

project(cam_normal)
