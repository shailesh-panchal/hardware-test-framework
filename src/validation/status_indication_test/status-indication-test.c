#include "status-indication-test.h"

#include "logger.h"
#include "test-operation.h"
#include "validation-export.h"


typedef struct {
    void *gpio_hal;
} status_indication_context_t;

static status_indication_context_t status_indication_context = {
    .gpio_hal = NULL
};


static int32_t status_indication_setup(void *context) {
    status_indication_context_t *ctx = (status_indication_context_t *)context;

    if (ctx == NULL) {
        return -1;
    }

    if (ctx->gpio_hal == NULL) {
        LOG_ERROR("Status LED handle is NULL.");
        return -1;
    }

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
    (void)context;

    return 0;
}

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

int32_t status_indication_register(validation_system_t *system) {
    if (system == NULL){
        return -1;
    }

    return validation_system_register(system,"led_indication");
}

VALIDATION_EXPORT(status_indication)
