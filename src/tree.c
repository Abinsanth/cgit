/*
 * Tree object creation.
 *
 * A tree represents the directory structure described by
 * the staging index.
 */

#include <stdio.h>
#include <string.h>

#include "hash.h"
#include "tree.h"

int tree_create(const IndexEntry *entries,
                int count,
                char tree_id[CGIT_OBJECT_ID_SIZE])
{
    char buffer[4096];
    size_t offset = 0;

    for (int i = 0; i < count; i++)
    {
        int written = snprintf(
            buffer + offset,
            sizeof(buffer) - offset,
            "%u %s %s\n",
            entries[i].mode,
            entries[i].object_id,
            entries[i].path);

        if (written < 0 ||
            (size_t)written >= sizeof(buffer) - offset)
        {
            return 1;
        }

        offset += (size_t)written;
    }

    return hash_sha1(
        (const unsigned char *)buffer,
        offset,
        tree_id);
}
/*
 * Read entries from a stored tree object.
 *
 * The tree object uses the same text format as the index,
 * so each tree entry can be parsed into an IndexEntry.
 */
int tree_read_entries(const char *tree_id,
                      IndexEntry *entries,
                      int max_entries)
{
    char path[512];

    snprintf(
        path,
        sizeof(path),
        ".cgit/objects/%c%c/%s",
        tree_id[0],
        tree_id[1],
        tree_id + 2);

    FILE *file = fopen(path, "r");

    if (file == NULL)
    {
        return -1;
    }

    int count = 0;

    while (count < max_entries)
    {
        int result = fscanf(
            file,
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