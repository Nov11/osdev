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
        src/common.c
        src/gdt.c
        src/idt.c
        src/isr_handler.c
        src/gdt_flush.s
        src/idt_flush.s
        src/isr.s
        src/timer.c
        src/kheap.c
        src/paging.c
        src/ordered_array.c
        src/fs.c
        src/initrd.c)

add_executable(myos.bin ${sources})
add_custom_command(OUTPUT init.o
        COMMAND ${PROJECT_SOURCE_DIR}/mkvfs.sh
        depend init.o)
if(EXISTS "${PROJECT_SOURCE_DIR}/init.o")
    target_link_libraries(myos.bin )
endif()
#add_executable(generator src/tools/ramGenerator.c)
#target_link_libraries(generator c)

add_custom_command(TARGET myos.bin
        POST_BUILD
        COMMAND ${PROJECT_SOURCE_DIR}/conf/mkiso.sh ${PROJECT_SOURCE_DIR} ${PROJECT_BINARY_DIR})