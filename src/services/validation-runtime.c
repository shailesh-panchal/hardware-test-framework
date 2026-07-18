
#include "plugin-manager.h"
#include "dm.h"
#include "validation-runtime.h"
#include "safe_string.h"

/*---------------------------------------------------------------------------
 * Public Functions
 *---------------------------------------------------------------------------*/
typedef struct {
    runtime_manager_t *runtime;
}validation_runtime_context_t;

static validation_runtime_context_t validation_runtime_context = {
    .runtime = NULL
};
int32_t validation_runtime_init(runtime_manager_t *runtime){
    if(runtime == NULL) {
        return -1;
    }
    validation_runtime_context.runtime = runtime;

    return 0;
}

int32_t validation_runtime_deinit(void)
{
    return 0;
}

gpio_handle_t *validation_runtime_open_gpio(const char *device_name,gpio_configuration_t *configuration) {

    Device_t device = {0};
    plugin_t *plugin = NULL;
    gpio_handle_t *gpio = NULL;
    device_manager_t *dm = NULL;

    if ((validation_runtime_context.runtime == NULL) || (device_name == NULL) || (configuration == NULL)) {
    
        return NULL;
    }

    dm = runtime_manager_get_device_manager(validation_runtime_context.runtime);
    if(dm == NULL) {
        return NULL;
    }
    /*
     * Resolve device information.
     */
    if(0 != device_manager_get_device_by_name(dm,device_name, &device)){
        return NULL;
    }

    plugin_manager_t* plugin_manager = runtime_manager_get_plugin_manager(validation_runtime_context.runtime);
    if(plugin_manager == NULL) {
        return NULL;
    }
    /*
     * Resolve plugin.
     */
    plugin = plugin_manager_get(plugin_manager,(const char *)device.binding.plugin);
    if (plugin == NULL) {
        return NULL;
    }

    //get the GPIO configuration from the device binding
    for(uint32_t index = 0; index < device.binding.configuration.parameter_count; index++) {
        if(strcmp(device.binding.configuration.parameters[index].key,"chip") == 0) {
            safe_string_copy(configuration->chip, device.binding.configuration.parameters[index].value.string_value, CONFIG_KEY_LENGTH - 1);
        }
        else if(strcmp(device.binding.configuration.parameters[index].key,"line") == 0) {
            configuration->line = device.binding.configuration.parameters[index].value.int_value;
        }
        else if(strcmp(device.binding.configuration.parameters[index].key,"direction") == 0) {
            configuration->direction = (gpio_direction_t)device.binding.configuration.parameters[index].value.int_value;
        }
        else if(strcmp(device.binding.configuration.parameters[index].key,"active_low") == 0) {
            configuration->active_low = device.binding.configuration.parameters[index].value.bool_value;
        }
    }

    /*
     * Create HAL.
     */
    gpio = gpio_hal_open(plugin);

    if (gpio == NULL) {
        return NULL;
    }
    return gpio;
}

int32_t validation_runtime_close_gpio(
        gpio_handle_t *handle)
{
    return gpio_hal_close(handle);
}