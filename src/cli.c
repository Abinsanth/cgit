#include <stdio.h>
#include <string.h>

#include "cli.h"
#include "repository.h"

int cli_version(void)
{
    printf("cgit version 0.1.0\n");
    return 0;
}

int cli_help(void)
{
    printf("Usage: cgit <command>\n\n");
    printf("Available commands:\n");
    printf("  help       Show this help message\n");
    printf("  version    Show cgit version\n");
    printf("  init       Initialize a repository\n");
    printf("  status     Show repository status\n");

    return 0;
}

int cli_error(const char *message)
{
    fprintf(stderr, "cgit: %s\n", message);
    return 1;
}

int cli_init(void)
{
    if (repository_exists())
    {
        return cli_error("repository already exists");
    }

    if (repository_create() != 0)
    {
        return cli_error("failed to create repository");
    }

    if (repository_create_head() != 0)
    {
        return cli_error("failed to create HEAD");
    }

    printf("Initialized empty cgit repository\n");

    return 0;
}

int cli_run(int argc, char *argv[])
{
    if (argc < 2)
    {
        return cli_error("no command specified");
    }

    if (strcmp(argv[1], "version") == 0)
    {
        return cli_version();
    }

    if (strcmp(argv[1], "help") == 0)
    {
        return cli_help();
    }
    if (strcmp(argv[1], "init") == 0)
    {
        return cli_init();
    }

    char message[100];

    snprintf(message, sizeof(message),
             "unknown command '%s'", argv[1]);

    return cli_error(message);
}
