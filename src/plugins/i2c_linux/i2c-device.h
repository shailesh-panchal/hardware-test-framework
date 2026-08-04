#ifndef I2C_DEVICE_H
#define I2C_DEVICE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "plugin.h"

#define I2C_COMMAND_SET_CONFIGURATION 0U

typedef struct {
    char bus[64];
    uint16_t address;
    uint32_t frequency_hz;
} i2c_configuration_t;

typedef struct {
    i2c_configuration_t configuration;
    int fd;
} i2c_device_context_t;

int32_t i2c_device_open(void *plugin_private_data, void **device_private_data);
int32_t i2c_device_close(void *device_private_data);
int32_t i2c_device_read(void *device_private_data, void *buffer, uint32_t size);
int32_t i2c_device_write(void *device_private_data, const void *buffer, uint32_t size);
int32_t i2c_device_control(void *device_private_data, uint32_t command, void *argument);

#ifdef __cplusplus
}
#endif

#endif /* I2C_DEVICE_H */
