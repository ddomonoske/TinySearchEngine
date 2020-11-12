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
#include <pthread.h>
#include <unistd.h> //for sleep function
 
 
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
	


	
void *tfunc1(void *argp) { //function that calls LQPUT
	
	lqueue_t *lqp;
	car_t *cp1,*cp2;

	lqp = (lqueue_t*)argp;
	
	cp1 = make_car("car1", 1.0, 1910); //declare all cars
	//cp2 = make_car("car2", 2.0, 1920);
	
	lqput(lqp,(void*)cp1);
	//printf("thread 1 unlocking lqp\n");
	//lqput(lqp,(void*)cp2);

	return(lqp);
}


void *tfunc2(void *argp) { //function that calls LQPUT
	
	lqueue_t *lqp;
	car_t *cp3,*cp4;

	lqp = (lqueue_t*)argp;
	
	cp3 = make_car("car3", 3.0, 1930); //declare all cars
	//cp4 = make_car("car4", 4.0, 1940);
	
	sleep(5);	//second thread waits 5 seconds before attempting PUT

	lqput(lqp,(void*)cp3);
	//printf("thread 2 gets lock for lqp\n");
	//lqput(lqp,(void*)cp4);
	
	return(lqp);
}

 
int main (void) {

	pthread_t tid1,tid2; //to do multithreading
	
	lqueue_t *lqp; //ONE agnostic queue that both threads try to alter
	lqp = lqopen(); //returns pointer to newly created queue_t

	setQueueDelay(10);
	
	if(pthread_create(&tid1,NULL,tfunc1,lqp) != 0 ) {
		exit(EXIT_FAILURE);
	}

	if(pthread_create(&tid2,NULL,tfunc2,lqp) != 0 ) {
		exit(EXIT_FAILURE);
	}
	
	if(pthread_join(tid1,NULL) != 0 ) {
		exit(EXIT_FAILURE);
	}
	if(pthread_join(tid2,NULL) != 0 ) {
		exit(EXIT_FAILURE);
	}
	
	
	lqclose(lqp);
	
	
	/* TESTING MULTIPLE THREADS
	
	Main program does 2 pthread_create() - each executes a peice of code (which is a function)
	Helpful to have setDelay that we can set between the pthread_creates
	
	Thread 1
	//- locks lqp, does lput, and hold lock for 10 sec before unlock (prints messaged when unlocking)
	Thread 2 
	//- waits for 5 seconds, tries to do lput and lock the queue (print message when gets the lock)
	//- because thread 1 is blocking lqp, thread 2 will not print message until thread 1 unlocks
	
	Note - short delay between unlock and printf to show on screen. Thus, we want the second process to wait
	for some period after it gets the lock before printing message that it has gotten the lock
	
	*/
	
	exit(EXIT_SUCCESS);
}







