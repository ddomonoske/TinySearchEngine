/* list.c -- implements interface for list.h
 *
 *
 * Author: Eric Chen
 * Created: Thursday, Oct 1, 2020
 * Version:
 *
 * Description: functions on car_t object
 
 
*/ 


#include <stdio.h>
#include <stdint.h>
#include "list.h"
#include <string.h>


static car_t *front = NULL;


/* put(): place a car at the beginning of the list
 * returns 0 if successful; nonzero otherwise 
 */
int32_t lput(car_t *cp) {

	if (cp == NULL) {	
		printf("cp is NULL\n");
		return 1;
	}
	
	if (front == NULL) {	//if list empty
		front = cp;	
	}
	else {						//if list non-empty
		cp->next = front;		//set cp->next to point to original front
		front = cp;				//set cp as the new front
	}
	
	printf("car added to list\n");
	return 0;
}


/* get(): remove and return the first car in the list;
 * return NULL if the list is empty
 */
car_t *lget() {

	if (front==NULL) {
		printf("list is empty\n");
		return NULL;
	}
	else {
		printf("first car removed and returned\n");
		car_t *firstCar = front;
		front = front->next; //effectively removes first car from list
		return firstCar; //returns first car
	}	
}



/* apply a function to every car in the list */
void lapply(void (*fn)(car_t *cp)) {
	
	if (fn==NULL) {
		printf("Error. Function is NULL\n");
	}
	if (front==NULL) {
		printf("list is empty\n");
	}
	else {
		printf("Applying function to every car.\n");
		car_t *p; //create pointer to start looping through car_t list
		for (p=front; p!=NULL; p=p->next) { //loops through list and applies function fn to each item
			fn(p);
		}
	}

}


/* remove(): find, remove, and return any car with
 * the designated plate; return NULL if not present
 */
car_t *lremove(char *platep) { 

	if (platep==NULL) {
		printf("Error. Invalid plate entry.\n");
		return NULL;
	}
	
	car_t *p, *f; //pointers to help loop through list
	
	for (p=front; p!=NULL; p=p->next) { //loop through list and find car with matching plate	
		
		if (strcmp(p->plate,platep) == 0) { 
			printf("match found!\n");
			
			if (p==front) { 		//if match is first item
				front = p->next; 	//update front to point to second item
				return p; 			//return pointer to first item
			}
			else { 					//if match is not first item
				f->next = p->next; 	//removes matching item from list. If p is last item, then f->next becomes NULL
				return p; 			//return pointer to matching item
			}
		}
		
		f = p; //f is always one behind p
	}
	
	printf("no matches found\n");
	return NULL; //if goes through entire list and no matches occur, then returns NULL
}









