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
#include "diff.h"

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
        return cli_error("failed to create .cgit directory");
    }

    if (repository_create_head() != 0)
    {
        return cli_error("failed to create HEAD file");
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

    if (strcmp(argv[1], "log") == 0)
    {
        return cli_log();
    }

    if (strcmp(argv[1], "diff") == 0)
    {
        return cli_diff();
    }

    if (strcmp(argv[1], "branch") == 0)
    {
        if (argc == 2)
        {
            return cli_branch_list();
        }

        if (argc == 3)
        {
            return cli_branch(argv[2]);
        }

        return cli_error("usage: cgit branch [<name>]");
    }

    if (strcmp(argv[1], "checkout") == 0)
    {
        if (argc != 3)
        {
            return cli_error("usage: cgit checkout <branch>");
        }

        return cli_checkout(argv[2]);
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
        return cli_error("failed to create blob object");
    }

    if (object_store(object_id, data, length) != 0)
    {
        free(data);
        return cli_error("failed to store blob object");
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

static int find_index_entry(IndexEntry *entries,
                            int count,
                            const char *path,
                            IndexEntry *result)
{
    for (int i = 0; i < count; i++)
    {
        if (strcmp(entries[i].path, path) == 0)
        {
            *result = entries[i];
            return 1;
        }
    }

    return 0;
}

int cli_status(void)
{
    IndexEntry entries[100];

    int count = index_read_entries(entries, 100);
    char branch[256];
    char head_commit_id[CGIT_OBJECT_ID_SIZE];
    char head_tree_id[CGIT_OBJECT_ID_SIZE];

    IndexEntry head_entries[100];

    int head_count = 0;

    if (repository_read_head(branch, sizeof(branch)) == 0 &&
        repository_read_branch(
            branch,
            head_commit_id,
            sizeof(head_commit_id)) == 0 &&
        commit_read_tree(
            head_commit_id,
            head_tree_id,
            sizeof(head_tree_id)) == 0)
    {
        head_count = tree_read_entries(
            head_tree_id,
            head_entries,
            100);
    }
    for (int i = 0; i < count; i++)
    {
        IndexEntry head_entry;

        if (!find_index_entry(
                head_entries,
                head_count,
                entries[i].path,
                &head_entry))
        {
            printf("  staged: %s\n", entries[i].path);
            continue;
        }

        if (strcmp(
                entries[i].object_id,
                head_entry.object_id) != 0)
        {
            printf("  staged: %s\n", entries[i].path);
        }
    }

    for (int i = 0; i < head_count; i++)
    {
        IndexEntry index_entry;

        if (!find_index_entry(
                entries,
                count,
                head_entries[i].path,
                &index_entry))
        {
            printf("  staged: %s\n", head_entries[i].path);
        }
    }

    printf("Changes to be committed:\n");

    for (int i = 0; i < count; i++)
    {

        char current_id[CGIT_OBJECT_ID_SIZE];

        if (object_hash_file(entries[i].path, current_id) != 0)
        {
            printf("  deleted: %s\n", entries[i].path);
            continue;
        }

        if (strcmp(current_id, entries[i].object_id) != 0)
        {
            printf("  modified: %s\n", entries[i].path);
        }
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
        return cli_error("failed to create tree object");
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
        return cli_error("failed to store tree object");
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
        return cli_error("failed to create commit object");
    }

    if (object_store(
            commit_id,
            commit_buffer,
            commit_length) != 0)
    {
        return cli_error("failed to store commit object");
    }

    if (repository_update_branch(branch, commit_id) != 0)
    {
        return cli_error("failed to update branch reference");
    }

    printf("[%s] %s\n", commit_id, message);

    return 0;
}
/*
 * Display commit history starting from the current branch.
 *
 * Each commit points to its parent, allowing us to walk
 * backward through the repository history.
 */
int cli_log(void)
{
    char branch[256];

    if (repository_read_head(branch, sizeof(branch)) != 0)
    {
        return cli_error("failed to read HEAD");
    }

    char commit_id[CGIT_OBJECT_ID_SIZE];

    if (repository_read_branch(
            branch,
            commit_id,
            sizeof(commit_id)) != 0)
    {
        return cli_error("no commits yet");
    }

    while (1)
    {
        char object_path[512];

        snprintf(
            object_path,
            sizeof(object_path),
            ".cgit/objects/%c%c/%s",
            commit_id[0],
            commit_id[1],
            commit_id + 2);

        FILE *file = fopen(object_path, "r");

        if (file == NULL)
        {
            return cli_error("failed to read commit");
        }

        char line[4096];

        char parent_id[CGIT_OBJECT_ID_SIZE];
        int has_parent = 0;

        printf("commit %s\n", commit_id);

        while (fgets(line, sizeof(line), file) != NULL)
        {
            if (strncmp(line, "parent ", 7) == 0)
            {
                snprintf(
                    parent_id,
                    sizeof(parent_id),
                    "%s",
                    line + 7);

                parent_id[strcspn(parent_id, "\n")] = '\0';

                has_parent = 1;
            }
            else if (strncmp(line, "author ", 7) == 0)
            {
                printf("Author: %s", line + 7);
            }
            else if (strncmp(line, "message ", 8) == 0)
            {
                printf("\n    %s", line + 8);
            }
        }

        fclose(file);

        printf("\n");

        if (!has_parent)
        {
            break;
        }

        snprintf(
            commit_id,
            sizeof(commit_id),
            "%s",
            parent_id);
    }

    return 0;
}
int cli_diff(void)
{
    char branch[256];
    char commit_id[CGIT_OBJECT_ID_SIZE];
    char tree_id[CGIT_OBJECT_ID_SIZE];

    if (repository_read_head(branch, sizeof(branch)) != 0)
    {
        return cli_error("failed to read HEAD");
    }

    if (repository_read_branch(
            branch,
            commit_id,
            sizeof(commit_id)) != 0)
    {
        return cli_error("no commits yet");
    }

    if (commit_read_tree(
            commit_id,
            tree_id,
            sizeof(tree_id)) != 0)
    {
        return cli_error("failed to read tree object");
    }

    IndexEntry head_entries[100];

    int head_count = tree_read_entries(
        tree_id,
        head_entries,
        100);

    if (head_count < 0)
    {
        return cli_error("failed to read tree entries");
    }
    for (int i = 0; i < head_count; i++)
    {
        unsigned char *old_data;
        size_t old_length;

        if (object_read_blob(head_entries[i].object_id,
                             &old_data,
                             &old_length) != 0)
        {
            return cli_error("failed to read blob object");
        }

        unsigned char *new_data;
        size_t new_length;

        if (object_read_file(head_entries[i].path,
                             &new_data,
                             &new_length) != 0)
        {
            free(old_data);
            continue;
        }

        printf("diff -- %s\n", head_entries[i].path);
        printf("--- HEAD\n");
        printf("+++ working tree\n");

        diff_text(
            old_data,
            old_length,
            new_data,
            new_length);

        free(old_data);
        free(new_data);
    }

    return 0;
}

int cli_branch(const char *branch_name)
{
    char current_branch[256];
    char commit_id[CGIT_OBJECT_ID_SIZE];

    if (repository_read_head(
            current_branch,
            sizeof(current_branch)) != 0)
    {
        return cli_error("failed to read HEAD");
    }

    if (repository_read_branch(
            current_branch,
            commit_id,
            sizeof(commit_id)) != 0)
    {
        return cli_error("no commits yet");
    }

    if (repository_create_branch(
            branch_name,
            commit_id) != 0)
    {
        return cli_error("failed to create branch");
    }

    printf("Created branch '%s'\n", branch_name);

    return 0;
}

int cli_branch_list(void)
{
    char current_branch[256];

    if (repository_read_head(
            current_branch,
            sizeof(current_branch)) != 0)
    {
        return cli_error("failed to read HEAD");
    }

    DIR *directory = opendir(".cgit/refs/heads");

    if (directory == NULL)
    {
        return cli_error("failed to open branch references directory");
    }

    struct dirent *entry;

    while ((entry = readdir(directory)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        if (strcmp(entry->d_name, current_branch) == 0)
        {
            printf("* %s\n", entry->d_name);
        }
        else
        {
            printf("  %s\n", entry->d_name);
        }
    }

    closedir(directory);

    return 0;
}

int cli_checkout(const char *branch_name)
{
    char commit_id[CGIT_OBJECT_ID_SIZE];

    if (repository_read_branch(
            branch_name,
            commit_id,
            sizeof(commit_id)) != 0)
    {
        return cli_error("branch not found");
    }

    if (repository_update_head(branch_name) != 0)
    {
        return cli_error("failed to update HEAD reference");
    }

    printf("Switched to branch '%s'\n", branch_name);

    return 0;
}