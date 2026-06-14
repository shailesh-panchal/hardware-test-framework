#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <stdint.h>

#define CONFIG_MANAGER_FILE_PATH 255

typedef struct config_manager_t config_manager_t;

config_manager_t* config_manager_init(const char *config_path);
int32_t config_manager_deinit(config_manager_t *cfg_man);
int32_t config_manager_print(config_manager_t *cfg_man);
int32_t config_manager_load(config_manager_t *cfg_man);

#endif //CONFIG_MANAGER_H