/*
 * Commit object creation.
 *
 * A commit records a tree, optional parent, author information,
 * and the commit message.
 */

#include <stdio.h>
#include <time.h>

#include "commit.h"
#include "hash.h"

int commit_create(const char *tree_id,
                  const char *parent_id,
                  const char *author,
                  const char *message,
                  char commit_id[CGIT_OBJECT_ID_SIZE],
                  unsigned char *buffer,
                  size_t buffer_size,
                  size_t *length)
{
    time_t timestamp = time(NULL);

    if (timestamp == (time_t)-1)
    {
        return 1;
    }

    int written;

    if (parent_id != NULL)
    {
        written = snprintf(
            (char *)buffer,
            buffer_size,
            "tree %s\n"
            "parent %s\n"
            "author %s %ld\n"
            "message %s\n",
            tree_id,
            parent_id,
            author,
            (long)timestamp,
            message);
    }
    else
    {
        written = snprintf(
            (char *)buffer,
            buffer_size,
            "tree %s\n"
            "author %s %ld\n"
            "message %s\n",
            tree_id,
            author,
            (long)timestamp,
            message);
    }

    if (written < 0 || (size_t)written >= buffer_size)
    {
        return 1;
    }

    *length = (size_t)written;

    return hash_sha1(
        buffer,
        *length,
        commit_id);
}