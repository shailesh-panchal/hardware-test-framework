#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "config-manager.h"

int32_t main(int argc, char *argv[]) {

    if(argc != 2) {
        printf("Usage: %s <config_path>\n",argv[0]);
        return EXIT_FAILURE;
    }

    config_manager_t *config_manager = NULL;

    config_manager = config_manager_init(argv[1]);

    if(config_manager == NULL)
        return EXIT_FAILURE;

    if(0 != config_manager_load(config_manager)) {
        printf("failed to load configuration\n");
        return EXIT_FAILURE;
    }

    printf("Config Manager initialized with path: %s\n", argv[1]);

    config_manager_print(config_manager);

    return EXIT_SUCCESS;
}
