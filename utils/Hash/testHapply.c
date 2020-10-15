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
	
	car_t *cp[3];
	
	hp = hopen(hsize); //returns pointer to newly created hashtable_t
	
	cp[0] = make_car("car1", 1.0, 1910); //declare all cars
	cp[1] = make_car("car2", 4.0, 1940);
	cp[2] = make_car("car3", 9.0, 1990);
	
	// test with NULLs
	happly(NULL, NULL);
	happly(NULL, fn);
	happly((hashtable_t*)hp, NULL);
	
	// test with empty queue, should print queue is empty
	happly((hashtable_t*)hp, fn);
	
	result = 0;
	result += hput(hp, (void*)cp[0], cp[0]->plate, strlen(cp[0]->plate)); //put to empty queue (returns 1 if any error)
	result += hput(hp, (void*)cp[1], cp[1]->plate, strlen(cp[1]->plate)); //put into non-empty queue
	result += hput(hp, (void*)cp[2], cp[2]->plate, strlen(cp[2]->plate));

	happly((hashtable_t*)hp, fn); //test with non-empty queue, should print "Applying function,,,"

	for (int i=0; i<3; i++ ) {
		if ( cp[i]->year != 0 )
			result++;
	}

	if ( result > 0 ) {
		fprintf(stderr, "happly failed to implement on non-empty queue\n");
		hclose(hp);
		exit(EXIT_FAILURE);
	}
	else printf("happly successful");

	hclose(hp);
	
	exit(EXIT_SUCCESS);
}

 
