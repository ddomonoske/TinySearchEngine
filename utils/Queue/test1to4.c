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

/* makeCar(): creates a student object */
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
void fn(car_t *cp) {
	cp->year = 0;
}
	
 
int main (void) {

	int32_t result;

	queue_t *qp; //agnostic queue, can hole cars and students
	queue_t *qpNULL = NULL; //dumby queue
	
	car_t *cp1, *cp2, *cp3;
	car_t *cpNULL = NULL;
	car_t *cpFirst, *cpSecond, *cpThird;\
	
	student_t *sp1, *sp2;
	student_t *spFirst, *spSecond;
	
	
	// TEST QOPEN
	qp = qopen(); //returns pointer to newly created queue_t
	if (qp == NULL) {
		printf("qopen returns NULL when should have opened successfully\n");
		exit(EXIT_FAILURE);
	}
	else printf("qopen successful\n\n");


	// TEST QPUT
	if((result = qput(qp, (void*)cpNULL)) == 0){ //testing qput with NULL element
		printf(">>ISSUE: qput succeeded with NULL element when should have failed\n");
		exit(EXIT_FAILURE);
	}
	else printf(">> qput (cpNULL) occurred as expected\n");
	
	cp1 = make_car("car1", 1.0, 1910); //declare all cars
	cp2 = make_car("car2", 4.0, 1940);
	cp3 = make_car("car3", 9.0, 1990);
	
	sp1 = make_student("Eric",2021); //declare all students
	sp2 = make_student("David",2020); 
	
	result = 0;
	result += qput(qp, (void*)cp1); //put to empty queue (returns 1 if any error)
	result += qput(qp, (void*)cp2); //put into non-empty queue
	result += qput(qp, (void*)cp3);
	result += qput(qp, (void*)sp1);	//put different objects into same queue
	result += qput(qp, (void*)sp2);

	if ( result > 0 ) {
		printf("qput error \n");
		qclose(qp);	//need to free memory of everything already put into queue
		exit(EXIT_FAILURE);
	}
	else printf("qput cases successful\n\n");
	
	cp1->year = 0000; //can directly alter the data stored queue from this main code
 
 
 	// TEST QGET
 	cpNULL = (car_t*)qget(qpNULL); //get from NULL queue
 	if (cpNULL != NULL) {
 		printf(">>ISSUE: qget succeeded with NULL queue when should have failed\n");
		qclose(qp);
		exit(EXIT_FAILURE);
 	}
 	printf(">>qget (qpNULL) occurred as expected\n");

	cpFirst = (car_t*)qget(qp); //get from non-empty queue. Need to coerce void pointer into correct object pointer
	if ( cpFirst != cp1 ) result ++; //cp1 and cpFirst should point to same memory
	cpSecond = (car_t*)qget(qp); 
	if ( cpSecond != cp2 ) result ++;
	cpThird = (car_t*)qget(qp);	
	if ( cpThird != cp3 ) result ++;
	spFirst = (student_t*)qget(qp);
	if ( spFirst != sp1 ) result ++;
	spSecond = (student_t*)qget(qp); //get last element from queue
	if ( spSecond != sp2 ) result ++;
	
	cpNULL = (car_t*)qget(qp); //get from empty queue
	if (cpNULL != NULL) {
		printf(">>ISSUE: qget succeeded on empty queue when should have failed\n");
		qclose(qp);
		exit(EXIT_FAILURE);
	}
	else printf(">>qget from empty queue occurred as expected\n");
	
	
	if (result>0) {
	 	printf("Error with qget\n");
	 	qclose(qp);
		exit(EXIT_FAILURE);
	}
	else printf("qget successful\n\n");

	printf("Car 1: plate = %s, price = %f, year = %d\n", cpFirst->plate, cpFirst->price, cpFirst->year); //show all data in queue
	printf("Car 2: plate = %s, price = %f, year = %d\n", cpSecond->plate, cpSecond->price, cpSecond->year);
	printf("Car 3: plate = %s, price = %f, year = %d\n", cpThird->plate, cpThird->price, cpThird->year);
	printf("Student 1: name = %s, year = %d\n", spFirst->name, sp1->year); //sp1 and spFirst point to same memory
	printf("Student 2: name = %s, year = %d\n", sp2->name, spSecond->year);
	

	// TEST QCLOSE
	qclose(qpNULL); //close a NULL queue
	printf(">>qclose (qpNULL) occurred as expected\n");

	qclose(qp);
	printf("qclose successful\n\n");
	
	free(cpFirst);
	free(cpSecond);
	free(cpThird);
	free(spFirst);
	free(spSecond);

	exit(EXIT_SUCCESS);
}


 
