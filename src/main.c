// #include "cli.h"

// int main(int argc, char *argv[])
// {
//     return cli_run(argc, argv);
// }

#include <stdio.h>
#include <string.h>

#include "object.h"

int main(void)
{
    const char *content = "Hello!";

    char object_id[41];

    if (object_create_blob(
            (const unsigned char *)content,
            strlen(content),
            object_id) != 0)
    {
        return 1;
    }

    printf("Blob ID: %s\n", object_id);

    return 0;
}