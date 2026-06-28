#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "runtime.h"
#include "safe_string.h"


int32_t main(int argc, char *argv[]) {

    if(argc != 2) {
        printf("Usage: %s <config_path>\n",argv[0]);
        return EXIT_FAILURE;
    }

    runtime_manager_t *runtime = NULL;
    char config_path[255] = {0};

    safe_string_copy(config_path, argv[1], sizeof(config_path));

    runtime = runtime_manager_init(config_path);
    if(NULL == runtime) {
        return EXIT_FAILURE;
    }

    runtime_manager_deinit(runtime);
    
    return EXIT_SUCCESS;
}
