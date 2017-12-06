cmake_minimum_required(VERSION 3.8)
project(reviewJamesOS C ASM)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -pedantic -Wall -m32 -ffreestanding")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T${PROJECT_SOURCE_DIR}/link.ld -ffreestanding -nostdlib -m32 -static -lgcc -Wl,--build-id=none")
set(CMAKE_ASM_FLAGS  "${CMAKE_ASM_FLAGS} -m32")

include_directories(src)
set(sources
        src/boot.S
        src/kernel.c)
add_executable(myos.bin ${sources})
