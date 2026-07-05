#include "gpio-plugin.h"

#include "gpio-device.h"

/*---------------------------------------------------------------------------
 * Plugin Operations
 *---------------------------------------------------------------------------*/

static plugin_ops_t gpio_plugin_ops =
{
    .name    = "gpio_linux",
    .version = "1.0.0",

    .open    = gpio_device_open,
    .close   = gpio_device_close,
    .read    = gpio_device_read,
    .write   = gpio_device_write,
    .control = gpio_device_control
};

/*---------------------------------------------------------------------------
 * Public Functions
 *---------------------------------------------------------------------------*/

plugin_ops_t *gpio_plugin_init(void)
{
    return &gpio_plugin_ops;
}

void gpio_plugin_deinit(void)
{
    /* Nothing to release. */
}