#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "runtime.h"
#include "safe_string.h"
#include "logger.h"
#include "validation-system.h"


int32_t main(int argc, char *argv[]) {

    runtime_manager_t *runtime = NULL;
    validation_system_t *validation_system = NULL;
    char config_path[255] = {0};

    logger_init();

    if(argc != 2) {
        LOG_ERROR("Usage: %s <config_path>",argv[0]);
        return EXIT_FAILURE;
    }
    safe_string_copy(config_path, argv[1], sizeof(config_path));
    runtime = runtime_manager_init(config_path);
    if(NULL == runtime) {
        return EXIT_FAILURE;
    }

    validation_system = validation_system_init(runtime);
    if(NULL == validation_system) {
        runtime_manager_deinit(runtime);
        return EXIT_FAILURE;
    }

    //free the validation system and runtime manager
    validation_system_deinit(validation_system);
    runtime_manager_deinit(runtime);
    return EXIT_SUCCESS;
}
