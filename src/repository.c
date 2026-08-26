#include <stdio.h>
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