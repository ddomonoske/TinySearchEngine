/* testHremove.c -- test hremove() in hash.c
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
	
	car_t *cp[5];
	car_t *c = NULL;
	car_t *cp1, *cp2, *cp3;
	
	// test with NULLS and keylen < 1
	c = hremove(NULL, NULL, NULL, 10); //HREMOVE requires (*htp, *searchfn, char *key, keylen)
	if (c != NULL) result++;
	c = hremove(NULL, fn, (void*)("randomString"),10); //*htp NULL
	if (c != NULL) result++;
	c = hremove(hp, NULL, (void*)("randomString"),10); //*hremove NULL
	if (c != NULL) result++;
	c = hremove(hp, fn, NULL, 10); //char *key NULL
	if (c != NULL) result++;
	c = hremove(hp, fn, (void*)("randomString"),0); //keylen < 1
	if (c != NULL) result++;
	
	if ( result > 0 ) {
		fprintf(stderr, "Error while removing with NULL input / keylen>1\n");
		hclose(hp);
		exit(EXIT_FAILURE); //no need to reset result bc will exit
	}

	//declare all cars
	cp[0] = make_car("car1", 1.0, 1910);
	cp[1] = make_car("car2", 4.0, 1940);
	cp[2] = make_car("car3", 9.0, 1990);
	cp[3] = make_car("car4", 29.0, 1995);
	cp[4] = make_car("car5", 91.0, 1999);
	
	
	// test with empty hashtable
	if ((hremove(hp, fn, (void*)("randomString"),10)) != NULL) { //hremove calls qremove, which should print "Queue is empty"
		printf("Error while removing from empty hashtable");
	}
	else printf(">>removing from empty hashtable occurred as expected\n");
	
	// assuming hput is good
	hput(hp, (void*)cp[0], cp[0]->plate, strlen(cp[0]->plate)); 
	hput(hp, (void*)cp[1], cp[1]->plate, strlen(cp[1]->plate));
	hput(hp, (void*)cp[2], cp[2]->plate, strlen(cp[2]->plate));


	// test successful match cases
	cp1 = hremove(hp, fn, cp[0]->plate, strlen(cp[0]->plate)); //HREMOVE requires (*htp, *searchfn, char *key, keylen)
	if ( cp1 != cp[0] ) result ++;
	cp2 = hremove(hp, fn, cp[1]->plate, strlen(cp[1]->plate));
	if ( cp2 != cp[1] ) result ++;
	cp3 = hremove(hp, fn, cp[2]->plate, strlen(cp[2]->plate)); //removing last element from hashtable
	if ( cp3 != cp[2] ) result ++;
	
	printf("result is %d\n", result); //result>0 if any of the hsearch failed
	printf("hash table should now be empty\n");
	
	hput(hp, (void*)cp[3], cp[3]->plate, strlen(cp[3]->plate));
	hput(hp, (void*)cp[4], cp[4]->plate, strlen(cp[4]->plate));
	
	if ((hremove(hp, fn, "not_a_match", 6)) != NULL) { //test for search key (and keylen) with no match, should return NULL
		fprintf(stderr, "qremove failed when it should have not matched\n");
		hclose(hp);
		exit(EXIT_FAILURE);
	} else printf(">>removing from hashtable using key with no match occurred as expected\n");
	
	
	if ( result > 0 ) {
		fprintf(stderr, "hremove failed when it should have matched\n");
		hclose(hp);
		exit(EXIT_FAILURE);
	} else printf("hremove successful!\n");
	
	free(cp1); //need to free pointers for elements that were removed from hash table
	free(cp2);
	free(cp3);
	//free(cp[3]);
	//free(cp[4]);
	
	hclose(hp);
	
	exit(EXIT_SUCCESS);
}

