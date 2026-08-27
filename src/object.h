#ifndef CGIT_OBJECT_H
#define CGIT_OBJECT_H

#include <stddef.h>

#define CGIT_OBJECT_ID_SIZE 41

int object_create_blob(const unsigned char *data,
                       size_t length,
                       char object_id[CGIT_OBJECT_ID_SIZE]);

int object_store(const char *object_id,
                 const unsigned char *data,
                 size_t length);

int object_read(const char *object_id,
                unsigned char *buffer,
                size_t buffer_size,
                size_t *bytes_read);

int object_read_file(const char *path,
                     unsigned char **data,
                     size_t *length);

#endif