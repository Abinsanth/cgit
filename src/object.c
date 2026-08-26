#include <stdio.h>

#include "hash.h"
#include "object.h"

int object_create_blob(const unsigned char *data,
                       size_t length,
                       char object_id[41])
{
    if (hash_sha1(data, length, object_id) != 0)
    {
        fprintf(stderr, "Failed to calculate blob hash\n");
        return 1;
    }

    return 0;
}