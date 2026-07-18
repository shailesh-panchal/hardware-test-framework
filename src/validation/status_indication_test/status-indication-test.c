#include "status-indication-test.h"

#include "logger.h"
#include "test-operation.h"
#include "validation-export.h"
#include "validation-runtime.h"

static int32_t status_indication_setup(void *context);
static int32_t status_indication_execute(void *context);
static int32_t status_indication_cleanup(void *context);

typedef struct {
    void *gpio_hal;
} status_indication_context_t;

static status_indication_context_t status_indication_context = {
    .gpio_hal = NULL
};

static test_operations_t status_indication_ops = {
    .setup     = status_indication_setup,
    .execute  = status_indication_execute,
    .cleanup  = status_indication_cleanup
};


static const validation_descriptor_t status_indication_descriptor = {
    .name = "status_indication_test",
    .context = &status_indication_context,
    .ops = &status_indication_ops
};

static int32_t status_indication_setup(void *context) {
    status_indication_context_t *ctx = (status_indication_context_t *)context;
    gpio_configuration_t gpio_config = {0};

    if (ctx == NULL) {
        return -1;
    }

    /*
     * Open GPIO HAL for the status_led device.
     */
    ctx->gpio_hal = validation_runtime_open_gpio(status_indication_descriptor.name,&gpio_config);

    if (ctx->gpio_hal == NULL) {
        LOG_ERROR("Status LED handle is NULL.");
        return -1;
    }


    //configure the GPIO plugin as per the requirement of the test case
    //TODO shailesh how to get the GPIO configuration for the status LED from the config file or test descriptor
    gpio_hal_configure(ctx->gpio_hal,&gpio_config);

    return 0;
}

static int32_t status_indication_execute(void *context) {
    status_indication_context_t *ctx = (status_indication_context_t *)context;

    if (ctx == NULL) {
        return -1;
    }

    LOG_INFO("Status indication validation started.");
#if 0
    gpio_hal_write(ctx->status_led,GPIO_LEVEL_HIGH);

    osal_sleep_ms(500);

    gpio_hal_write(ctx->status_led,GPIO_LEVEL_LOW);

    osal_sleep_ms(500);
#endif
    LOG_INFO("Status indication validation completed.");

    return 0;
}

static int32_t status_indication_cleanup(void *context){
    status_indication_context_t *ctx = (status_indication_context_t *)context;

    if (ctx == NULL) {
        return -1;
    }

    gpio_hal_close(ctx->gpio_hal);
    return 0;
}

int32_t status_indication_register(validation_system_t *system) {
    if (system == NULL){
        return -1;
    }

    return validation_system_register(system,status_indication_descriptor.name);
}

VALIDATION_EXPORT(status_indication)
