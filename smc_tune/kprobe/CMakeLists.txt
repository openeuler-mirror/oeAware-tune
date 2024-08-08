cmake_minimum_required(VERSION 3.10)

# 设置模块名称
project(smc_acc)

execute_process(
    COMMAND sh -c "rpm -q kernel-devel | awk -F'kernel-devel-' '{print $2}' | sort -V | tail -n 1"
    OUTPUT_VARIABLE KERNEL_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
)


# 设置内核目录
set(KDIR /lib/modules/${KERNEL_VERSION}/build)


function(compile_module obj)
    set(TARGET_NAME "${obj}")
    add_custom_target(${TARGET_NAME} ALL cp -f ${CMAKE_CURRENT_SOURCE_DIR}/*.c ${CMAKE_CURRENT_BINARY_DIR}/
        COMMAND cp -f ${CMAKE_CURRENT_SOURCE_DIR}/*.c ${CMAKE_CURRENT_BINARY_DIR}/
        COMMAND echo "compiling module ${obj}.ko..."
        )
    add_custom_command(TARGET ${TARGET_NAME}
        POST_BUILD
        COMMAND echo "obj-m := ${obj}.o" > ${CMAKE_CURRENT_BINARY_DIR}/Makefile
        COMMAND make -C ${KDIR} M=${CMAKE_CURRENT_BINARY_DIR} modules
        )
endfunction()

compile_module(smc_acc)
