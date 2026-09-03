#ifndef CGIT_REPOSITORY_H
#define CGIT_REPOSITORY_H

#include <stddef.h>

int repository_exists(void);
int repository_create(void);
int repository_create_head(void);

int repository_read_head(char *branch, size_t branch_size);
int repository_read_branch(const char *branch,
                           char *commit_id,
                           size_t commit_id_size);
int repository_update_branch(const char *branch,
                             const char *commit_id);

#endif