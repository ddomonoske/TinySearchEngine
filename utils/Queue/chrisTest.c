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
void fn(car_t *cp) {
	cp->year = 0;
}
	
 
int main (void) {

	//TEST QOPEN
	queue_t *qp;
	car_t *cp1;
	car_t *cp2;
	car_t *cp3;
	car_t *cp4;
	int32_t result;
	int yr;
	
	qp = qopen(); //returns pointer to newly created queue_t object

	cp1 = make_car("1234", 5.0, 1969);
	cp2 = make_car("1000", 2.0, 1940);
	cp3 = make_car("5555", 9.0, 1931);
	
	if((result = qput(qp, (void*)cp1)) != 0){
		printf("qput failed\n");
		exit(EXIT_FAILURE);
	}
	
	if((result = qput(qp, (void*)cp2)) != 0){
		printf("qput failed\n");
		exit(EXIT_FAILURE);
	}
	
	if((result = qput(qp, (void*)cp3)) != 0){
		printf("qput failed\n");
		exit(EXIT_FAILURE);
	}
	
	cp4 = qget(qp);
	
	yr = cp4->year;
	printf("year = %d\n", yr);
	
	qclose(qp);
	
	exit(EXIT_SUCCESS);
}







