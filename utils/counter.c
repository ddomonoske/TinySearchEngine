/* counter.c --- implementation of counter.h
 * 
 * 
 * Author: David W. Domonoske
 * Created: Wed Nov 25 18:38:13 2020 (-0500)
 * Version: 1.0
 * 
 * Description: functions on counter_t object
 */

#include "counter.h"
#include <stdlib.h>
#include <stdio.h>

/* the counter representation is hidden from users of the module */
typedef struct cheader {
	int32_t count;
} cheader_t;


/* create a new counter */
counter_t* openCount(void){
	cheader_t *chp = (cheader_t*)malloc(sizeof(cheader_t));
	chp->count = 0;
	return (counter_t*)chp;
}


/* deallocate a counter */
void closeCount(counter_t *cp) {
	cheader_t *chp = (cheader_t*)cp;
	free(chp);
	return;
}


/* get current count */
int32_t getCount(counter_t *cp) {
	cheader_t *chp = (cheader_t*)cp;
	return chp->count;
}


/* increase count by 1 */
int32_t incCount(counter_t *cp) {
	cheader_t *chp = (cheader_t*)cp;
	(chp->count)++;
	return chp->count;
}


/* decrease count by 1 */
int32_t decCount(counter_t *cp) {
	cheader_t *chp = (cheader_t*)cp;
	(chp->count)--;
	return chp->count;
}

/* set count to x */
int32_t setCount(counter_t *cp, int32_t x) {
	cheader_t *chp = (cheader_t*)cp;
	chp->count = x;
	return chp->count;
}

/* reset count to 0 */
int32_t resetCount(counter_t *cp) {
	cheader_t *chp = (cheader_t*)cp;
	chp->count = 0;
	return chp->count;
}

/* add n to current count */
int32_t addCount(counter_t *cp, int32_t x) {
	cheader_t *chp = (cheader_t*)cp;
	chp->count += x;
	return chp->count;
}
