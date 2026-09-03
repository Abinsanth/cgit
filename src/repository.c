#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "repository.h"

int repository_exists(void)
{
    struct stat info;

    if (stat(".cgit", &info) != 0)
    {
        return 0;
    }

    return S_ISDIR(info.st_mode);
}

int repository_create(void)
{
    if (mkdir(".cgit", 0755) != 0)
    {
        return 1;
    }

    if (mkdir(".cgit/objects", 0755) != 0)
    {
        return 1;
    }

    if (mkdir(".cgit/refs", 0755) != 0)
    {
        return 1;
    }

    if (mkdir(".cgit/refs/heads", 0755) != 0)
    {
        return 1;
    }

    return 0;
}

int repository_create_head(void)
{
    FILE *file = fopen(".cgit/HEAD", "w");

    if (file == NULL)
    {
        return 1;
    }

    fprintf(file, "ref: refs/heads/main\n");

    fclose(file);

    return 0;
}
/*
 * Read the branch referenced by HEAD.
 *
 * HEAD contains a line such as:
 * ref: refs/heads/main
 */
int repository_read_head(char *branch, size_t branch_size)
{
    FILE *file = fopen(".cgit/HEAD", "r");

    if (file == NULL)
    {
        return 1;
    }

    char reference[256];

    if (fgets(reference, sizeof(reference), file) == NULL)
    {
        fclose(file);
        return 1;
    }

    fclose(file);

    if (strncmp(reference, "ref: refs/heads/", 16) != 0)
    {
        return 1;
    }

    char *name = reference + 16;

    name[strcspn(name, "\n")] = '\0';

    if (strlen(name) + 1 > branch_size)
    {
        return 1;
    }

    snprintf(branch, branch_size, "%s", name);

    return 0;
}

int repository_read_branch(const char *branch,
                           char *commit_id,
                           size_t commit_id_size)
{
    char path[512];

    snprintf(
        path,
        sizeof(path),
        ".cgit/refs/heads/%s",
        branch);

    FILE *file = fopen(path, "r");

    if (file == NULL)
    {
        return 1;
    }

    if (fgets(commit_id, commit_id_size, file) == NULL)
    {
        fclose(file);
        return 1;
    }

    fclose(file);

    commit_id[strcspn(commit_id, "\n")] = '\0';

    return 0;
}
int repository_update_branch(const char *branch,
                             const char *commit_id)
{
    char path[512];

    snprintf(
        path,
        sizeof(path),
        ".cgit/refs/heads/%s",
        branch);

    FILE *file = fopen(path, "w");

    if (file == NULL)
    {
        return 1;
    }

    fprintf(file, "%s\n", commit_id);

    fclose(file);

    return 0;
}