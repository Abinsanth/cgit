/*
 * Commit object creation.
 *
 * A commit records a tree, optional parent, author information,
 * and the commit message.
 */

#include <stdio.h>
#include <string.h>
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
/*
 * Read the tree ID referenced by a commit.
 */
int commit_read_tree(const char *commit_id,
                     char *tree_id,
                     size_t tree_id_size)
{
    char path[512];

    snprintf(
        path,
        sizeof(path),
        ".cgit/objects/%c%c/%s",
        commit_id[0],
        commit_id[1],
        commit_id + 2);

    FILE *file = fopen(path, "r");

    if (file == NULL)
    {
        return 1;
    }

    char line[4096];

    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (strncmp(line, "tree ", 5) == 0)
        {
            char *value = line + 5;

            value[strcspn(value, "\n")] = '\0';

            if (strlen(value) + 1 > tree_id_size)
            {
                fclose(file);
                return 1;
            }

            snprintf(tree_id, tree_id_size, "%s", value);

            fclose(file);
            return 0;
        }
    }

    fclose(file);

    return 1;
}