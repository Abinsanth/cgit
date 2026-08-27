#include <stdio.h>
#include <sys/stat.h>
#include "hash.h"
#include "object.h"
#include <stdlib.h>

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

int object_read(const char *object_id,
                unsigned char *buffer,
                size_t buffer_size,
                size_t *bytes_read)
{
    char path[128];

    snprintf(path,
             sizeof(path),
             ".cgit/objects/%.2s/%s",
             object_id,
             object_id + 2);

    FILE *file = fopen(path, "rb");

    if (file == NULL)
    {
        return 1;
    }

    size_t count = fread(buffer, 1, buffer_size, file);

    if (ferror(file))
    {
        fclose(file);
        return 1;
    }

    fclose(file);

    *bytes_read = count;

    return 0;
}

int object_read_file(const char *path,
                     unsigned char **data,
                     size_t *length)
{
    FILE *file = fopen(path, "rb");

    if (file == NULL)
    {
        return 1;
    }

    if (fseek(file, 0, SEEK_END) != 0)
    {
        fclose(file);
        return 1;
    }

    long file_size = ftell(file);

    if (file_size < 0)
    {
        fclose(file);
        return 1;
    }

    rewind(file);

    unsigned char *buffer = malloc((size_t)file_size);

    if (buffer == NULL && file_size > 0)
    {
        fclose(file);
        return 1;
    }

    size_t bytes_read = fread(
        buffer,
        1,
        (size_t)file_size,
        file);

    fclose(file);

    if (bytes_read != (size_t)file_size)
    {
        free(buffer);
        return 1;
    }

    *data = buffer;
    *length = bytes_read;

    return 0;
}