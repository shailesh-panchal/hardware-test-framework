#include "i2c-device.h"

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

static int32_t i2c_open_bus(const char *bus, uint16_t address, int *fd_out) {
    int fd = open(bus, O_RDWR);
    if (fd < 0) {
        return -1;
    }

    if (ioctl(fd, I2C_SLAVE, address) < 0) {
        close(fd);
        return -1;
    }

    *fd_out = fd;
    return 0;
}

int32_t i2c_device_open(void *plugin_private_data, void **device_private_data) {
    i2c_device_context_t *context;

    (void)plugin_private_data;

    if (device_private_data == NULL) {
        return -1;
    }

    context = (i2c_device_context_t *)calloc(1, sizeof(i2c_device_context_t));
    if (context == NULL) {
        return -1;
    }

    context->fd = -1;
    *device_private_data = context;
    return 0;
}

int32_t i2c_device_close(void *device_private_data) {
    i2c_device_context_t *context = (i2c_device_context_t *)device_private_data;

    if (context == NULL) {
        return -1;
    }

    if (context->fd >= 0) {
        close(context->fd);
        context->fd = -1;
    }

    free(context);
    return 0;
}

int32_t i2c_device_read(void *device_private_data, void *buffer, uint32_t size) {
    i2c_device_context_t *context = (i2c_device_context_t *)device_private_data;

    if ((context == NULL) || (buffer == NULL) || (size == 0)) {
        return -1;
    }

    if (context->fd < 0) {
        return -1;
    }

    ssize_t bytes_read = read(context->fd, buffer, size);
    if (bytes_read < 0) {
        return -1;
    }

    return (int32_t)bytes_read;
}

int32_t i2c_device_write(void *device_private_data, const void *buffer, uint32_t size) {
    i2c_device_context_t *context = (i2c_device_context_t *)device_private_data;

    if ((context == NULL) || (buffer == NULL) || (size == 0)) {
        return -1;
    }

    if (context->fd < 0) {
        return -1;
    }

    ssize_t bytes_written = write(context->fd, buffer, size);
    if (bytes_written < 0) {
        return -1;
    }

    return (int32_t)bytes_written;
}

int32_t i2c_device_control(void *device_private_data, uint32_t command, void *argument) {
    i2c_device_context_t *context = (i2c_device_context_t *)device_private_data;
    i2c_configuration_t *configuration;

    if ((context == NULL) || (argument == NULL)) {
        return -1;
    }

    switch (command) {
    case I2C_COMMAND_SET_CONFIGURATION:
        configuration = (i2c_configuration_t *)argument;
        if (configuration->bus[0] == '\0') {
            return -1;
        }

        if (context->fd >= 0) {
            close(context->fd);
            context->fd = -1;
        }

        if (i2c_open_bus(configuration->bus, configuration->address, &context->fd) != 0) {
            return -1;
        }

        memcpy(&context->configuration, configuration, sizeof(i2c_configuration_t));
        return 0;

    default:
        return -1;
    }
}
