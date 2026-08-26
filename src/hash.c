#include <stdio.h>

#include <CommonCrypto/CommonDigest.h>

#include "hash.h"

int hash_sha1(const unsigned char *data,
              size_t length,
              char output[CGIT_SHA1_HEX_SIZE])
{
    unsigned char digest[CGIT_SHA1_SIZE];

    if (CC_SHA1(data, (CC_LONG)length, digest) == NULL)
    {
        return 1;
    }

    for (size_t i = 0; i < CGIT_SHA1_SIZE; i++)
    {
        snprintf(&output[i * 2],
                 3,
                 "%02x",
                 digest[i]);
    }

    output[CGIT_SHA1_HEX_SIZE - 1] = '\0';

    return 0;
}