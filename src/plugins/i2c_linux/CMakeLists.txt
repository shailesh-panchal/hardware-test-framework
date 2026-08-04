cmake_minimum_required(VERSION 3.16)

#----------------------------------------------------------------------------
# I2C Plugin
#----------------------------------------------------------------------------

add_library(i2c_linux SHARED
    i2c-plugin.c
    i2c-device.c
)

#----------------------------------------------------------------------------
# Include Directories
#----------------------------------------------------------------------------

target_include_directories(i2c_linux
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/src/hal
)

#----------------------------------------------------------------------------
# Output Name
#----------------------------------------------------------------------------

set_target_properties(i2c_linux PROPERTIES
    OUTPUT_NAME "i2c_linux"
    PREFIX ""
    SUFFIX ".so"
)

#----------------------------------------------------------------------------
# Compiler Options
#----------------------------------------------------------------------------

if(MSVC)
    target_compile_options(i2c_linux PRIVATE /W4)
else()
    target_compile_options(i2c_linux PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -fPIC
    )
endif()
