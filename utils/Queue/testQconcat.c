/* testQremove.c -- test qremove() in queue.c
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
	queue_t *qp1, *qp2, *qp_mem; //agnostic queue
	car_t *cp[4];
	int32_t result=0, i;
	
	qp1 = qopen(); //returns pointer to newly created queue_t
	qp2 = qopen();
	
 	if ( result > 0 ) {
		fprintf(stderr, "Error while searching with NULL inputs\n");
		qclose(qp1);
		qclose(qp2);
		exit(EXIT_FAILURE);
	}

	// test with NULL inputs
	qconcat(NULL, NULL);
	qconcat(qp1, NULL);
	qconcat(NULL, qp2);

	if (qp1 == NULL || qp2 == NULL ) {
		printf("Error with NULL inputs\n");
		qclose(qp1);
		qclose(qp2);
		exit(EXIT_FAILURE);
	}
	
	// test with empty queues
	qp_mem = qp1;
	qconcat(qp1, qp2);
	if (qp1 != qp_mem) {
		printf("Error with empty queues\n");
		qclose(qp1);
		qclose(qp2);
		exit(EXIT_FAILURE);
	}

	qp2 = qopen();
	
	//declare all cars
	cp[0] = make_car("car1", 1.0, 1910);
	cp[1] = make_car("car2", 4.0, 1940);
	cp[2] = make_car("car3", 9.0, 1990);
	cp[3] = make_car("car4", 2.0, 2000);
	
	// place 2 cars in each queue
	result += qput(qp1, (void*)cp[0]);
	result += qput(qp1, (void*)cp[1]);
	result += qput(qp2, (void*)cp[2]);
	result += qput(qp2, (void*)cp[3]);
	
	if ( result > 0 ) {
		fprintf(stderr, "Error while adding cars to queue\n");
		qclose(qp1);
		qclose(qp2);
		exit(EXIT_FAILURE);
	}

	// test under normal cases
	qp_mem = qp1;
	qconcat(qp1, qp2);
	if ( qp1 != qp_mem ) result++;
	for ( i=0; i<4; i++ ) {
		if ( qsearch(qp1, fn, cp[i]->plate) != cp[i] )
			result++;
	}
	if ( result > 0 ) {
		printf("Error concatenating non-empty queues\n");
		qclose(qp1);
		qclose(qp2);
		exit(EXIT_FAILURE);
	}
	qp2 = qopen();
	
	// add empty list to non-empty  list
	qp_mem = qp1;
	qconcat(qp1, qp2);
	if ( qp1 != qp_mem ) result++;
	for ( i=0; i<4; i++ ) {
		if ( qsearch(qp1, fn, cp[i]->plate) != cp[i] )
			result++;
	}
	if ( result > 0 ) {
		printf("Error concatenating empty to non-empty queues\n");
		qclose(qp1);
		qclose(qp2);
		exit(EXIT_FAILURE);
	}
	qp2 = qopen();

	// add non-empty list to empty list
	qp_mem = qp2;
	qconcat(qp2, qp1);
	if ( qp2 != qp_mem ) result++;
	for ( i=0; i<4; i++ ) {
		if ( qsearch(qp2, fn, cp[i]->plate) != cp[i] )
			result++;
	}
	if ( result > 0 ) {
		printf("Error concatenating non-empty to empty queues\n");
		qclose(qp1);
		qclose(qp2);
		exit(EXIT_FAILURE);
	}
	
	qclose(qp1);
	
	exit(EXIT_SUCCESS);
}

