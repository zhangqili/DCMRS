cmake_minimum_required(VERSION 3.15)

include(proj.conf)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

sdk_library_add_sources(app PRIVATE ./yyy/xxx.c ./xxx1.c);

sdk_add_include_directories(./);
sdk_add_include_directories(./yyy/);

sdk_set_main_file(main.c)

project(terminal)
