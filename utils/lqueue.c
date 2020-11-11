/* 
 * queue.c -- implementation to the locked queue module
 */

#include "queue.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "lqueue.h"
#include <unistd.h> //for sleep function


static bool print_message = false; // control printing to screen

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


//set value internal to module that sets time to wait between lock and unlock functions
//will sleep for that amount of time before unlocking
void setDelay(int time) {
   
   printf("Sleeping %d seconds\n",time);
   sleep(time);
}


/* defines each node of a queue */
typedef struct lqueue{
	queue_t *qp;	//queue
	pthread_mutex_t m;  //associated mutex lock
} lqheader_t;



/* create an empty locked queue */
lqueue_t* lqopen(void) {

	lqheader_t *lqp;
	if ( (lqp = (lqheader_t*)malloc(sizeof(lqheader_t))) == NULL ) //allocate space for lqueue_t object
		return NULL;	
	lqp->qp = qopen(); 
	pthread_mutex_init(&(lqp->m),NULL); //initialize mutex
	print("New locked queue created\n");

	return (lqueue_t*)lqp;
	
}      

/* deallocate a locked queue, frees everything in it */
void lqclose(lqueue_t *lqp) {

	lqheader_t *hp;
	
	if (lqp == NULL) {
		print("Error: locked queue is NULL\n");
	}
	else {
		hp = (lqheader_t*)lqp;
		qclose(hp->qp);
		pthread_mutex_destroy(&(hp->m));/* destroy the mutex */
		free(hp);
		print("Locked queue closed\n");
	}
} 

/* put element at the end of the lqueue
 * returns 0 is successful; nonzero otherwise 
 */
int32_t lqput(lqueue_t *lqp, void *elementp) {

	lqheader_t *hp;	
	int32_t status;
	
	if (lqp == NULL) {	
		print("Error: locked queue is NULL\n");
		return 1;
	}
	
	if (elementp == NULL) {	
		print("Error: object passed in is NULL\n");
		return 1;
	}

	hp = (lqheader_t*)lqp; //case lqp into local object type

	pthread_mutex_lock(&(hp->m));
	printf("lqp locked\n");
	
	status = qput(hp->qp,elementp); //puts elementp into lqp
	
	setDelay(10);	//hold lock for 10 second
	pthread_mutex_unlock(&(hp->m));
	printf("lqp unlocked\n");
	
	return(status);
}



/* get the first first element from lqueue, removing it from the lqueue */
void* lqget(lqueue_t *lqp) {

	lqheader_t *hp;	
	void *tmp;
	
	if (lqp == NULL) {	
		print("Error: queue is NULL\n");
		return NULL;
	}
	
	hp = (lqheader_t*)lqp;
	
	pthread_mutex_lock(&(hp->m));
	tmp = qget(hp->qp);
	pthread_mutex_unlock(&(hp->m));

	return (tmp); //returns element from lqueue
}


/* apply a function to every element of the locked queue */
void lqapply(lqueue_t *lqp, void (*fn)(void* elementp)) {

	lqheader_t *hp;

	if (lqp == NULL || fn == NULL ) {	//checks inputs
		print("lqp or fn is NULL\n");
	} 
	else {
		hp = (lqheader_t*)lqp;
		
		pthread_mutex_lock(&(hp->m));
		qapply(hp->qp,fn);	//qapply to each queue
		pthread_mutex_unlock(&(hp->m));
	}

}

/* search a queue using a supplied boolean function
 * skeyp -- a key to search for
 * searchfn -- a function applied to every element of the queue
 *          -- elementp - a pointer to an element
 *          -- keyp - the key being searched for (i.e. will be 
 *             set to skey at each step of the search
 *          -- returns TRUE or FALSE as defined in bool.h
 * returns a pointer to an element, or NULL if not found
 */
void* lqsearch(lqueue_t *lqp, 
							bool (*searchfn)(void* elementp,const void* keyp),
							const void* skeyp) {
							
	lqheader_t *hp;
	void *tmp;
	
	if (lqp == NULL) {	//check inputs
		print("Locked queue is NULL\n");
		return NULL;
	}
	
	hp = (lqheader_t*)lqp;
	
	pthread_mutex_lock(&(hp->m));
	tmp = qsearch(hp->qp,searchfn,skeyp);	//search locked queue
	pthread_mutex_unlock(&(hp->m));
	
	return(tmp);	
}

// NOT MENTIONED IN MODULE STEP
/* search a queue using a supplied boolean function (as in qsearch),
 * removes the element from the queue and returns a pointer to it or
 * NULL if not found
 */
void* lqremove(lqueue_t *lqp,
							bool (*searchfn)(void* elementp,const void* keyp),
							const void* skeyp);

/* concatenatenates elements of q2 into q1
 * q2 is dealocated, closed, and unusable upon completion 
 */
void lqconcat(lqueue_t *lq1p, lqueue_t *lq2p) {

	lqheader_t *hp1;
	lqheader_t *hp2;
	
	if ((lq1p == NULL) && (lq2p == NULL)) {	
		print("One of the locked queues are NULL\n");
	} 
	else {		
		hp1 = (lqheader_t*)lq1p;
		hp2 = (lqheader_t*)lq2p;
	
		pthread_mutex_lock(&(hp1->m));	
		pthread_mutex_lock(&(hp2->m));	
		qconcat(hp1->qp,hp2->qp);
		pthread_mutex_unlock(&(hp1->m));
		pthread_mutex_unlock(&(hp2->m));
	}
	
}






