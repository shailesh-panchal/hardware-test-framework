
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

    /* Get the GPIO configuration from the device binding.
     * If caller pre-populated configuration->lines_count (>0), preserve existing lines.
     */
    uint32_t line_index = configuration->lines_count;
    uint32_t chip_index = 0;
    uint32_t direction_index = 0;
    uint32_t active_low_index = 0;

    for(uint32_t index = 0; index < device.binding.configuration.parameter_count; index++) {
        if(strcmp(device.binding.configuration.parameters[index].key,"chip") == 0) {
            if (configuration->chip[0] == '\0') {
                safe_string_copy(configuration->chip, device.binding.configuration.parameters[index].value.string_value, CONFIG_KEY_LENGTH - 1);
            }
        }
        else if(strcmp(device.binding.configuration.parameters[index].key,"line") == 0) {
            if (line_index < GPIO_MAX_LINES) {
                configuration->lines[line_index] = device.binding.configuration.parameters[index].value.int_value;
                if (configuration->lines_chip[line_index][0] == '\0' && configuration->chip[0] != '\0') {
                    safe_string_copy(configuration->lines_chip[line_index], configuration->chip, CONFIG_KEY_LENGTH - 1);
                }
                line_index++;
            }
        }
        else if(strcmp(device.binding.configuration.parameters[index].key,"line_chip") == 0) {
            if (chip_index < GPIO_MAX_LINES) {
                safe_string_copy(configuration->lines_chip[chip_index], device.binding.configuration.parameters[index].value.string_value,
                    CONFIG_KEY_LENGTH - 1);
                chip_index++;
            }
        }
        else if(strcmp(device.binding.configuration.parameters[index].key,"direction") == 0) {
            if (direction_index < GPIO_MAX_LINES) {
                configuration->directions[direction_index] = (gpio_direction_t)device.binding.configuration.parameters[index].value.int_value;
                if (direction_index == 0) {
                    configuration->direction = configuration->directions[0];
                }
                direction_index++;
            }
        }
        else if(strcmp(device.binding.configuration.parameters[index].key,"active_low") == 0) {
            if (active_low_index < GPIO_MAX_LINES) {
                configuration->active_low_lines[active_low_index] = device.binding.configuration.parameters[index].value.bool_value;
                if (active_low_index == 0) {
                    configuration->active_low = configuration->active_low_lines[0];
                }
                active_low_index++;
            }
        }
    }

    if (line_index > configuration->lines_count) {
        configuration->lines_count = line_index;
    }
    configuration->directions_count = direction_index;
    configuration->active_low_count = active_low_index;

    if (configuration->lines_count == 0 && configuration->chip[0] != '\0') {
        configuration->lines[0] = configuration->line;
        configuration->lines_count = 1;
    }

    if (configuration->lines_count > 0) {
        configuration->line = configuration->lines[0];
    }

    if (configuration->lines_count > 0 && configuration->lines_chip[0][0] == '\0') {
        for (uint32_t i = 0; i < configuration->lines_count; ++i) {
            safe_string_copy(configuration->lines_chip[i], configuration->chip, CONFIG_KEY_LENGTH - 1);
        }
    }

    if (configuration->directions_count == 0 && configuration->lines_count > 0) {
        configuration->directions[0] = configuration->direction;
        configuration->directions_count = 1;
    }

    if (configuration->active_low_count == 0 && configuration->lines_count > 0) {
        configuration->active_low_lines[0] = configuration->active_low;
        configuration->active_low_count = 1;
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