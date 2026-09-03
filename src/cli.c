#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <time.h>

#include "cli.h"
#include "repository.h"
#include "object.h"
#include "index.h"
#include "commit.h"
#include "tree.h"

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
    if (strcmp(argv[1], "status") == 0)
    {
        return cli_status();
    }
    if (strcmp(argv[1], "commit") == 0)
    {
        if (argc < 4 || strcmp(argv[2], "-m") != 0)
        {
            return cli_error("usage: cgit commit -m \"message\"");
        }

        return cli_commit(argv[3]);
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

static int index_contains_path(IndexEntry *entries,
                               int count,
                               const char *path)
{
    for (int i = 0; i < count; i++)
    {
        if (strcmp(entries[i].path, path) == 0)
        {
            return 1;
        }
    }

    return 0;
}

int cli_status(void)
{
    IndexEntry entries[100];

    int count = index_read_entries(entries, 100);

    printf("Changes to be committed:\n");

    for (int i = 0; i < count; i++)
    {
        unsigned char *data;
        size_t length;

        if (object_read_file(entries[i].path, &data, &length) != 0)
        {
            printf("  deleted: %s\n", entries[i].path);
            continue;
        }

        char current_id[CGIT_OBJECT_ID_SIZE];

        if (object_create_blob(data, length, current_id) != 0)
        {
            free(data);
            return cli_error("failed to calculate file hash");
        }

        if (strcmp(current_id, entries[i].object_id) == 0)
        {
            printf("  staged: %s\n", entries[i].path);
        }
        else
        {
            printf("  modified: %s\n", entries[i].path);
        }

        free(data);
    }

    DIR *directory = opendir(".");

    if (directory == NULL)
    {
        return cli_error("failed to open current directory");
    }

    struct dirent *entry;

    while ((entry = readdir(directory)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0 ||
            strcmp(entry->d_name, ".cgit") == 0)
        {
            continue;
        }

        if (!index_contains_path(entries, count, entry->d_name))
        {
            printf("  untracked: %s\n", entry->d_name);
        }
    }

    closedir(directory);

    return 0;
}
/*
 * Create and store a commit from the current index.
 *
 * The commit points to a tree generated from the staging area.
 */
int cli_commit(const char *message)
{
    IndexEntry entries[100];

    int count = index_read_entries(entries, 100);

    if (count <= 0)
    {
        return cli_error("nothing to commit");
    }

    char tree_id[CGIT_OBJECT_ID_SIZE];

    if (tree_create(entries, count, tree_id) != 0)
    {
        return cli_error("failed to create tree");
    }

    char tree_buffer[4096];

    size_t tree_length = 0;

    for (int i = 0; i < count; i++)
    {
        int written = snprintf(
            tree_buffer + tree_length,
            sizeof(tree_buffer) - tree_length,
            "%u %s %s\n",
            entries[i].mode,
            entries[i].object_id,
            entries[i].path);

        if (written < 0 ||
            (size_t)written >= sizeof(tree_buffer) - tree_length)
        {
            return cli_error("tree is too large");
        }

        tree_length += (size_t)written;
    }

    if (object_store(
            tree_id,
            (const unsigned char *)tree_buffer,
            tree_length) != 0)
    {
        return cli_error("failed to store tree");
    }

    char branch[256];

    if (repository_read_head(branch, sizeof(branch)) != 0)
    {
        return cli_error("failed to read HEAD");
    }

    char parent_id[CGIT_OBJECT_ID_SIZE];
    const char *parent = NULL;

    if (repository_read_branch(
            branch,
            parent_id,
            sizeof(parent_id)) == 0)
    {
        parent = parent_id;
    }

    unsigned char commit_buffer[4096];
    size_t commit_length;

    char commit_id[CGIT_OBJECT_ID_SIZE];

    if (commit_create(
            tree_id,
            parent,
            "Abin Santh",
            message,
            commit_id,
            commit_buffer,
            sizeof(commit_buffer),
            &commit_length) != 0)
    {
        return cli_error("failed to create commit");
    }

    if (object_store(
            commit_id,
            commit_buffer,
            commit_length) != 0)
    {
        return cli_error("failed to store commit");
    }

    if (repository_update_branch(branch, commit_id) != 0)
    {
        return cli_error("failed to update branch");
    }

    printf("[%s] %s\n", commit_id, message);

    return 0;
}