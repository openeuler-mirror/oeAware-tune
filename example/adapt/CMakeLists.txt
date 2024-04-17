set(tune_adapt_srcs
    adapt.cpp
)

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/oeAwarelib)

add_library(tune_example SHARED ${tune_adapt_srcs})

message(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(tune_example PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/dep_plugin/
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/frame/
    ${CMAKE_CURRENT_SOURCE_DIR}/../tune/
)

target_link_libraries(tune_example tune_base)
