#ifndef __HASH_H__
#define __HASH_H__

typedef struct _hashelem
{
  char             *name;
  struct _hashelem *next;
  int               index; /* for row and column name hash tables */
  struct _hashelem *nextelem;
} hashelem;

typedef struct _hashstruct
{
  hashelem **table;
  int        size;
  struct _hashelem *first, *last;
} hashstruct;

hashstruct *create_hash_table(int size, int base);
void free_hash_table(hashstruct *ht);
hashelem *findhash(const char *name, hashstruct *ht);
hashelem *puthash(const char *name, int varindex, hashelem **list, hashstruct *ht);
hashstruct *copy_hash_table(hashstruct *ht);

#endif
