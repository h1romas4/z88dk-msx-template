cmake_minimum_required(VERSION 3.13)

# Require Z88DK_HOME for this CMakeLists.txt
if(NOT DEFINED ENV{Z88DK_HOME})
    message(FATAL_ERROR
        "Z88DK_HOME not set; z88dk is required for this project."
        " Set Z88DK_HOME in your environment."
    )
endif()
# Set z88dk toolchain
set(CMAKE_C_COMPILER $ENV{Z88DK_HOME}/bin/zcc)
set(CMAKE_ASM_COMPILER $ENV{Z88DK_HOME}/bin/zcc)
# Export compile commands for clangd and other tooling
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_language(ASM)
project(example.rom LANGUAGES C ASM)

# Create executable target
add_executable(${PROJECT_NAME})

# Output directory for built artifacts (target-local)
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/dist
)

# Project Source files
target_sources(${PROJECT_NAME} PRIVATE
    src/c/example.c
    src/asm/psgdriver.asm
    src/asm/define_music.asm
    src/asm/define_graphic.asm
)

# Project include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src/c
    ${CMAKE_SOURCE_DIR}/src/asm
    $ENV{Z88DK_HOME}/include
    $ENV{Z88DK_HOME}/include/_DEVELOPMENT/sccz80
)

# Basic compile options
target_compile_options(${PROJECT_NAME} PRIVATE
    +msx
    -O2
    -vn
    #$<$<COMPILE_LANGUAGE:C>:-debug>
    $<$<COMPILE_LANGUAGE:ASM>:--list>
)

# Defines
target_compile_definitions(${PROJECT_NAME} PRIVATE
    # https://github.com/z88dk/z88dk/wiki/Classic-allocation#automatic-heap-configuration
    -DAMALLOC
)

# Linker options
target_link_options(${PROJECT_NAME} PRIVATE
    +msx
    -create-app
    -subtype=rom
    -m
    -lm
    -lmsxbios
)
