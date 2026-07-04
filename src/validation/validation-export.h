#ifndef VALIDATION_EXPORT_H
#define VALIDATION_EXPORT_H

#include "validation-system.h"

/*
 * Export a validation descriptor.
 *
 * Each validation module shall define a static
 * validation_descriptor_t and export it using this macro.
 *
 * Example:
 *
 * static validation_descriptor_t descriptor = { ... };
 *
 * VALIDATION_EXPORT(status_indication, descriptor)
 */

#define VALIDATION_EXPORT(name)             \
const validation_descriptor_t *                               \
name##_get_descriptor(void){                            \
    return &name##_descriptor;                            \
}

#endif