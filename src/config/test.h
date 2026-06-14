#ifndef TEST_PARSER_H
#define TEST_PARSER_H

#include <stdint.h>
#include "device.h"

#include "device.h"
#include "function.h"

#define MAX_TESTS 128
#define TEST_NAME_LENGTH 64
#define TEST_DESC_LENGTH 128

typedef struct 
{
    char name[TEST_NAME_LENGTH];
    char description[TEST_DESC_LENGTH];
    char function[FUNCTION_NAME_LENGTH]; // Name of the function to execute the test
} test_def_t;


typedef struct
{
    uint32_t schema_version;

    test_def_t tests[MAX_TESTS];

    uint32_t count;

} test_config_t;

int32_t test_parser_load(const char* filename,test_config_t* cfg);
void test_printf(test_config_t *cfg);
#endif // TEST_PARSER_H