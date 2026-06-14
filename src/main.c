#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "dm.h"
#include "config-manager.h"


int32_t main(int argc, char *argv[]) {

    if(argc != 2) {
        printf("Usage: %s <config_path>\n",argv[0]);
        return EXIT_FAILURE;
    }

    config_manager_t *config_manager = NULL;
    device_manager_t *device_manager = NULL;

    config_manager = config_manager_init(argv[1]);

    if(config_manager == NULL)
        return EXIT_FAILURE;

    if(0 != config_manager_load(config_manager)) {
        printf("failed to load configuration\n");
        config_manager_deinit(config_manager);
        return EXIT_FAILURE;
    }

    printf("Config Manager initialized with path: %s\n", argv[1]);

    device_manager = device_manager_init(config_manager);
    if(device_manager == NULL) {
        printf("failed to load configuration\n");
        config_manager_deinit(config_manager);
        return EXIT_FAILURE;
    }

    uint32_t platform_device_count = device_manager_get_count(device_manager);

    printf("Totatl Register platform device is %d\n",platform_device_count);

    return EXIT_SUCCESS;
}
