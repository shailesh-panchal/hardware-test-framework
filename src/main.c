#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include "cli-parser.h"
#include "config-manager.h"
#include "logger.h"
#include "runtime.h"
#include "test-engine.h"
#include "test-manager.h"
#include "validation-system.h"

#define ANSI_COLOR_GREEN  "\033[32m"
#define ANSI_COLOR_YELLOW "\033[33m"
#define ANSI_COLOR_RED    "\033[31m"
#define ANSI_COLOR_RESET  "\033[0m"

static const char *result_state_name(test_execution_state_e state) {
    switch(state) {
        case TEST_STATE_PASS: return "PASS";
        case TEST_STATE_FAIL: return "FAIL";
        case TEST_STATE_SKIP: return "SKIP";
        case TEST_STATE_TIMEOUT: return "TIMEOUT";
        default: return "UNKNOWN";
    }
}

static void print_test(const test_def_t *test) {
    printf("  %-32s %s\n", test->name, test->description);
    printf("    Function: %s\n", test->function);
}

static int wait_for_test_result(test_engine_t *test_engine, const char *name,
    test_result_t *result) {
    const struct timespec wait_interval = { .tv_sec = 0, .tv_nsec = 10000000 };

    for(uint32_t attempt = 0; attempt < 3000; attempt++) {
        if(test_engine_get_result(test_engine, name, result) == 0 &&
           result->state != TEST_STATE_QUEUED && result->state != TEST_STATE_RUNNING) {
            return 0;
        }
        nanosleep(&wait_interval, NULL);
    }
    return -1;
}

static int find_configured_test(config_manager_t *config_manager, const char *name,
    test_def_t *test) {
    uint32_t test_count = 0;

    if(config_manager == NULL || name == NULL || test == NULL ||
       config_manager_get_test_count(config_manager, &test_count) != 0) {
        return -1;
    }
    for(uint32_t index = 0; index < test_count; index++) {
        if(config_manager_get_test_by_index(config_manager, index, test) == 0 &&
           strcmp(test->name, name) == 0) {
            return 0;
        }
    }
    return -1;
}

static int list_tests(runtime_manager_t *runtime, validation_system_t *system,
    cli_list_filter_t filter) {
    config_manager_t *config_manager = runtime_manager_get_config_manager(runtime);
    test_manager_t *test_manager = runtime_manager_get_test_manager(runtime);
    uint32_t test_count = 0;
    test_def_t test = {0};

    if(config_manager_get_test_count(config_manager, &test_count) != 0) {
        return -1;
    }

    if(filter == CLI_LIST_ALL || filter == CLI_LIST_RUNNABLE) {
        printf(ANSI_COLOR_GREEN "Runnable tests:" ANSI_COLOR_RESET "\n");
        for(uint32_t index = 0; index < test_count; index++) {
            if(config_manager_get_test_by_index(config_manager, index, &test) == 0 &&
               test_manager_test_is_available(test_manager, test.name) &&
               validation_system_is_implemented(system, test.name)) {
                print_test(&test);
            }
        }
    }

    if(filter == CLI_LIST_ALL || filter == CLI_LIST_UNIMPLEMENTED) {
        printf(ANSI_COLOR_YELLOW "Supported, not implemented tests:" ANSI_COLOR_RESET "\n");
        for(uint32_t index = 0; index < test_count; index++) {
            if(config_manager_get_test_by_index(config_manager, index, &test) == 0 &&
               test_manager_test_is_available(test_manager, test.name) &&
               !validation_system_is_implemented(system, test.name)) {
                print_test(&test);
            }
        }
    }

    if(filter == CLI_LIST_ALL || filter == CLI_LIST_UNAVAILABLE) {
        printf(ANSI_COLOR_RED "Unavailable on this platform tests:" ANSI_COLOR_RESET "\n");
        for(uint32_t index = 0; index < test_count; index++) {
            if(config_manager_get_test_by_index(config_manager, index, &test) == 0 &&
               !test_manager_test_is_available(test_manager, test.name)) {
                print_test(&test);
            }
        }
    }
    return 0;
}

