cmake_minimum_required(VERSION 3.12)


project(CPU_Monitor LANGUAGES C) 


file(GLOB PROJECT_SOURCE_FILES CONFIGURE_DEPENDS src/*.c)
include_directories(headers/)


add_compile_options(-Wall -Wextra -Weverything)

add_executable(CPU_Monitor ${PROJECT_SOURCE_FILES})

target_link_libraries(CPU_Monitor pthread)