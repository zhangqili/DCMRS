cmake_minimum_required(VERSION 3.15)

include(proj.conf)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

target_sources(app PRIVATE 
./dht22/dht22.c 
./temt6000/temt6000.c) 
 target_sources(app PRIVATE
 ./sgp30/driver_sgp30.c 
 ./sgp30/driver_sgp30_advance.c 
 ./sgp30/driver_sgp30_basic.c 
 ./sgp30/driver_sgp30_interface.c)
# target_sources(app PRIVATE
# ./sgp30/sgp30.c)


sdk_add_include_directories(./dht22/)
sdk_add_include_directories(./temt6000/)
sdk_add_include_directories(./sgp30/)
sdk_add_include_directories(./)

sdk_set_main_file(main.c)

project(driver_test)
