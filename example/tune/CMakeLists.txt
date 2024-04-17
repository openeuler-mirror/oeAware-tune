set(tune_srcs
    tune.cpp
)

include_directories(tune_base PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/dep_plugin/
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/frame/
)
add_compile_options(-Wall -fPIC)
add_library(tune_base OBJECT ${tune_srcs})