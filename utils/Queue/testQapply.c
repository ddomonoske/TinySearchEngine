/* testQapply.c -- test qapply() in queue.c
 *
 * Author: David Domonoske
 * Created: Sun Oct 11
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
	if ((cp = (car_t*)malloc(sizeof(car_t))) == NULL) { //check if enough space in memory for car_t*
		printf("Error: malloc failed allocating car\n");
		return NULL;
	}
	printf("car malloc succesful\n");
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
	

int main (void) {
	queue_t *qp; //agnostic queue
	car_t *cp[3];
	int32_t result, i;
	
	qp = qopen(); //returns pointer to newly created queue_t

	cp[0] = make_car("car1", 1.0, 1910); //declare all cars
	cp[1] = make_car("car2", 4.0, 1940);
	cp[2] = make_car("car3", 9.0, 1990);

	// test with NULLs
	qapply(NULL, NULL);
	qapply(NULL, fn);
	qapply((queue_t*)qp, NULL);

	// test with empty queue, should print queue is empty
	qapply((queue_t*)qp, fn); //why need to recast qp?
	
	result = 0;
	result += qput(qp, (void*)cp[0]);
	result += qput(qp, (void*)cp[1]);
	result += qput(qp, (void*)cp[2]);

	if ( result > 0 ) {
		fprintf(stderr, "Error while adding cars to queue\n");
		qclose(qp);
		exit(EXIT_FAILURE);
	}

	qapply((queue_t*)qp, fn); //test with non-empty queue, should print "Applying function,,,"

	for ( i=0; i<3; i++ ) {
		if ( cp[i]->year != 0 )
			result++;
	}

	if ( result > 0 ) {
		fprintf(stderr, "qapply failed to implement on non-empty queue\n");
		qclose(qp);
		exit(EXIT_FAILURE);
	}
	else printf("qapply successful");
	
	qclose(qp);
	
	exit(EXIT_SUCCESS);
}

