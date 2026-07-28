#include "gpio-device.h"

#include <stdlib.h>
#include <string.h>
#if defined(__linux__)
#include <gpiod.h>
#endif

/*----------------------------------------------------------------------------
 * Public Functions
 *---------------------------------------------------------------------------*/

int32_t gpio_device_open(void *plugin_private_data, void **device_private_data) {
    gpio_device_context_t *context;

    (void)plugin_private_data;

    if (device_private_data == NULL) {
        return -1;
    }

    context = (gpio_device_context_t *)malloc(sizeof(gpio_device_context_t));

    if (context == NULL) {
        return -1;
    }

    /* Initialize runtime context; defer opening libgpiod resources until configured */
    memset(context, 0, sizeof(gpio_device_context_t));
#if defined(__linux__)
    for (uint32_t i = 0; i < GPIO_MAX_LINES; ++i) {
        context->chips[i] = NULL;
        context->lines[i] = NULL;
    }
    context->lines_count = 0;
    context->active_low = 0;
#endif
    *device_private_data = context;

    return 0;
}

int32_t gpio_device_close(void *device_private_data)
{
    gpio_device_context_t *context;

    context = (gpio_device_context_t *)device_private_data;

    if (context == NULL) {
        return -1;
    }

    /* Release platform-specific GPIO object on supported platforms */
#if defined(__linux__)
    for (uint32_t i = 0; i < context->lines_count; ++i) {
        if (context->lines[i] != NULL) {
            gpiod_line_request_release(context->lines[i]);
            context->lines[i] = NULL;
        }
        if (context->chips[i] != NULL) {
            gpiod_chip_close(context->chips[i]);
            context->chips[i] = NULL;
        }
    }
    context->lines_count = 0;
#endif
    free(context);

    return 0;
}

int32_t gpio_device_read(void *device_private_data, void *buffer, uint32_t size) {
    gpio_device_context_t *context;

    context = (gpio_device_context_t *)device_private_data;

    if ((context == NULL) || (buffer == NULL)) {
        return -1;
    }

    /* Read GPIO level(s) (only supported on Linux with libgpiod) */
#if defined(__linux__)
    if (context->lines_count == 0) return -1;

    /* single-level read */
    if (size == sizeof(gpio_level_t)) {
        enum gpiod_line_value value = gpiod_line_request_get_value(context->lines[0], context->configuration.lines[0]);
        if (value == GPIOD_LINE_VALUE_ERROR) return -1;
        int val = (value == GPIOD_LINE_VALUE_ACTIVE) ? 1 : 0;
        if (context->configuration.active_low) val = !val;
        *((gpio_level_t *)buffer) = (val ? GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW);
        return 0;
    }

    /* multi-level read */
    if (size != (context->lines_count * sizeof(gpio_level_t))) return -1;
    gpio_level_t *out = (gpio_level_t *)buffer;
    for (uint32_t i = 0; i < context->lines_count; ++i) {
        enum gpiod_line_value value = gpiod_line_request_get_value(context->lines[i], context->configuration.lines[i]);
        if (value == GPIOD_LINE_VALUE_ERROR) return -1;
        int val = (value == GPIOD_LINE_VALUE_ACTIVE) ? 1 : 0;
        if (context->configuration.active_low) val = !val;
        out[i] = (val ? GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW);
    }
    return 0;
#else
    (void)context; (void)buffer; (void)size;
    return -1;
#endif
}

