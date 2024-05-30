cmake_minimum_required(VERSION 3.11)
project(thread_tune)

if (WITH_DEBUG)
    add_compile_options(-g)
else()
    add_compile_options(-O2)
endif()
add_compile_options(-fPIC -Wall -Wextra)

include_directories(../include)

add_library(thread_tune SHARED
    thread_tune.cpp
)

target_link_libraries(thread_tune numa)
