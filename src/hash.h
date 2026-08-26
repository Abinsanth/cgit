#ifndef CGIT_HASH_H
#define CGIT_HASH_H

#include <stddef.h>

#define CGIT_SHA1_SIZE 20
#define CGIT_SHA1_HEX_SIZE 41

int hash_sha1(const unsigned char *data,
              size_t length,
              char output[CGIT_SHA1_HEX_SIZE]);

#endif