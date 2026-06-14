#ifndef FUNCTION_PARSER_H
#define FUNCTION_PARSER_H

#include <stdint.h>
#include "device.h"

#define MAX_FUNCTIONS 128
#define FUNCTION_NAME_LENGTH 64
#define FUNCTION_DESC_LENGTH 128

typedef struct
{
    char name[FUNCTION_NAME_LENGTH];
} device_name_t;

typedef struct 
{
    char name[FUNCTION_NAME_LENGTH];
    char description[FUNCTION_DESC_LENGTH];
    device_name_t device_name[MAX_DEVICES];
    uint32_t count;

} function_def_t;


typedef struct
{
    uint32_t schema_version;

    function_def_t functions[MAX_FUNCTIONS];

    uint32_t count;

} fuction_config_t;

int32_t function_parser_load(const char* filename,fuction_config_t* cfg);
void function_print(fuction_config_t* cfg);

#endif // FUNCTION_PARSER_H