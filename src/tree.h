#ifndef CGIT_TREE_H
#define CGIT_TREE_H

#include "index.h"

int tree_create(const IndexEntry *entries,
                int count,
                char tree_id[CGIT_OBJECT_ID_SIZE]);

#endif