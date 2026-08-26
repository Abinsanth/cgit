#ifndef CGIT_OBJECT_H
#define CGIT_OBJECT_H

#include <stddef.h>

int object_create_blob(const unsigned char *data,
                       size_t length,
                       char object_id[41]);

#endif