/* lcounter.c --- implementation of locked counter module
 * 
 * 
 * Author: David W. Domonoske
 * Created: Wed Nov 25 18:38:13 2020 (-0500)
 * Version: 1.0
 * 
 * Description: functions described in lcounter.h
 */

#include "lcounter.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

static bool isPrinting = false;

/* locked counter consists of a counter and a thread mutex */
typedef struct lcheader {
	counter_t *cp;
	pthread_mutex_t m;
} lcheader_t;


static void print(char *message) {
	if (isPrinting) {
		fprintf(stderr,"~lcounter.c -> ");
		fprintf(stderr,"%s", message);
	}
}

/* lock associated mutex for counter
 *  - makes frequent calls a little easier
 */
static void lockCounter(lcheader_t *lcp) {
	pthread_mutex_lock(&(lcp->m));
	print("lcp is locked\n");
}


/* unlock associated mutex for counter
 *  - makes frequent calls a little easier
 */
static void unlockCounter(lcheader_t *lcp) {
	pthread_mutex_unlock(&(lcp->m));
	print("lcp is unlocked\n");
}


/* create a new counter */
lcounter_t* openLCount(void) {
	lcheader_t *lcp = (lcheader_t*)malloc(sizeof(lcheader_t));

	lcp->cp = openCount();
	pthread_mutex_init(&(lcp->m),NULL);
	print("New locked counter created\n");

	return (lcounter_t*)lcp;
}


/* deallocate a counter */
void closeLCount(lcounter_t *cp) {
	lcheader_t *lcp;

	lcp = (lcheader_t*)cp;
	lockCounter(lcp);
	closeCount(lcp->cp);
	unlockCounter(lcp);
	
	pthread_mutex_destroy(&(lcp->m));
	free(lcp);
	print("Locked counter closed\n");
	return;
}


/* get current count */
int32_t getLCount(lcounter_t *cp) {
	lcheader_t *lcp;
	int32_t count;
	
	lcp = (lcheader_t*)cp;
	lockCounter(lcp);

	count = getCount(lcp->cp);
	unlockCounter(lcp);

	return count;
}


/* increase count by 1 */
int32_t incLCount(lcounter_t *cp) {
	lcheader_t *lcp;
	int32_t count;
	
	lcp = (lcheader_t*)cp;
	lockCounter(lcp);
	count = incCount(lcp->cp);
	unlockCounter(lcp);

	return count;
}


/* decrease count by 1 */
int32_t decLCount(lcounter_t *cp) {
	lcheader_t *lcp;
	int32_t count;
	
	lcp = (lcheader_t*)cp;
	lockCounter(lcp);
	count = decCount(lcp->cp);
	unlockCounter(lcp);

	return count;
}


/* set count to x */
int32_t setLCount(lcounter_t *cp, int32_t x) {
	lcheader_t *lcp;
	int32_t count;
	
	lcp = (lcheader_t*)cp;
	lockCounter(lcp);
	count = setCount(lcp->cp, x);
	unlockCounter(lcp);

	return count;
}


/* reset count to 0 */
int32_t resetLCount(lcounter_t *cp) {
	lcheader_t *lcp;
	int32_t count;
	
	lcp = (lcheader_t*)cp;
	lockCounter(lcp);
	count = resetCount(lcp->cp);
	unlockCounter(lcp);

	return count;
}


/* add n to current count */
int32_t addLCount(lcounter_t *cp, int32_t x) {
	lcheader_t *lcp;
	int32_t count;
	
	lcp = (lcheader_t*)cp;
	lockCounter(lcp);
	count = addCount(lcp->cp, x);
	unlockCounter(lcp);

	return count;
}
