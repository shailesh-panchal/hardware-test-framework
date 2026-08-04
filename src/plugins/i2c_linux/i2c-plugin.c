#include "i2c-plugin.h"

#include "i2c-device.h"

static plugin_ops_t i2c_plugin_ops = {
    .name = "i2c_linux",
    .version = "1.0.0",
    .open = i2c_device_open,
    .close = i2c_device_close,
    .read = i2c_device_read,
    .write = i2c_device_write,
    .control = i2c_device_control,
};

plugin_ops_t *i2c_plugin_init(void) {
    return &i2c_plugin_ops;
}

void i2c_plugin_deinit(void) {
    /* Nothing to release. */
}
