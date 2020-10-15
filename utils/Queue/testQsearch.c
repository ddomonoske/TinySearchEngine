/* testQsearch.c -- test qsearch() in queue.c
 *
 * Author: David Domonoske
 * Created: Mon Oct 12, 2020
 * Version:
 *
 * Description: returns 0 if success, nonzero if fail
 *
 */
 
#include <stdio.h>
#include "queue.h"
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAXREG 10


/* the representation of a car */
typedef struct car {
	char plate[MAXREG];
	double price;
	int year;
} car_t;


/* makeCar(): creates a car object */
car_t* make_car(char *platep, double price, int year) {
	car_t *cp;
	if ((cp = (car_t*)malloc(sizeof(car_t))) == NULL) { 	//check if enough space in memory for car_t*
		printf("Error: malloc failed allocating car\n");
		return NULL;
	}
	printf("car malloc succesful\n");
	strcpy(cp->plate, platep);
	cp->price = price;
	cp->year = year;
	return cp;
}


/* fn() checks if the car_t pointer matches with the provided key
 * returns bool
 */
bool fn(void* p, const void* keyp) { //requires (void* elementp, const void* keyp)
	car_t *cp = (car_t*)p;
	char *key = (char*)keyp;
	if ( strcmp(cp->plate, key) == 0 ) //if match
		return true;
	else
		return false;
}
	

int main (void) {
	queue_t *qp; //agnostic queue
	car_t *cp[3];
	car_t *c = NULL;
	int32_t result=0;
	
	qp = qopen(); //returns pointer to newly created queue_t

	// test with NULLs
	c = qsearch(NULL, NULL, NULL);
	if (c != NULL) result++;
	c = qsearch(NULL, fn, (void*)("randomString"));
	if (c != NULL) result++;
	c = qsearch((queue_t*)qp, NULL, (void*)("randomString"));
	if (c != NULL) result++;
	c = qsearch((queue_t*)qp, fn, NULL);
	if (c != NULL) result++;

	if ( result > 0 ) {
		fprintf(stderr, "Error while searching with NULL input\n");
		qclose(qp);
		exit(EXIT_FAILURE); //no need to reset result bc will exit
	}

	//declare all cars
	cp[0] = make_car("car1", 1.0, 1910);
	cp[1] = make_car("car2", 4.0, 1940);
	cp[2] = make_car("car3", 9.0, 1990);
	
	// test with empty queue
	qsearch((queue_t*)qp, fn, (void*)("randomString")); //should print Queue is empty
	
	result += qput(qp, (void*)cp[0]);
	result += qput(qp, (void*)cp[1]);
	result += qput(qp, (void*)cp[2]);

	if ( result > 0 ) {
		fprintf(stderr, "Error while adding cars to queue\n");
		qclose(qp);
		exit(EXIT_FAILURE);
	}

	// test successful match cases
	c = qsearch((queue_t*)qp, fn, cp[0]->plate);
	if ( c != cp[0] ) result ++;
	c = qsearch((queue_t*)qp, fn, cp[1]->plate);
	if ( c != cp[1] ) result ++;
	c = qsearch((queue_t*)qp, fn, cp[2]->plate);
	if ( c != cp[2] ) result ++;
	
	//printf("result is %d\n", result);
	
	if ( result > 0 ) {
		fprintf(stderr, "qsearch failed when it should have matched\n");
		qclose(qp);
		exit(EXIT_FAILURE);
	}

	c = qsearch((queue_t*)qp, fn, "not_a_match"); //test for search key with no match, should return NULL
	if ( c != NULL ) result ++;
	
	if ( result > 0 ) {
		fprintf(stderr, "qsearch failed when it should have not matched\n");
		qclose(qp);
		exit(EXIT_FAILURE);
	}
	
	qclose(qp);
	
	exit(EXIT_SUCCESS);
}

