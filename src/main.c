#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "config-manager.h"

int32_t main() {

    printf("Welcome to Hardware Validator Framework!\n");

    config_manager_t *config_manager = NULL;

    config_manager = config_manager_init("C:/data_backup/workspace/hardware-test-framework/config/json");

    if(config_manager == NULL)
        return EXIT_FAILURE;

    if(0 != config_manager_load(config_manager)) {
        printf("failed to load configuration\n");
        return EXIT_FAILURE;
    }

    config_manager_print(config_manager);

    return 0;
}
