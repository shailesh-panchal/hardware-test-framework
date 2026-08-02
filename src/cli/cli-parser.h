#ifndef CLI_PARSER_H
#define CLI_PARSER_H

typedef enum {
    CLI_ACTION_INVALID = 0,
    CLI_ACTION_LIST,
    CLI_ACTION_RUN,
    CLI_ACTION_RUN_ALL
} cli_action_t;

typedef enum {
    CLI_LIST_ALL = 0,
    CLI_LIST_RUNNABLE,
    CLI_LIST_UNIMPLEMENTED,
    CLI_LIST_UNAVAILABLE
} cli_list_filter_t;

typedef struct {
    cli_action_t action;
    const char *config_path;
    const char *test_name;
    cli_list_filter_t list_filter;
} cli_options_t;

int cli_parse(int argc, char *argv[], cli_options_t *options);
void cli_print_usage(const char *program_name);

#endif
