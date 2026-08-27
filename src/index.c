#include <stdio.h>

#include "index.h"

int index_write_entry(const IndexEntry *entry)
{
    FILE *file = fopen(".cgit/index", "a");

    if (file == NULL)
    {
        return 1;
    }

    if (fprintf(file,
                "%u %s %s\n",
                entry->mode,
                entry->object_id,
                entry->path) < 0)
    {
        fclose(file);
        return 1;
    }

    fclose(file);

    return 0;
}

int index_read_entries(IndexEntry *entries, int max_entries)
{
    FILE *file = fopen(".cgit/index", "r");

    if (file == NULL)
    {
        return 0;
    }

    int count = 0;

    while (count < max_entries)
    {
        int result = fscanf(file,
                            "%u %40s %255[^\n]\n",
                            &entries[count].mode,
                            entries[count].object_id,
                            entries[count].path);

        if (result != 3)
        {
            break;
        }

        count++;
    }

    fclose(file);

    return count;
}