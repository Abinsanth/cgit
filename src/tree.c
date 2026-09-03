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