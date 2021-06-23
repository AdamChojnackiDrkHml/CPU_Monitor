cmake_minimum_required(VERSION 3.12)


project(CPU_Monitor LANGUAGES C) 


file(GLOB PROJECT_SOURCE_FILES CONFIGURE_DEPENDS src/*.c)
include_directories(headers/)

string( TOLOWER "${CMAKE_C_COMPILER_ID}" COMPILER_ID )
if (COMPILER_ID STREQUAL "clang")
    add_compile_options(-Weverything -Wno-vla -Wno-unused-parameter -Watomic-implicit-seq-cst)
elseif(COMPILER_ID STREQUAL "gcc")
    add_compile_options(-Wall -Wextra -Weverything -Wno-vla -Wno-unused-parameter -Watomic-implicit-seq-cst)
endif() 

add_executable(CPU_Monitor ${PROJECT_SOURCE_FILES})

target_link_libraries(CPU_Monitor pthread)