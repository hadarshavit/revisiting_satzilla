#include <string.h>
#include <limits.h>
#include "lpkit.h" /* only for MALLOC, CALLOC */
#include "hash.h"

/* hash functions for open hashing */

hashstruct *create_hash_table(int size, int base)
{
  hashstruct *ht;

  if (CALLOC(ht, 1, hashstruct) != NULL) {
    if (CALLOC(ht->table, size, hashelem *) == NULL) {
      FREE(ht);
    }
    else
      ht->size = size;
  }
  return(ht);
}

void free_hash_table(hashstruct *ht)
{
  /* int i; */
  hashelem *hp, *thp;

  /* for(i = 0; i < ht->size; i++) { */
    /* hp = ht->table[i]; */
    hp = ht->first;
    while(hp != NULL) {
      thp = hp;
      /* hp = hp->next; */
      hp = hp->nextelem;
      free(thp->name);
      free(thp);
    }
  /* } */
  free(ht->table);
  free(ht);
}

/* make a good hash function for any int size */
/* inspired by Aho, Sethi and Ullman, Compilers ..., p436 */
#define HASH_1 sizeof(unsigned int)
#define HASH_2 (sizeof(unsigned int) * 6)
#define HASH_3 (((unsigned int)0xF0) << ((sizeof(unsigned int) - 1) * CHAR_BIT))

static int hashval(const char *string, int size)
{
  unsigned int result = 0, tmp;

  for(; *string; string++) {
    result = (result << HASH_1) + *string;
    if((tmp = result & HASH_3) != 0) {
      /* if any of the most significant bits is on */
      result ^= tmp >> HASH_2; /* xor them in in a less significant part */
      result ^= tmp; /* and reset the most significant bits to 0 */
    }
  }
  return(result % size);
} /* hashval */

hashelem *findhash(const char *name, hashstruct *ht)
{
  hashelem *h_tab_p;

  if(ht == NULL)
    h_tab_p = NULL;
  else
    for(h_tab_p = ht->table[hashval(name, ht->size)];
        h_tab_p != NULL;
        h_tab_p = h_tab_p->next)
      if(strcmp(name, h_tab_p->name) == 0) /* got it! */
        break;
  return(h_tab_p);
} /* findhash */

hashelem *puthash(const char *name, int varindex, hashelem **list, hashstruct *ht)
{
  hashelem *hp;
  int index;

  if((hp = findhash(name, ht)) == NULL) {
    index = hashval(name, ht->size);
    if (CALLOC(hp, 1, hashelem) != NULL) {
      if (MALLOC(hp->name, strlen(name) + 1, char) == NULL) {
        FREE(hp);
      }
      else {
	strcpy(hp->name, name);
	hp->index = varindex;
	if(list != NULL)
	  list[varindex] = hp;
	hp->next = ht->table[index];
	ht->table[index] = hp;
	if(ht->first == NULL)
	  ht->first = hp;
	if(ht->last != NULL)
	  ht->last->nextelem = hp;
	ht->last = hp;
      }
    }
  }
  return(hp);
}

hashstruct *copy_hash_table(hashstruct *ht)
{
  hashstruct *copy;
  hashelem *elem, *new_elem;
  /* int i; */

  copy = create_hash_table(ht->size, 0);
  if (copy != NULL) {
    /* for(i = 0; i < ht->size; i++) { */
      /* for(elem = ht->table[i]; elem != NULL; elem = elem->next) { */
    elem = ht->first;
    while (elem != NULL) {
        if((new_elem = puthash(elem->name, elem->index, NULL, copy)) == NULL) {
        /* if (CALLOC(new_elem, 1) == NULL) { */
	  free_hash_table(copy);
	  return(NULL);
        }
	/* ... but link it into the new list */
	/* new_elem->next = copy->table[i]; */
	/* copy->table[i] = new_elem; */
	elem = elem ->nextelem;
    }
  }

  return(copy);
}
