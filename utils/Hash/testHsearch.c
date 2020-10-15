/* testHsearch.c -- test hsearch() in hash.c
 *
 * Author: Eric Chen
 * Created: Mon Oct 12, 2020
 * Version:
 *
 * Description: returns 0 if success, nonzero if fail
 *
 */
 
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "queue.h"
#include "hash.h"

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
	
	int32_t result = 0;
	uint32_t hsize = 6;
	
	// assuming hopen and hclose are good
	hashtable_t *hp; //agnostic hashtable, can hold queues with cars and students
	hp = hopen(hsize); //returns pointer to newly created hashtable_t
	
	car_t *cp[3];
	car_t *c = NULL;
	
	// test with NULLS and keylen < 1
	c = hsearch(NULL, NULL, NULL, 10); //HSEARCH requires (*htp, *searchfn, char *key, keylen)
	if (c != NULL) result++;
	
	c = hsearch(NULL, fn, (void*)("randomString"),10); //*htp NULL
	if (c != NULL) result++;
	
	c = hsearch(hp, NULL, (void*)("randomString"),10); //*searchfn NULL
	if (c != NULL) result++;
	
	c = hsearch(hp, fn, NULL, 10); //char *key NULL
	if (c != NULL) result++;
	
	c = hsearch(hp, fn, (void*)("randomString"),0); //keylen < 1
	if (c != NULL) result++;
	
	if ( result > 0 ) {
		fprintf(stderr, "Error while searching with NULL input / keylen>1\n");
		hclose(hp);
		exit(EXIT_FAILURE); //no need to reset result bc will exit
	}

	//declare all cars
	cp[0] = make_car("car1", 1.0, 1910);
	cp[1] = make_car("car2", 4.0, 1940);
	cp[2] = make_car("car3", 9.0, 1990);
	
	// test with empty hashtable
	if ((hsearch(hp, fn, (void*)("randomString"),10)) != NULL) { //hsearch calls qsearch, whicih should print Queue is empty
		printf("Error while searching empty hashtable");
	}
	else printf(">>searching empty hashtable occurred as expected\n");
	
	// assuming hput is good
	hput(hp, (void*)cp[0], cp[0]->plate, 5); 
	hput(hp, (void*)cp[1], cp[1]->plate, strlen(cp[1]->plate));
	hput(hp, (void*)cp[2], cp[2]->plate, strlen(cp[2]->plate));


	// test successful match cases
	c = hsearch(hp, fn, cp[0]->plate, strlen(cp[0]->plate)); //HSEARCH requires (*htp, *searchfn, char *key, keylen)
	if ( c != cp[0] ) result ++;
	
	c = hsearch(hp, fn, cp[1]->plate, strlen(cp[1]->plate));
	if ( c != cp[1] ) result ++;
	
	c = hsearch(hp, fn, cp[2]->plate, strlen(cp[2]->plate));
	if ( c != cp[2] ) result ++;
	
	printf("result is %d\n", result); //result>0 if any of the hsearch failed
		
	if ((hsearch(hp, fn, "not_a_match", 5)) != NULL) { //test for search key (and keylen) with no match, should return NULL
		fprintf(stderr, "qsearch failed when it should have not matched\n");
		hclose(hp);
		exit(EXIT_FAILURE);
	} else printf(">>searching hashtable using key with no match occurred as expected\n");
	
	
	if ( result > 0 ) {
		fprintf(stderr, "hsearch failed when it should have matched\n");
		hclose(hp);
		exit(EXIT_FAILURE);
	} else printf("hsearch successful!\n");
	
	
	hclose(hp); //should free everything in hash table
	
	exit(EXIT_SUCCESS);
}

