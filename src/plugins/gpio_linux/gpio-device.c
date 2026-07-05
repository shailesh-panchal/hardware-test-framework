#include "gpio-device.h"

#include <stdlib.h>
#include <string.h>

/*----------------------------------------------------------------------------
 * Public Functions
 *---------------------------------------------------------------------------*/

int32_t gpio_device_open(void *plugin_private_data, void **device_private_data) {
    gpio_device_context_t *context;

    (void)plugin_private_data;

    if (device_private_data == NULL) {
        return -1;
    }

    context = (gpio_device_context_t *)malloc(sizeof(gpio_device_context_t));

    if (context == NULL) {
        return -1;
    }

    /*
     * TODO:
     *
     * Create platform specific GPIO object.
     */

    *device_private_data = context;

    return 0;
}

int32_t gpio_device_close(void *device_private_data)
{
    gpio_device_context_t *context;

    context = (gpio_device_context_t *)device_private_data;

    if (context == NULL) {
        return -1;
    }

    /*
     * TODO:
     *
     * Release platform specific GPIO object.
     */

    free(context);

    return 0;
}

int32_t gpio_device_read(void *device_private_data, void *buffer, uint32_t size) {
    gpio_device_context_t *context;

    context = (gpio_device_context_t *)device_private_data;

    if ((context == NULL) || (buffer == NULL) ||(size != sizeof(gpio_level_t))) {
        return -1;
    }

    /*
     * TODO:
     *
     * Read GPIO level.
     */

    return 0;
}

int32_t gpio_device_write(void *device_private_data, const void *buffer, uint32_t size) {
    gpio_device_context_t *context;

    context = (gpio_device_context_t *)device_private_data;

    if ((context == NULL) || (buffer == NULL) || (size != sizeof(gpio_level_t))) {
        return -1;
    }

    /*
     * TODO:
     *
     * Write GPIO level.
     */

    return 0;
}

int32_t gpio_device_control(void *device_private_data, uint32_t command, void *argument) {
    gpio_device_context_t *context;

    context = (gpio_device_context_t *)device_private_data;

    if (context == NULL) {
        return -1;
    }

    switch (command)
    {
        case GPIO_COMMAND_SET_CONFIGURATION:

            if (argument == NULL)
            {
                return -1;
            }

            memcpy(&context->configuration, argument,sizeof(gpio_configuration_t));

            /*
             * TODO:
             *
             * Configure platform GPIO.
             */

            break;

        default:
            return -1;
    }

    return 0;
}