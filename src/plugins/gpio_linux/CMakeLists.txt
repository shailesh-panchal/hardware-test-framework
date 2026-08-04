cmake_minimum_required(VERSION 3.16)

#----------------------------------------------------------------------------
# GPIO Plugin
#----------------------------------------------------------------------------

add_library(gpio_linux SHARED
    gpio-plugin.c
    gpio-device.c
)

#----------------------------------------------------------------------------
# Include Directories
#----------------------------------------------------------------------------

target_include_directories(gpio_linux
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/src/hal
)

#----------------------------------------------------------------------------
# Output Name
#----------------------------------------------------------------------------

set_target_properties(gpio_linux PROPERTIES

    OUTPUT_NAME "gpio_linux"

    PREFIX ""

    SUFFIX ".so"
)

#----------------------------------------------------------------------------
# Compiler Options
#----------------------------------------------------------------------------

if(MSVC)

    target_compile_options(gpio_linux PRIVATE /W4)

else()

    target_compile_options(gpio_linux PRIVATE

        -Wall
        -Wextra
        -Wpedantic
        -fPIC
    )

endif()

# Link libgpiod on UNIX-like platforms only
if(UNIX)
    find_package(PkgConfig QUIET)
    if(PKG_CONFIG_FOUND)
        pkg_check_modules(LIBGPIOD QUIET gpiod)
        if(LIBGPIOD_FOUND)
            target_include_directories(gpio_linux PRIVATE ${LIBGPIOD_INCLUDE_DIRS})
            target_link_libraries(gpio_linux PRIVATE ${LIBGPIOD_LIBRARIES})
        else()
            # fall back to link name if pkg-config not available
            target_link_libraries(gpio_linux PRIVATE gpiod)
        endif()
    else()
        target_link_libraries(gpio_linux PRIVATE gpiod)
    endif()
endif()