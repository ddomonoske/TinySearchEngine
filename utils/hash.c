/* 
 * hash.c -- implements a generic hash table as an indexed set of queues.
 *
 */
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "queue.h"
#include "hash.h"
#include <stdbool.h>

#define get16bits(d) (*((const uint16_t *) (d)))

static bool print_message = false; // control printing to screen

/* the hash table representation is hidden from users of the module */
typedef struct hheader {
	queue_t** table;    // an array of queue_t pointers (array of addresses that will eventually point to queue_t objects)
	uint32_t n;          // size of array
}hheader_t;


/* 
 * - prints message to screen with a prefix that includes file name
 * - only prints to screen when print_message is true
 */
static void print(char *message) {
	if (print_message) {
		fprintf(stderr,"~hash.c -> ");
		fprintf(stderr,"%s\n", message);
	}
}


/* 
 * SuperFastHash() -- produces a number between 0 and the tablesize-1.
 * 
 * The following (rather complicated) code, has been taken from Paul
 * Hsieh's website under the terms of the BSD license. It's a hash
 * function used all over the place nowadays, including Google Sparse
 * Hash.
 */
static uint32_t SuperFastHash (const char *data,int len,uint32_t tablesize) { //each hput needs same char *data and int len??
  uint32_t hash = len, tmp;
  int rem;
  
  if (len <= 0 || data == NULL)
		return 0;
  rem = len & 3;
  len >>= 2;
  /* Main loop */
  for (;len > 0; len--) {
    hash  += get16bits (data);
    tmp    = (get16bits (data+2) << 11) ^ hash;
    hash   = (hash << 16) ^ tmp;
    data  += 2*sizeof (uint16_t);
    hash  += hash >> 11;
  }
  /* Handle end cases */
  switch (rem) {
  case 3: hash += get16bits (data);
    hash ^= hash << 16;
    hash ^= data[sizeof (uint16_t)] << 18;
    hash += hash >> 11;
    break;
  case 2: hash += get16bits (data);
    hash ^= hash << 11;
    hash += hash >> 17;
    break;
  case 1: hash += *data;
    hash ^= hash << 10;
    hash += hash >> 1;
  }
  /* Force "avalanching" of final 127 bits */
  hash ^= hash << 3;
  hash += hash >> 5;
  hash ^= hash << 4;
  hash += hash >> 17;
  hash ^= hash << 25;
  hash += hash >> 6;
  return hash % tablesize;
}


/* hopen -- opens a hash table with initial size hsize */
hashtable_t *hopen(uint32_t hsize) {
	hheader_t *hp;
	uint32_t i;

	if (hsize<1) {
		print("hopen():\tError. Cannot make hash table with size < 1.");
		return NULL;
	}
	if ( (hp = (hheader_t*)malloc(sizeof(hheader_t))) == NULL ) //malloc memory for a hheader_t object
		return NULL;
	
	if ( (hp->table = (queue_t*)malloc(sizeof(queue_t*) * hsize)) == NULL ) { //malloc enough memory to hold hsize # of queue_t
		free(hp);
		return NULL;
	}
	hp->n = hsize;

	/* initialize each pointer in hash table with empty queue_t */
	for ( i=0; i<hp->n; i++ ) {
		hp->table[i] = qopen(); //table[i] is address of ith queue_t* pointer
	}

	print("hopen():\tTable successfully created and returned.");
	
	return (hashtable_t*)hp;
}


/* hclose -- closes a hash table */
void hclose(hashtable_t *htp) {
	hheader_t *hp;
	uint32_t i;
	
	if (htp != NULL) {
		hp = (hheader_t*)htp;
		
		for ( i=0; i<hp->n; i++ ) {  // for all queues in table
			qclose((hp->table)[i]);    // close each queue (hp->table[i] = address to queue_t object)
		}
		free(hp->table);
		free(hp);  //free header
		print("hclose():\tTable successfully closed.");
	} else {
		print("hclose():\tError. Provided table pointer is NULL.");
	}
}


/* hput -- puts an entry into a hash table under designated key 
 * returns 0 for success; non-zero otherwise
 */
int32_t hput(hashtable_t *htp, void *ep, const char *key, int keylen) {
	hheader_t *hp;
	uint32_t slot;

	// check that all pointers are defined
	if (htp == NULL || ep == NULL || key == NULL) {
		print("hput():\tError. One or more inputs are NULL.");
		return 1;
	}
	if (keylen < 1) {
		print("hput():\tError. Key must have length > 0.");
		return 1;
	}

	hp = (hheader_t*)htp; //create hash table

	slot = SuperFastHash(key, keylen, hp->n);  //use hash function (key from element,length of key from element,hash table size)

	// add element to corresponding queue
	if ( qput((hp->table)[slot], ep) != 0) {
		print("hput():\tError. Failed to add data to queue in hash table.");
		return 1;
	}
	print("hput():\tSuccessfully placed item in table.");
	return 0;
}


/* happly -- applies a function to every entry in hash table */
void happly(hashtable_t *htp, void (*fn)(void* ep)) {
	hheader_t *hp;
	uint32_t i;

	if ( htp != NULL && fn != NULL ) {
		hp = (hheader_t*)htp;

		for ( i=0; i<hp->n; i++ ) {     // for each queue in table
			qapply((hp->table)[i], fn);   // apply function to every element in queue
		}
		print("happly():\tFunction successfully applied to all elements.");
	} else {
		print("happly():\tError. One or more inputs are NULL.");
	}
}


/* hsearch -- searchs for an entry under a designated key using a
 * designated search fn -- returns a pointer to the entry or NULL if
 * not found
 */
void *hsearch(hashtable_t *htp, 
							bool (*searchfn)(void* elementp, const void* searchkeyp), 
							const char *key, 
							int32_t keylen) {
	hheader_t *hp;
	uint32_t slot;

	if ( htp == NULL || searchfn == NULL || key == NULL) {
		print("hsearcj():\tError. One or more inputs are NULL.");
		return NULL;
	}
	if ( keylen < 1 ) {
		print("hsearch():\tError. Key must have length > 0.");
		return NULL;
	}
	
	hp = (hheader_t*)htp;
	slot = SuperFastHash(key, keylen, hp->n);  // get slot based on key -> determines which queue to look into
	
	print("hsearch():\tReturning result of hsearch().");
	void* nodeMatch = qsearch((hp->table)[slot], searchfn, key); //qsearch returns pointer to matching element in queue
	return nodeMatch; 

}


/* hremove -- removes and returns an entry under a designated key
 * using a designated search fn -- returns a pointer to the entry or
 * NULL if not found
 */
void *hremove(hashtable_t *htp, 
							bool (*searchfn)(void* elementp, const void* searchkeyp), 
							const char *key, 
							int32_t keylen) {
	hheader_t *hp;
	uint32_t slot;

	if ( htp == NULL || searchfn == NULL || key == NULL ) {
		print("hremove():\tError. One or more inputs are NULL.");
		return NULL;
	}
	if ( keylen < 1 ) {
		print("hremove():\tError. Key must have length > 0.");
		return NULL;
	}
	
	hp = (hheader_t*)htp;
	slot = SuperFastHash(key, keylen, hp->n);  // get slot based on key

	print("hremove():\tReturning result of hremove().");
	void *nodeMatch = qremove((hp->table)[slot], searchfn, key);  // qremove removes and returns element
	return nodeMatch;
}

