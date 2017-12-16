cmake_minimum_required(VERSION 3.5)
project(OSdev C ASM)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pedantic -Wall -m32 -ffreestanding -g")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T${PROJECT_SOURCE_DIR}/conf/link.ld -nostdlib -static  -Wl,--build-id=none")
set(CMAKE_ASM_FLAGS  "${CMAKE_ASM_FLAGS} -m32 -g")

include_directories(src)
set(sources
        src/boot.S
        src/kernel.c
        src/terminal.c
        src/stringops.c
        src/common.c)
add_executable(myos.bin ${sources})
add_custom_command(TARGET myos.bin
        POST_BUILD
        COMMAND ${PROJECT_SOURCE_DIR}/conf/mkiso.sh ${PROJECT_SOURCE_DIR} ${PROJECT_BINARY_DIR})