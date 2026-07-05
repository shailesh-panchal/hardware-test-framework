#include <stdlib.h>

#include "gpio-hal.h"

struct gpio_handle {
    plugin_t *plugin;

    void *device_private_data;
};


gpio_handle_t *gpio_hal_open(plugin_t *plugin) {
    gpio_handle_t *handle;

    if ((plugin == NULL) || (plugin->ops == NULL) || (plugin->ops->open == NULL)) {
        return NULL;
    }

    handle = (gpio_handle_t *)malloc(sizeof(gpio_handle_t));

    if(handle == NULL) {
        return NULL;
    }

    handle->plugin = plugin;

    if (plugin->ops->open(plugin->private_data, &handle->device_private_data) != 0) {
        free(handle);
        return NULL;
    }

    return handle;
}

int32_t gpio_hal_close(gpio_handle_t *handle) {
    if(handle == NULL){
        return -1;
    }

    if ((handle->plugin != NULL) && (handle->plugin->ops != NULL) && (handle->plugin->ops->close != NULL)) {
        handle->plugin->ops->close(handle->device_private_data);
    }

    free(handle);

    return 0;
}

int32_t gpio_hal_configure(gpio_handle_t *handle,const gpio_configuration_t *configuration) {
    if ((handle == NULL) || (configuration == NULL)) {
        return -1;
    }

    return handle->plugin->ops->control(handle->device_private_data,GPIO_COMMAND_SET_CONFIGURATION,(void *)configuration);
}

int32_t gpio_hal_read(gpio_handle_t *handle, gpio_level_t *level) {
    if ((handle == NULL) || (level == NULL)){
        return -1;
    }

    return handle->plugin->ops->read(handle->device_private_data,level,sizeof(gpio_level_t));
}

int32_t gpio_hal_write(gpio_handle_t *handle,gpio_level_t level) {
    if (handle == NULL) {
        return -1;
    }

    return handle->plugin->ops->write(handle->device_private_data,&level,sizeof(gpio_level_t));
}

int32_t gpio_hal_control(gpio_handle_t *handle,gpio_command_t command,void *argument) {
    if (handle == NULL){
        return -1;
    }

    return handle->plugin->ops->control(handle->device_private_data, command,argument);
}