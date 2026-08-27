#include <stdio.h>
#include <sys/stat.h>
#include "hash.h"
#include "object.h"

int object_create_blob(const unsigned char *data,
                       size_t length,
                       char object_id[CGIT_OBJECT_ID_SIZE])
{
    if (hash_sha1(data, length, object_id) != 0)
    {
        fprintf(stderr, "Failed to calculate blob hash\n");
        return 1;
    }

    return 0;
}

int object_store(const char *object_id,
                 const unsigned char *data,
                 size_t length)
{
    char directory[64];
    char path[128];

    snprintf(directory,
             sizeof(directory),
             ".cgit/objects/%.2s",
             object_id);

    snprintf(path,
             sizeof(path),
             ".cgit/objects/%.2s/%s",
             object_id,
             object_id + 2);

    /*
     * The object directory must exist before
     * the object file can be created.
     */
    if (mkdir(".cgit/objects", 0755) != 0)
    {
        /*
         * It may already exist.
         * We'll improve this error handling later.
         */
    }

    if (mkdir(directory, 0755) != 0)
    {
        /*
         * It may already exist.
         */
    }

    FILE *file = fopen(path, "wb");

    if (file == NULL)
    {
        fprintf(stderr, "Failed to store object\n");
        return 1;
    }

    if (fwrite(data, 1, length, file) != length)
    {
        fclose(file);
        fprintf(stderr, "Failed to write object\n");
        return 1;
    }

    fclose(file);

    return 0;
}