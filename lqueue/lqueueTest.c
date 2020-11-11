/* queueTest.c -- tests qopen, qclose, qput, and qget functions in queue.h interface
 *
 * Author: Eric Chen
 * Created: Tues Oct 6
 * Version:
 *
 * Description: returns 0 if success, nonzero if fail
 *
 */
 
#include <stdio.h>
#include "queue.h"
#include "lqueue.h"
#include <inttypes.h> 
#include <stdlib.h> //needed for malloc
#include <string.h>

 
 
#define MAXREG 10

/* the representation of a car */
typedef struct car {
	char plate[MAXREG];
	double price;
	int year;
} car_t;
 

/* makeCar(): creates a car object
 * returns 0 if successful; nonzero otherwise 
 */
car_t* make_car(char *platep, double price, int year) {
	
	car_t *cp;
	
	if ( !(cp = (car_t*)malloc(sizeof(car_t))) ) { 	//check if enough space in memory for car_t*
		printf("Error: malloc failed allocating car\n");
		return NULL;
	}
	strcpy(cp->plate, platep);
	cp->price = price;
	cp->year = year;
	return cp;
}


/* fn() takes a car_t pointer and sets year of car_t to 0
 * returns nothing
 */
void fn(void *p) {
	car_t *cp = (car_t*)p;
	cp->year = 0;
}


/* fn() checks if the car_t pointer matches with the provided key
 * returns bool
 */
bool searchfn(void* p, const void* keyp) { //requires (void* elementp, const void* keyp)
	car_t *cp = (car_t*)p;
	char *key = (char*)keyp;
	if ( strcmp(cp->plate, key) == 0 ) //if match
		return true;
	else
		return false;
}
	
	
	
	
 
int main (void) {

	pthread_t tid1,tid2; //to do multithreading

	//TEST QOPEN
	lqueue_t *lqp; //agnostic queue
	car_t *cp[4];
	car_t *c = NULL; //for lsearch returns

	int32_t result;
	int yr;
	
	lqp = lqopen(); //returns pointer to newly created queue_t

	cp[0] = make_car("car1", 1.0, 1910); //declare all cars
	cp[1] = make_car("car2", 4.0, 1940);
	cp[2] = make_car("car3", 9.0, 1990);
	
	lqapply((lqueue_t*)lqp, fn); //apply on empty lqueue
	lqsearch((lqueue_t*)lqp, searchfn, (void*)("randomString")); //should print Queue is empty

	
	result = 0;
	result += lqput(lqp, (void*)cp[0]);
	result += lqput(lqp, (void*)cp[1]);
	result += lqput(lqp, (void*)cp[2]);

	if ( result > 0 ) {
		fprintf(stderr, "Error while adding cars to queue\n");
		lqclose(lqp);
		exit(EXIT_FAILURE);
	}
	
	// TEST LQGET
	//cp[3] = lqget(lqp);
	//yr = cp[3]->year;
	//printf("year = %d\n", yr);
	//free(cp[3]);
	
	
	// TEST LQAPPLY
	
	lqapply((lqueue_t*)lqp, fn); //test with non-empty queue, should print "Applying function,,,"

	for (int i=1; i<3; i++ ) {
		if ( cp[i]->year != 0 ) {
			printf("year s %d\n", cp[i]->year);
			result++;
		}
	}

	if ( result > 0 ) {
		fprintf(stderr, "lqapply failed to implement on non-empty lqueue\n");
		lqclose(lqp);
		exit(EXIT_FAILURE);
	}
	else printf("lqapply successful\n");
	
	
	// test successful match cases
	c = lqsearch((lqueue_t*)lqp, searchfn, cp[0]->plate);
	if ( c != cp[0] ) result ++;
	c = lqsearch((lqueue_t*)lqp, searchfn, cp[1]->plate);
	if ( c != cp[1] ) result ++;
	c = lqsearch((lqueue_t*)lqp, searchfn, cp[2]->plate);
	if ( c != cp[2] ) result ++;
	
	//printf("result is %d\n", result);
	
	if ( result > 0 ) {
		fprintf(stderr, "qsearch failed when it should have matched\n");
		lqclose(lqp);
		exit(EXIT_FAILURE);
	}
	
	
	lqclose(lqp);
	
	
	/* TESTING MULTIPLE THREADS
	
	Main program does 2 pthread_create() - each executes a peice of code (which is a function)
	Helpful to have setDelay that we can set between the pthread_creates
	
	Thread 1
	- locks lqp, does lput, and hold lock for 10 sec before unlock (prints messaged when unlocking)
	Thread 2 
	- waits for 5 seconds, tries to do lput and lock the queue (print message when gets the lock)
	- because thread 1 is blocking lqp, thread 2 will not print message until thread 1 unlocks
	
	Note - short delay between unlock and printf to show on screen. Thus, we want the second process to wait
	for some period after it gets the lock before printing message that it has gotten the lock
	
	*/
	
	exit(EXIT_SUCCESS);
}







