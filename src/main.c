#include <stdio.h>

#include "commit.h"
#include "object.h"

int main(void)
{
    unsigned char buffer[4096];
    size_t length;

    char commit_id[CGIT_OBJECT_ID_SIZE];

    if (commit_create(
            "1234567890abcdef1234567890abcdef12345678",
            NULL,
            "Abin Santh",
            "initial commit",
            commit_id,
            buffer,
            sizeof(buffer),
            &length) != 0)
    {
        fprintf(stderr, "Failed to create commit\n");
        return 1;
    }

    printf("Commit ID: %s\n", commit_id);

    if (object_store(commit_id, buffer, length) != 0)
    {
        fprintf(stderr, "Failed to store commit\n");
        return 1;
    }

    printf("Commit stored\n");

    return 0;
}