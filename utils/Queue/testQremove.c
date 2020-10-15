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
bool fn(void* p, const void* keyp) {
	car_t *cp = (car_t*)p;
	char *key = (char*)keyp;
	if ( strcmp(cp->plate, key) == 0 )
		return true;
	else
		return false;
}
	

int main (void) {
	queue_t *qp; //agnostic queue
	car_t *cp[5];
	car_t *c = NULL;
	car_t *cp1, *cp2, *cp3;
	int32_t result=0;
	
	qp = qopen(); //returns pointer to newly created queue_t

	// test with NULLs
	c = qremove(NULL, NULL, NULL);
	if (c != NULL) result++;
	qremove(NULL, fn, (void*)("randomString"));
	if (c != NULL) result++;
	qremove((queue_t*)qp, NULL, (void*)("randomString"));
	if (c != NULL) result++;
	qremove((queue_t*)qp, fn, NULL);
	if (c != NULL) result++;

	if ( result > 0 ) {
		fprintf(stderr, "ERROR while removing with NULL inputs\n");
		qclose(qp);
		exit(EXIT_FAILURE);
	}

	//declare all cars
	cp[0] = make_car("car1", 1.0, 1910);
	cp[1] = make_car("car2", 4.0, 1940);
	cp[2] = make_car("car3", 9.0, 1990);
	cp[3] = make_car("car4", 29.0, 1995);
	cp[4] = make_car("car5", 91.0, 1999);
	
	// test with empty queue
	c = qremove((queue_t*)qp, fn, (void*)("randomString"));
	if (c != NULL) {
		fprintf(stderr, "ERROR while removing from empty queue.\n");
		qclose(qp);
		exit(EXIT_FAILURE);
	}

	// add cars to queue
	result += qput(qp, (void*)cp[0]);
	result += qput(qp, (void*)cp[1]);
	result += qput(qp, (void*)cp[2]);
	
	if ( result > 0 ) {
		fprintf(stderr, "ERROR while adding cars to queue\n");
		qclose(qp);
		exit(EXIT_FAILURE);
	}

	// test successful match cases
	cp1 = qremove((queue_t*)qp, fn, cp[0]->plate);
	if ( cp1 != cp[0] ) result ++;
	cp2 = qremove((queue_t*)qp, fn, cp[2]->plate);
	if ( cp2 != cp[2] ) result ++;
	cp3 = qremove((queue_t*)qp, fn, cp[1]->plate);
	if ( cp3 != cp[1] ) result ++;
	
	if ( result > 0 ) {
		fprintf(stderr, "ERROR. qremove failed when it should have matched\n");
		qclose(qp);
		exit(EXIT_FAILURE);
	}
	
	result += qput(qp, (void*)cp[3]);
	result += qput(qp, (void*)cp[4]);
	
	c = qremove((queue_t*)qp, fn, "not_a_match");
	if ( c != NULL ) result ++;
	
	if ( result > 0 ) {
		fprintf(stderr, "ERROR. qremove failed when it should have not matched\n");
		qclose(qp);
		exit(EXIT_FAILURE);
	}
	
	free(cp1); //need to free pointers for elements that were removed from queue
	free(cp2);
	free(cp3);
	
	qclose(qp);
	
	exit(EXIT_SUCCESS);
}

