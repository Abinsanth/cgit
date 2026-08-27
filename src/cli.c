#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "cli.h"
#include "repository.h"
#include "object.h"
#include "index.h"

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
    if (strcmp(argv[1], "add") == 0)
    {
        if (argc < 3)
        {
            return cli_error("nothing specified, nothing added");
        }

        for (int i = 2; i < argc; i++)
        {
            if (cli_add(argv[i]) != 0)
            {
                return 1;
            }
        }

        return 0;
    }

    char message[100];

    snprintf(message, sizeof(message),
             "unknown command '%s'", argv[1]);

    return cli_error(message);
}

int cli_add(const char *path)
{
    unsigned char *data;
    size_t length;

    if (object_read_file(path, &data, &length) != 0)
    {
        return cli_error("failed to read file");
    }

    char object_id[CGIT_OBJECT_ID_SIZE];

    if (object_create_blob(data, length, object_id) != 0)
    {
        free(data);
        return cli_error("failed to create blob");
    }

    if (object_store(object_id, data, length) != 0)
    {
        free(data);
        return cli_error("failed to store object");
    }

    IndexEntry entry;

    snprintf(entry.path, sizeof(entry.path), "%s", path);
    snprintf(entry.object_id,
             sizeof(entry.object_id),
             "%s",
             object_id);

    entry.mode = 100644;

    if (index_add_entry(&entry) != 0)
    {
        free(data);
        return cli_error("failed to update index");
    }

    free(data);

    return 0;
}