int32_t gpio_device_write(void *device_private_data, const void *buffer, uint32_t size) {
    gpio_device_context_t *context;

    context = (gpio_device_context_t *)device_private_data;

    if ((context == NULL) || (buffer == NULL)) {
        return -1;
    }

    /* Write GPIO level(s) (only supported on Linux with libgpiod) */
#if defined(__linux__)
    if (context->lines_count == 0) return -1;

    /* single-level write */
    if (size == sizeof(gpio_level_t)) {
        gpio_level_t level = *((gpio_level_t *)buffer);
        int val = (level == GPIO_LEVEL_HIGH) ? 1 : 0;
        if (context->configuration.active_low) val = !val;
        enum gpiod_line_value value = (val ? GPIOD_LINE_VALUE_ACTIVE : GPIOD_LINE_VALUE_INACTIVE);
        if (gpiod_line_request_set_value(context->lines[0], context->configuration.lines[0], value) < 0) return -1;
        return 0;
    }

    /* multi-level write */
    if (size != (context->lines_count * sizeof(gpio_level_t))) return -1;
    const gpio_level_t *in = (const gpio_level_t *)buffer;
    for (uint32_t i = 0; i < context->lines_count; ++i) {
        int val = (in[i] == GPIO_LEVEL_HIGH) ? 1 : 0;
        if (context->configuration.active_low) val = !val;
        enum gpiod_line_value value = (val ? GPIOD_LINE_VALUE_ACTIVE : GPIOD_LINE_VALUE_INACTIVE);
        if (gpiod_line_request_set_value(context->lines[i], context->configuration.lines[i], value) < 0) return -1;
    }
    return 0;
#else
    (void)context; (void)buffer; (void)size;
    return -1;
#endif
}

