#ifndef VALIDATION_REGISTRY_H
#define VALIDATION_REGISTRY_H

#include "validation-system.h"

typedef const validation_descriptor_t *
(*validation_get_descriptor_fn)(void);

extern const validation_get_descriptor_fn validation_registry[];

#endif