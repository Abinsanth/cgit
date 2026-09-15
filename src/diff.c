#include <stdio.h>
#include <string.h>

#include "diff.h"

int diff_text(const unsigned char *old_data,
              size_t old_length,
              const unsigned char *new_data,
              size_t new_length)
{
    size_t old_position = 0;
    size_t new_position = 0;

    while (old_position < old_length ||
           new_position < new_length)
    {
        size_t old_end = old_position;
        size_t new_end = new_position;

        while (old_end < old_length &&
               old_data[old_end] != '\n')
        {
            old_end++;
        }

        while (new_end < new_length &&
               new_data[new_end] != '\n')
        {
            new_end++;
        }

        size_t old_line_length = old_end - old_position;
        size_t new_line_length = new_end - new_position;

        if (old_line_length == new_line_length &&
            memcmp(old_data + old_position,
                   new_data + new_position,
                   old_line_length) == 0)
        {
            old_position = old_end;
            new_position = new_end;

            if (old_position < old_length &&
                old_data[old_position] == '\n')
            {
                old_position++;
            }

            if (new_position < new_length &&
                new_data[new_position] == '\n')
            {
                new_position++;
            }

            continue;
        }

        if (old_position < old_length)
        {
            printf("- %.*s\n",
                   (int)old_line_length,
                   old_data + old_position);
        }

        if (new_position < new_length)
        {
            printf("+ %.*s\n",
                   (int)new_line_length,
                   new_data + new_position);
        }

        old_position = old_end;
        new_position = new_end;

        if (old_position < old_length &&
            old_data[old_position] == '\n')
        {
            old_position++;
        }

        if (new_position < new_length &&
            new_data[new_position] == '\n')
        {
            new_position++;
        }
    }

    return 0;
}