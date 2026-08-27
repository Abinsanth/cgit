#ifndef CGIT_INDEX_H
#define CGIT_INDEX_H

#define CGIT_OBJECT_ID_SIZE 41
#define CGIT_INDEX_PATH_SIZE 256

typedef struct
{
    char path[CGIT_INDEX_PATH_SIZE];
    char object_id[CGIT_OBJECT_ID_SIZE];
    unsigned int mode;
} IndexEntry;

int index_add_entry(const IndexEntry *entry);
int index_write_entry(const IndexEntry *entry);
int index_read_entries(IndexEntry *entries, int max_entries);

#endif