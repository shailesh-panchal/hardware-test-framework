
#include "plugin-manager.h"
#include "dm.h"
#include "validation-runtime.h"

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

gpio_handle_t *validation_runtime_open_gpio(const char *device_name) {

    Device_t device = {0};
    plugin_t *plugin = NULL;
    gpio_handle_t *gpio = NULL;
    device_manager_t *dm = NULL;

    if ((validation_runtime_context.runtime == NULL) || (device_name == NULL)) {
    
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