int32_t gpio_device_control(void *device_private_data, uint32_t command, void *argument) {
    gpio_device_context_t *context;

    context = (gpio_device_context_t *)device_private_data;

    if (context == NULL) {
        return -1;
    }

    switch (command)
    {
        case GPIO_COMMAND_SET_CONFIGURATION:
            if (argument == NULL) {
                return -1;
            }

            memcpy(&context->configuration, argument, sizeof(gpio_configuration_t));

#if defined(__linux__)
            if (context->configuration.lines_count == 0 && context->configuration.chip[0] != '\0') {
                context->configuration.lines[0] = context->configuration.line;
                context->configuration.lines_count = 1;
            }

            if (context->configuration.lines_count > GPIO_MAX_LINES) {
                return -1;
            }

            /* Release previous resources if any */
            for (uint32_t i = 0; i < context->lines_count; ++i) {
                if (context->lines[i] != NULL) {
                    gpiod_line_request_release(context->lines[i]);
                    context->lines[i] = NULL;
                }
                if (context->chips[i] != NULL) {
                    gpiod_chip_close(context->chips[i]);
                    context->chips[i] = NULL;
                }
            }
            context->lines_count = 0;

            for (uint32_t i = 0; i < context->configuration.lines_count; ++i) {
                const char *chip_name = NULL;
                if (context->configuration.lines_chip[i][0] != '\0') {
                    chip_name = context->configuration.lines_chip[i];
                } else if (context->configuration.chip[0] != '\0') {
                    chip_name = context->configuration.chip;
                } else {
                    return -1;
                }

                struct gpiod_chip *chip = NULL;
                if (chip_name[0] == '/') {
                    chip = gpiod_chip_open(chip_name);
                } else {
                    char chip_path[64];
                    int written = snprintf(chip_path, sizeof(chip_path), "/dev/%s", chip_name);
                    if (written < 0 || written >= (int)sizeof(chip_path)) {
                        chip = NULL;
                    } else {
                        chip = gpiod_chip_open(chip_path);
                    }
                }

                if (chip == NULL) {
                    for (uint32_t j = 0; j < context->lines_count; ++j) {
                        if (context->lines[j]) gpiod_line_request_release(context->lines[j]);
                        if (context->chips[j]) gpiod_chip_close(context->chips[j]);
                        context->lines[j] = NULL;
                        context->chips[j] = NULL;
                    }
                    context->lines_count = 0;
                    return -1;
                }

                gpio_direction_t direction = GPIO_DIRECTION_INPUT;
            if (i < context->configuration.directions_count) {
                direction = context->configuration.directions[i];
            } else {
                direction = context->configuration.direction;
            }

            struct gpiod_request_config *req_cfg = gpiod_request_config_new();
            struct gpiod_line_config *line_cfg = gpiod_line_config_new();
            struct gpiod_line_settings *line_settings = gpiod_line_settings_new();
            struct gpiod_line_request *request = NULL;
            unsigned int offset = context->configuration.lines[i];

            if (req_cfg == NULL || line_cfg == NULL || line_settings == NULL) {
                if (req_cfg != NULL) gpiod_request_config_free(req_cfg);
                if (line_cfg != NULL) gpiod_line_config_free(line_cfg);
                if (line_settings != NULL) gpiod_line_settings_free(line_settings);
                gpiod_chip_close(chip);
                for (uint32_t j = 0; j < context->lines_count; ++j) {
                    if (context->lines[j]) gpiod_line_request_release(context->lines[j]);
                    if (context->chips[j]) gpiod_chip_close(context->chips[j]);
                    context->lines[j] = NULL;
                    context->chips[j] = NULL;
                }
                context->lines_count = 0;
                return -1;
            }

            gpiod_request_config_set_consumer(req_cfg, "gpio_linux");
            if (gpiod_line_settings_set_direction(line_settings,
                    direction == GPIO_DIRECTION_INPUT ? GPIOD_LINE_DIRECTION_INPUT : GPIOD_LINE_DIRECTION_OUTPUT) < 0) {
                gpiod_request_config_free(req_cfg);
                gpiod_line_config_free(line_cfg);
                gpiod_line_settings_free(line_settings);
                gpiod_chip_close(chip);
                for (uint32_t j = 0; j < context->lines_count; ++j) {
                    if (context->lines[j]) gpiod_line_request_release(context->lines[j]);
                    if (context->chips[j]) gpiod_chip_close(context->chips[j]);
                    context->lines[j] = NULL;
                    context->chips[j] = NULL;
                }
                context->lines_count = 0;
                return -1;
            }

            if (direction == GPIO_DIRECTION_OUTPUT) {
                if (gpiod_line_settings_set_output_value(line_settings, GPIOD_LINE_VALUE_INACTIVE) < 0) {
                    gpiod_request_config_free(req_cfg);
                    gpiod_line_config_free(line_cfg);
                    gpiod_line_settings_free(line_settings);
                    gpiod_chip_close(chip);
                    for (uint32_t j = 0; j < context->lines_count; ++j) {
                        if (context->lines[j]) gpiod_line_request_release(context->lines[j]);
                        if (context->chips[j]) gpiod_chip_close(context->chips[j]);
                        context->lines[j] = NULL;
                        context->chips[j] = NULL;
                    }
                    context->lines_count = 0;
                    return -1;
                }
            }

            if (gpiod_line_config_add_line_settings(line_cfg, &offset, 1, line_settings) < 0) {
                gpiod_request_config_free(req_cfg);
                gpiod_line_config_free(line_cfg);
                gpiod_line_settings_free(line_settings);
                gpiod_chip_close(chip);
                for (uint32_t j = 0; j < context->lines_count; ++j) {
                    if (context->lines[j]) gpiod_line_request_release(context->lines[j]);
                    if (context->chips[j]) gpiod_chip_close(context->chips[j]);
                    context->lines[j] = NULL;
                    context->chips[j] = NULL;
                }
                context->lines_count = 0;
                return -1;
            }

            request = gpiod_chip_request_lines(chip, req_cfg, line_cfg);
            gpiod_request_config_free(req_cfg);
            gpiod_line_config_free(line_cfg);
            gpiod_line_settings_free(line_settings);

            if (request == NULL) {
                gpiod_chip_close(chip);
                for (uint32_t j = 0; j < context->lines_count; ++j) {
                    if (context->lines[j]) gpiod_line_request_release(context->lines[j]);
                    if (context->chips[j]) gpiod_chip_close(context->chips[j]);
                    context->lines[j] = NULL;
                    context->chips[j] = NULL;
                }
                context->lines_count = 0;
                return -1;
            }

            context->chips[context->lines_count] = chip;
            context->lines[context->lines_count] = request;
            context->lines_count++;
            }

            context->active_low = context->configuration.active_low;
#else
            (void)context; /* keep compiler quiet if fields are unused */
#endif
            break;

        default:
            return -1;
    }

    return 0;
}
