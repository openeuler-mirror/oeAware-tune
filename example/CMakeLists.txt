cmake_minimum_required(VERSION 3.15)

project(tune-example)
add_compile_options(-g -fPIC -Wall -Wextra)
add_subdirectory(tune)
add_subdirectory(adapt)
