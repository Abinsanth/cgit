#ifndef CGIT_COMMIT_H
#define CGIT_COMMIT_H

#include <stddef.h>

#include "object.h"

int commit_create(const char *tree_id,
                  const char *parent_id,
                  const char *author,
                  const char *message,
                  char commit_id[CGIT_OBJECT_ID_SIZE],
                  unsigned char *buffer,
                  size_t buffer_size,
                  size_t *length);

#endif