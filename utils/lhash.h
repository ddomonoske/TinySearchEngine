#pragma once
/*
 * lhash.h -- A locked version of hash.h
 *
 * Author: David Domonoske
 *
 */
#include "hash.h"
#include <pthread.h>

typedef void lhashtable_t;	/* representation of a hashtable hidden */

/* setHashDelay -- set delay_time */
void setHashDelay(uint32_t time);

/* hopen -- opens a hash table with initial size hsize */
lhashtable_t *lhopen(uint32_t hsize);

/* hclose -- closes a hash table */
void lhclose(lhashtable_t *htp);

/* hput -- puts an entry into a hash table under designated key 
 * returns 0 for success; non-zero otherwise
 */
int32_t lhput(lhashtable_t *htp, void *ep, const char *key, int keylen);

/* happly -- applies a function to every entry in hash table */
void lhapply(lhashtable_t *htp, void (*fn)(void* ep));

/* hsearch -- searchs for an entry under a designated key using a
 * designated search fn -- retcurns a pointer to the entry or NULL if
 * not found
 */
void *lhsearch(lhashtable_t *htp, 
	      bool (*searchfn)(void* elementp, const void* searchkeyp), 
	      const char *key, 
	      int32_t keylen);

/* hremove -- removes and returns an entry under a designated key
 * using a designated search fn -- returns a pointer to the entry or
 * NULL if not found
 */
void *lhremove(lhashtable_t *htp, 
	      bool (*searchfn)(void* elementp, const void* searchkeyp), 
	      const char *key, 
	      int32_t keylen);

