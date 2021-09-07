cmake_minimum_required(VERSION 3.0)

# target name "example.rom"
project(example.rom C ASM)

enable_language(ASM)
set(CMAKE_C_COMPILER $ENV{Z88DK_HOME}/bin/zcc)
set(CMAKE_ASM_COMPILER $ENV{Z88DK_HOME}/bin/zcc)

# for debug
# set(CMAKE_VERBOSE_MAKEFILE 1)

include(./cmake/macros.cmake)

header_directories(
    ./src/msx
    $ENV{Z88DK_HOME}/include
)

add_source_files(
    ./src/msx/example.c
    ./src/msx/chars.asm
)

add_compile_flags(C
    +msx
    -vn
    -llib3d
    -lm
    -lndos
    -lmsxbios
    # -debug
    # https://github.com/z88dk/z88dk/wiki/Classic-allocation#automatic-heap-configuration
    -DAMALLOC
)

add_compile_flags(ASM
    +msx
    -c
)

add_compile_flags(LD
    -create-app
    -subtype=rom
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../dist)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
