#ifndef CGIT_DIFF_H
#define CGIT_DIFF_H

#include <stddef.h>

int diff_text(const unsigned char *old_data,
              size_t old_length,
              const unsigned char *new_data,
              size_t new_length);

#endif