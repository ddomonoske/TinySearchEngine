/* test1to3.c -- tests hopen, hclose, and hput functions in hash.h interface
 *
 * Author: Chris Lyke
 * Created: Oct 12
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


/* the representation of a student */
typedef struct student {
	char name[MAXREG];
	int year;
} student_t;

/* make_student(): creates a student object */
student_t* make_student(char *namep, int year) {
	
	student_t *sp;
	if ((sp = (student_t*)malloc(sizeof(student_t))) == NULL) { 	//check if enough space in memory for car_t*
		printf("Error: malloc failed allocating car\n");
		return NULL;
	}
	printf("student malloc succesful\n");
	strcpy(sp->name, namep);
	sp->year = year;
	return sp;
}


/* fn() takes a car_t pointer and sets year of car_t to 0
 * returns nothing
 */
void fn(void *p) {
	car_t *cp = (car_t*)p;
	cp->year = 0;
}
	
 
int main (void) {

	int32_t result;
	uint32_t hsize = 20;

	hashtable_t *hp; //agnostic hashtable, can hold cars and students
	hashtable_t *hpNULL = NULL; //dumby hashtable
	
	car_t *cp1, *cp2, *cp3;
	
	// TEST HOPEN
	hp = hopen(hsize); //returns pointer to newly created hashtable_t
	if (hp == NULL) {
		printf("hopen returns NULL when should have opened successfully\n");
		exit(EXIT_FAILURE);
	}
	else printf("hopen successful\n\n");
	
	
	// TEST HPUT
	cp1 = make_car("car1", 1.0, 1910); //declare all cars
	cp2 = make_car("car2", 4.0, 1940);
	cp3 = make_car("car3", 9.0, 1990);
	
	result = 0;
	result += hput(hp, (void*)cp1, cp1->plate, strlen(cp1->plate)); //put to empty queue (returns 1 if any error)
	result += hput(hp, (void*)cp2, cp2->plate, strlen(cp2->plate)); //put into non-empty queue
	result += hput(hp, (void*)cp3, cp3->plate, strlen(cp3->plate));

	if ( result > 0 ) {
		printf("hput error \n");
		hclose(hp);	//need to free memory of everything already put into hash
		exit(EXIT_FAILURE);
	}
	else printf("hput cases successful\n\n");
	
	cp1->year = 0000; //can directly alter the data stored queue from this main code
 	
 
	// TEST HCLOSE
	hclose(hpNULL); //close a NULL queue
	printf(">>hclose (hpNULL) occurred as expected\n");

	hclose(hp);
	printf("hclose successful\n\n");
	
	
	exit(EXIT_SUCCESS);
}