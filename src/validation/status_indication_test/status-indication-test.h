/**
 * @file status_indication_test.h
 * @brief Status Indication Validation.
 *
 * This module implements the Status Indication validation.
 *
 * The validation verifies that the platform status indication
 * device operates correctly. The actual hardware device is
 * abstracted through the GPIO HAL, making the validation
 * independent of the underlying platform implementation.
 *
 * Module Responsibilities:
 *  - Register the validation with the Validation System.
 *  - Initialize validation resources.
 *  - Execute the validation.
 *  - Release validation resources.
 *
 * Execution Flow:
 *
 * Validation System
 *         │
 *         ▼
 * status_indication_register()
 *         │
 *         ▼
 * Test Engine
 *         │
 *         ▼
 * init()
 *         │
 *         ▼
 * execute()
 *         │
 *         ▼
 * cleanup()
 */

#ifndef STATUS_INDICATION_TEST_H
#define STATUS_INDICATION_TEST_H

#include <stdint.h>

#include "validation-system.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Register Status Indication validation.
 *
 * Registers the Status Indication validation with the
 * Validation System. During registration the validation
 * exposes its initialization, execution and cleanup
 * operations to the Test Engine.
 *
 * This function shall be called once during framework
 * initialization.
 *
 * @param[in,out] system
 * Validation System instance.
 *
 * @return Status code.
 *
 * @retval 0
 * Validation registered successfully.
 *
 * @retval -1
 * Invalid parameter.
 *
 * @retval -2
 * Registration failed.
 */
int32_t status_indication_test_register(
        validation_system_t *system);


#ifdef __cplusplus
}
#endif

#endif /* STATUS_INDICATION_TEST_H */