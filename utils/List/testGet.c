/* testGet.c -- tests get() function in list.h interface
 *
 * Author: Chris Lyke
 * Created: 
 * Version:
 *
 * Description: returns 0 if success, nonzero if error
 *
 */
 
#include <stdio.h>
#include "list.h"
#include <inttypes.h> 
#include <stdlib.h> //needed for malloc
#include <string.h>


/* carcmp takes the pointers to two different cars
 * returns 0 if the values are the same
 * returns 1 otherwise
 */
int32_t carcmp(car_t *cp1, car_t *cp2) {
	if (cp1->price != cp2->price) return 1;
	if (cp1->year != cp2->year) return 1;
	if (strcmp(cp1->plate,cp2->plate)) return 1;

	return 0;
}
 

/* makeCar(): creates a car object
 * returns 0 if successful; nonzero otherwise 
 */
car_t* make_car(car_t *next, char *platep, double price, int year) {
	
	car_t *cp;
	
	if ( !(cp = (car_t*)malloc(sizeof(car_t))) ) { 	//check if enough space in memory for car_t*
		printf("Error: malloc failed allocating car\n");
		return NULL;
	}
	
	cp->next = next;
	strcpy(cp->plate, platep);
	cp->price = price;
	cp->year = year;
	
	return cp;
}


int main (void) {

	
	car_t *cp1 = make_car(NULL,"2LLE50",20000.0,2014);
	car_t *cp2 = make_car(NULL,"NNHS16",30000.0,2016);


	//TEST LGET() FUNCTION
	car_t *cpNull = lget(); //test get function from empty list
	if (cpNull != NULL){
		fprintf(stderr, "Failed while getting from empty list\n");
		return 1;
	}
	
	lput(cp1); //adds cp1 to front of list
	lput(cp2); ///adds cp2 to front of list
	
	car_t *first = lget(); //removes and returns pointer to first car in list
	if (carcmp(first,cp2)) {
		fprintf(stderr, "Failed while getting from not-empty list\n");
		return 1;
	}
	
	free(cp1);
	free(cp2);
	free(cpNull);
	
	printf("lget success\n");
	return 0;
}

