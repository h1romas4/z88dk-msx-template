cmake_minimum_required(VERSION 3.0)

# target name "example.rom"
project(example.rom C ASM)

enable_language(ASM)

# for debug
#set(CMAKE_VERBOSE_MAKEFILE 1)

include(./cmake/macros.cmake)

add_source_files(
    ./src/c/example.c
    ./src/asm/psgdriver.asm
    ./src/asm/define_graphic.asm
    ./src/asm/define_music.asm
)

add_compile_flags(C
    +msx
    -I$ENV{Z88DK_HOME}/include
    -Isrc/c
    -vn
    -llib3d
    -lm
    -lndos
    -lmsxbios
    -m
    # -debug
    # https://github.com/z88dk/z88dk/wiki/Classic-allocation#automatic-heap-configuration
    -DAMALLOC
)

add_compile_flags(ASM
    +msx
    --list
)

add_compile_flags(LD
    -create-app
    -subtype=rom
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../dist)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
