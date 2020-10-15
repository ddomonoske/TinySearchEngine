/* testApply.c -- tests apply() function in list.h interface
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



/* fn() takes a car_t pointer and sets year of car_t to 0
 * returns nothing
 */
void fn(car_t *cp) {
	cp->year = 0;
}
	

 
int main (void) {

	//TEST LAPPLY() FUNCTION
	lapply(fn); //applies function fn to empty list
	
	car_t *cp1 = make_car(NULL,"2LLE50",20000.0,2014);
	car_t *cp2 = make_car(NULL,"NNHS16",30000.0,2016);
	car_t *cp3 = make_car(NULL,"H453SM",15000.0,2011);
	
	lput(cp1); //adds cp1 to front of list
	lput(cp2); //adds cp2 to front of list
	lput(cp3); //adds cp3 to front of list
	
	lapply(fn); //applies function fn to every car in the list

	car_t *p; //create pointer to start looping through car_t list
	for (p=cp3; p!=NULL; p=p->next) { //loops through list to make sure year is 0 for each car
		if(p->year != 0){
			fprintf(stderr, "Failed while applying function to non-empty list\n");
			return 1;	
		}
	}
	
	free(cp1);
	free(cp2);
	free(cp3);
	free(p);

	printf("lapply success\n");
	return 0;
}

