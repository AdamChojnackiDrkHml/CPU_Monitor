cmake_minimum_required(VERSION 3.12)

# Change CXX to C, if you're making a C program
project(CPU_Monitor LANGUAGES C) 

# Change *.cpp to *.c, if you're making a C program
file(GLOB PROJECT_SOURCE_FILES CONFIGURE_DEPENDS src/*.c)
include_directories(headers/)



# Change main.cpp to main.c, if you're making a C program
add_executable(CPU_Monitor ${PROJECT_SOURCE_FILES})

target_link_libraries(CPU_Monitor pthread)