static int run_test(runtime_manager_t *runtime, validation_system_t *system,
    const test_def_t *test) {
    test_manager_t *test_manager = runtime_manager_get_test_manager(runtime);
    test_engine_t *test_engine = runtime_manager_get_test_engine(runtime);
    test_result_t result = {0};

    if(!test_manager_test_is_available(test_manager, test->name)) {
        fprintf(stderr, "Test '%s' is unavailable on this platform.\n", test->name);
        return -1;
    }
    if(!validation_system_is_implemented(system, test->name)) {
        fprintf(stderr, "Test '%s' is supported but not implemented.\n", test->name);
        return -1;
    }
    if(validation_system_register(system, test->name) != 0 ||
       test_engine_execute(test_engine, test->name) != 0 ||
       wait_for_test_result(test_engine, test->name, &result) != 0) {
        fprintf(stderr, "Unable to run test '%s'.\n", test->name);
        return -1;
    }

    printf("%-32s %s (%u ms)\n", result.name,
        result_state_name(result.state), result.execution_time_ms);
    return result.state == TEST_STATE_PASS ? 0 : -1;
}

static int run_all_tests(runtime_manager_t *runtime, validation_system_t *system) {
    config_manager_t *config_manager = runtime_manager_get_config_manager(runtime);
    test_manager_t *test_manager = runtime_manager_get_test_manager(runtime);
    test_engine_t *test_engine = runtime_manager_get_test_engine(runtime);
    uint32_t test_count = 0;
    uint32_t passed = 0, failed = 0, skipped = 0;
    test_def_t test = {0};

    if(config_manager_get_test_count(config_manager, &test_count) != 0 ||
       validation_system_register_all(system) != 0 ||
       test_engine_execute_all(test_engine) != 0) {
        return -1;
    }

    for(uint32_t index = 0; index < test_count; index++) {
        test_result_t result = {0};
        if(config_manager_get_test_by_index(config_manager, index, &test) != 0) {
            failed++;
            continue;
        }
        if(!test_manager_test_is_available(test_manager, test.name) ||
           !validation_system_is_implemented(system, test.name)) {
            printf("%-32s SKIP\n", test.name);
            skipped++;
            continue;
        }
        if(wait_for_test_result(test_engine, test.name, &result) != 0 ||
           result.state != TEST_STATE_PASS) {
            printf("%-32s FAIL\n", test.name);
            failed++;
            continue;
        }
        printf("%-32s PASS (%u ms)\n", result.name, result.execution_time_ms);
        passed++;
    }

    printf("Summary: passed=%u failed=%u skipped=%u\n", passed, failed, skipped);
    return failed == 0 ? 0 : -1;
}

int32_t main(int argc, char *argv[]) {
    runtime_manager_t *runtime = NULL;
    validation_system_t *validation_system = NULL;
    cli_options_t options;
    int status = EXIT_FAILURE;

    if(logger_init() != 0) {
        return EXIT_FAILURE;
    }
    if(cli_parse(argc, argv, &options) != 0) {
        cli_print_usage(argv[0]);
        status = 2;
        goto cleanup;
    }

    runtime = runtime_manager_init(options.config_path);
    if(runtime == NULL) {
        goto cleanup;
    }
    validation_system = validation_system_init(runtime);
    if(validation_system == NULL) {
        goto cleanup;
    }

    if(options.action == CLI_ACTION_LIST) {
        status = list_tests(runtime, validation_system, options.list_filter) == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
    } else if(options.action == CLI_ACTION_RUN) {
        test_def_t test = {0};
        config_manager_t *config_manager = runtime_manager_get_config_manager(runtime);
        if(find_configured_test(config_manager, options.test_name, &test) != 0) {
            fprintf(stderr, "Unknown test '%s'.\n", options.test_name);
        } else {
            status = run_test(runtime, validation_system, &test) == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
        }
    } else if(options.action == CLI_ACTION_RUN_ALL) {
        status = run_all_tests(runtime, validation_system) == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
    }

cleanup:
    if(validation_system != NULL) {
        validation_system_deinit(validation_system);
    }
    if(runtime != NULL) {
        runtime_manager_deinit(runtime);
    }
    logger_deinit();
    return status;
}
