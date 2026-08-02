#include <stdio.h>
#include <string.h>

#include "cli-parser.h"

#define DEFAULT_CONFIG_PATH "config/json"

static int parse_list_filter(const char *argument, cli_list_filter_t *filter) {
    if(strcmp(argument, "--runnable-test") == 0 || strcmp(argument, "-R") == 0 ||
       strcmp(argument, "--runable-test") == 0) {
        *filter = CLI_LIST_RUNNABLE;
        return 0;
    }
    if(strcmp(argument, "--unimplemented-test") == 0 || strcmp(argument, "-n") == 0) {
        *filter = CLI_LIST_UNIMPLEMENTED;
        return 0;
    }
    if(strcmp(argument, "--unsupported-list") == 0 || strcmp(argument, "-u") == 0 ||
       strcmp(argument, "--unavailable-test") == 0) {
        *filter = CLI_LIST_UNAVAILABLE;
        return 0;
    }
    return -1;
}

int cli_parse(int argc, char *argv[], cli_options_t *options) {
    if(options == NULL) {
        return -1;
    }
    memset(options, 0, sizeof(*options));
    options->action = CLI_ACTION_INVALID;
    options->list_filter = CLI_LIST_ALL;

    if(argc >= 2 && (strcmp(argv[1], "--list") == 0 || strcmp(argv[1], "-l") == 0) && argc <= 3) {
        options->config_path = DEFAULT_CONFIG_PATH;
        options->action = CLI_ACTION_LIST;
        return argc == 2 || parse_list_filter(argv[2], &options->list_filter) == 0 ? 0 : -1;
    }
    if(argc >= 3 && (strcmp(argv[2], "--list") == 0 || strcmp(argv[2], "-l") == 0) && argc <= 4) {
        options->config_path = argv[1];
        options->action = CLI_ACTION_LIST;
        return argc == 3 || parse_list_filter(argv[3], &options->list_filter) == 0 ? 0 : -1;
    }
    if(argc == 3 && (strcmp(argv[2], "--run-all") == 0 || strcmp(argv[2], "-a") == 0)) {
        options->config_path = argv[1];
        options->action = CLI_ACTION_RUN_ALL;
        return 0;
    }
    if(argc == 4 && (strcmp(argv[2], "--run") == 0 || strcmp(argv[2], "-r") == 0)) {
        options->config_path = argv[1];
        options->test_name = argv[3];
        options->action = CLI_ACTION_RUN;
        return 0;
    }
    return -1;
}

void cli_print_usage(const char *program_name) {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "  %s [config_path] --list|-l [--runnable-test|-R | --unimplemented-test|-n | --unsupported-list|-u]\n", program_name);
    fprintf(stderr, "  %s <config_path> --run|-r <test_name>\n", program_name);
    fprintf(stderr, "  %s <config_path> --run-all|-a\n", program_name);
}
