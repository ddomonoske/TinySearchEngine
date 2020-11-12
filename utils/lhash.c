
/*
 * lhash.c -- Implementation to the locked hash table module
 *  - refer to lhash.h for documentation
 *
 * Author: David Domonoske
 *
 */
#include "lhash.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


static bool print_message = false; // control printing to screen
static int delay_time = 0; // control how long all functions that lock wait before unlocking


/* defines each locked hash table */
typedef struct lhheader{
	hashtable_t *hp; // hash table
	pthread_mutex_t m; // associated mutex lock
} lhheader_t;


/*
 * - prints message to screen with a prefix that includes file name
 * - only prints to screen when print_message is true
 */
static void print(char *message) {
	if (print_message) {
		fprintf(stderr,"~lhash.c -> ");
		fprintf(stderr,"%s\n", message);
	}
}


/*
 * lock associated mutex for hash table
 *  - includes taggled print message
 *  - makes frequen calls a little easier
 */
static void lockTable(lhheader_t *lhp) {
	pthread_mutex_lock(&(lhp->m));
	print("lhp is locked\n");
}


/*
 * unlock associated mutex for hash table
 *  - NOTE: this includes the call to sleep(delay_time)
 *  - includes taggled print message
 *  - makes frequen calls a little easier
 */
static void unlockTable(lhheader_t *lhp) {
	sleep(delay_time);
	pthread_mutex_unlock(&(lhp->m));
	print("lhp is unlocked\n");
}


//set value internal to module that sets time to wait between lock and unlock functions
//will sleep for that amount of time before unlocking
void setHashDelay(uint32_t time) {
	
	if (time >= 0) {
		printf("Setting delay time to %d seconds\n",time);
		delay_time = time;
	} else {
		printf("Delay time must be > 0\n");
	}
}


/* lhopen -- create an empty locked hash table */
lhashtable_t *lhopen(uint32_t hsize) {

	lhheader_t *lhp;
	if ( (lhp = (lhheader_t*)malloc(sizeof(lhheader_t))) == NULL )
		return NULL;

	lhp->hp = hopen(hsize);
	pthread_mutex_init(&(lhp->m),NULL);
	print("New locked hash created\n");

	return (lhashtable_t*)lhp;
}


/* hclose -- closes a locked hash table */
void lhclose(lhashtable_t *htp) {

	lhheader_t *lhp;

	if (htp == NULL) {
		print("Error: locked hash table is NULL\n");
	} else {
		lhp = (lhheader_t*)htp;

		// lock, close hash table, and unlock
	  lockTable(lhp);
		hclose(lhp->hp);
		unlockTable(lhp);

		free(lhp);
		lhp = NULL;
		print("Locked hash table closed\n");
	}
}


/* lhput -- puts an entry into a hash table under designated key 
 *  - returns 0 for success; non-zero otherwise
 */
int32_t lhput(lhashtable_t *htp, void *ep, const char *key, int keylen) {

	lhheader_t *lhp;
	int32_t status;

	if (htp == NULL) {
		print("Error: locked queue is NULL\n");
		return 1;
	}

	lhp = (lhheader_t*)htp;

	// lock, put element in table, and unlock
	lockTable(lhp);
	status = hput(lhp->hp, ep, key, keylen);
	unlockTable(lhp);

	return(status);
}


/* lhapply -- applies a function to every entry in locked hash table */
void lhapply(lhashtable_t *htp, void (*fn)(void* ep)) {

	lhheader_t *lhp;

	if ((htp == NULL) || (fn == NULL)) {
		print("lhp or fn is NULL\n");
		return;
	}

	lhp = (lhheader_t*)htp;

	// lock, apply fn to table, and unlock
	lockTable(lhp);
	happly(lhp->hp, fn);
	unlockTable(lhp);
}


/* hsearch -- searchs for an entry under a designated key using a
 * designated search fn -- retcurns a pointer to the entry or NULL if
 * not found
 */
void *lhsearch(lhashtable_t *htp, 
							 bool (*searchfn)(void* elementp, const void* searchkeyp), 
							 const char *key, 
							 int32_t keylen) {

	lhheader_t *lhp;
	void *tmp = NULL;

	if (htp == NULL) {
		print("Locked hash table is NULL\n");
		return NULL;
	}

	lhp = (lhheader_t*)htp;

	// lock, search table for key, and unlock
	lockTable(lhp);
	tmp = hsearch(lhp->hp, searchfn, key, keylen);
	unlockTable(lhp);

	return(tmp);
}


/* hremove -- removes and returns an entry under a designated key
 * using a designated search fn -- returns a pointer to the entry or
 * NULL if not found
 */
void *lhremove(lhashtable_t *htp, 
							 bool (*searchfn)(void* elementp, const void* searchkeyp), 
							 const char *key,
							 int32_t keylen) {

	lhheader_t *lhp;
	void *tmp = NULL;
	
	if (htp == NULL) {
		print("Locked hash table is null\n");
		return NULL;
	}

	lhp = (lhheader_t*)htp;

	// lock, remove key from table, and unlock
	lockTable(lhp);
	tmp = hremove(lhp->hp, searchfn, key, keylen);
	unlockTable(lhp);

	return(tmp);
}

