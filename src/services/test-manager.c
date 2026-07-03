#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#include "test-manager.h"
#include "safe_string.h"

/**
 * @brief Test Manager object.
 *
 * Stores all registered tests and their runtime status.
 */
struct test_manager_t
{
    /**
     * Registered test database.
     */
    Test_t tests[MAX_TESTS];

    /**
     * Number of registered tests.
     */
    uint32_t count;

};

static int32_t prepare_test_manager_info(test_manager_t *tm, config_manager_t *cm, function_manager_t *fm) {
    if((cm == NULL) || (fm == NULL) || (tm == NULL))
        return -1;

    uint32_t function_count = 0, fun_index = 0;
    uint32_t test_count = 0, test_index = 0 , tmp_count = 0;

    function_def_t function_def = {0};
    test_def_t test_def = {0};

     if(0 != config_manager_get_test_count(cm, &test_count)) {
        printf("failed to get the test count \n");
        return -1;
    }

     if(0 != function_manager_get_count(fm, &function_count)) {
        printf("failed to get the function count\n");
        return -1;
    }

    for(fun_index =0; fun_index < function_count; fun_index++) {
        if(0 != function_manager_get_function_by_index(fm,fun_index,&function_def)) {
            printf("failed to get the function defination by index\n");
            continue;
        }
        for(test_index =0; test_index < test_count; test_index++) {
            if(0 != config_manager_get_test_by_index(cm,test_index,&test_def)) {
                printf("failed to get the function defination by index\n");
                continue;
            }
            if(safe_string_compare(test_def.function,function_def.name)) {
                safe_string_copy(tm->tests[tmp_count].name,function_def.name,sizeof(tm->tests[tmp_count].name));
                tm->tests[tmp_count].state = TEST_STATE_AVAILABLE;
                memcpy(&tm->tests[tmp_count].definition,&test_def,sizeof(test_def_t));
                tm->count = tmp_count + 1;
                tmp_count++;
            }
        }
    }
    return 0;
}
test_manager_t* test_manager_init(config_manager_t* cm, function_manager_t* fm) {
    test_manager_t *tm = NULL;

    if((cm == NULL) || (fm == NULL))
        return NULL;

    tm = (test_manager_t *)malloc(sizeof(test_manager_t));

    if(tm == NULL)
        return NULL;

    if(0 != prepare_test_manager_info(tm,cm,fm)) {
        free(tm);
        return NULL;
    }
    return tm;
}

int32_t test_manager_deinit(test_manager_t* tm) {
    if(tm == NULL)
        return -1;

    free(tm);

    return 0;
}

int32_t test_manager_get_count(test_manager_t* tm, uint32_t *count){
    if((tm == NULL) || (count == NULL))
        return -1;

    *count = tm->count;
    return 0;
}

int32_t test_manager_get_test(test_manager_t* tm,const char* name,test_def_t* test) {
    if((tm == NULL) || (name == NULL) || (test == NULL))
        return -1;

    if(tm->count == 0)
        return -1;

    for(uint32_t index =0; index < tm->count; index++) {
        if(safe_string_compare(name, tm->tests[index].name)) {
            memcpy(test,&tm->tests[index].definition,sizeof(test_def_t));
            return 0;
        }
     }
    return -1;
}

int32_t test_manager_get_test_by_index(test_manager_t* tm, uint32_t index, test_def_t* test) {
    if((tm == NULL) || (test == NULL))
        return -1;

    if(index < tm->count)
        return -1;

    memcpy(test,&tm->tests[index].definition,sizeof(test_def_t));
    return 0;
}

bool test_manager_test_is_available(test_manager_t* tm, const char* name) {
    if(tm == NULL)
        return 0;

    if(tm->count == 0)
        return 0;

    for(uint32_t index =0; index < tm->count; index++) {
        if(safe_string_compare(name, tm->tests[index].name)) {
            return 1;
        }
    }
    return 0;
}

int32_t test_manager_get_function_name(test_manager_t* tm,const char* name, char* function_name){
    if((tm == NULL) || (name == NULL) || (function_name == NULL))
        return -1;

    if(tm->count == 0)
        return -1;

    for(uint32_t index =0; index < tm->count; index++) {
        if(safe_string_compare(name, tm->tests[index].name)) {
            safe_string_copy(function_name,tm->tests[index].definition.function,sizeof(tm->tests[index].definition.function));
            return 0;
        }
    }
    return -1;
}

void test_manager_printf(test_manager_t *tm){

    if(tm == NULL)
        return;

    for(uint32_t index =0; index < tm->count; index++) {
        printf("name: %s\n",tm->tests[index].name);
        printf("description: %s\n",tm->tests[index].definition.description);
        printf("function: %s\n",tm->tests[index].definition.function);
        printf("test: %d\n",tm->tests[index].state);
    }
}