/* testRemove.c --- tests remove() function in list.h interface
 * 
 * 
 * Author: David W. Domonoske
 * Created: Mon Oct  5 09:47:49 2020 (-0400)
 * Version: 
 * 
 * Description: returns 0 if success, nonzero if error
 *
 *              tests the following scenarios:
 *               - removing from an empty list
 *               - removing a plate that doesn't exist
 *               - removing from a non-empty list in the middle
 *               - removing from a non-empty list at the end
 *               - removing from a non-empty list at the beginning
 *               - removing the last element of a list 
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

int main(void) {

	car_t *cp1 = make_car(NULL,"2LLE50",20000.0,2014);
	car_t *cp2 = make_car(NULL,"NNHS16",30000.0,2016);
	car_t *cp3 = make_car(NULL,"H453SM",15000.0,2011);
	car_t *cp4 = make_car(NULL,"ABCDEF", 1000.0,2020);
	car_t *cp0 = NULL;
	
	int32_t err_ct = 0;
	
	// TEST lremove() on an empty list
	cp0 = lremove(NULL);
	if (cp0 != NULL) err_ct++;

	cp0 = lremove("AAAAAA");
	if (cp0 != NULL) err_ct++;
	
	if (err_ct != 0) {
		fprintf(stderr, "Failed while removing from empty list\n");
		return err_ct;
	}

	// add cars to list
	lput(cp1);
	lput(cp2);
	lput(cp3);
	lput(cp4);

	// TEST lremove with non-existing plate
	cp0 = lremove("AAAAAA");
	if (cp0 != NULL) {
		printf("%s\n",cp0->plate);
		fprintf(stderr, "Failed while removing non-existing plate.\n");
		err_ct++;
		return err_ct;
	}

	// TEST lremove in the middle of the list
	cp0 = lremove(cp2->plate);
	if (carcmp(cp0,cp2)) {
		fprintf(stderr, "Failed while removing from middle of list.\n");
		return 1;
	}
	
	// TEST lremove at end of list
	cp0 = lremove(cp1->plate);
	if (carcmp(cp0,cp1)) {
		fprintf(stderr, "Failed while removing from end of list.\n");
		return 1;
	}
	
	// TEST lremove at beginning of list
	cp0 = lremove(cp4->plate);
	if (carcmp(cp0,cp4)) {
		fprintf(stderr, "Failed while removing from beginning of list.\n");
		return 1;
	}

	// TEST lremove with last element of list
	cp0 = lremove(cp3->plate);
	if (carcmp(cp0,cp3)) {
		fprintf(stderr, "Failed while removing last element from list.\n");
		return 1;
	}
	
	free(cp1);
	free(cp2);
	free(cp3);
	free(cp4);

	return 0;
}
