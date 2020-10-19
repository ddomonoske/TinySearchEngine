/* crawler.c --- simply prints hello for now
 * 
 * 
 * Author: Eric Chen
 * Created: Thu Oct 15 13:31:31 2020 (-0400)
 * Version: 
 * 
 * Description: 
 * 
 */

#include "webpage.h"
#include "queue.h"
#include "hash.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>


/* fn() checks if the car_t pointer matches with the provided key
 * returns bool
 */
bool fn(void* p, const void* keyp) { //requires (void* elementp, const void* keyp)
	if ( p == NULL) return false;
	
	char *currURL = (char*)p;
	char *key = (char*)keyp;
	
	if ( strcmp(currURL, key) == 0 ) //if match
		return true;
	else
		return false;
}


int main (void) {

	webpage_t *page; 	//initial page we fetch
	webpage_t *intPage; //internal page
	webpage_t *tmpPage; //used to delete the webpages in the queue
	webpage_t *searchResult = NULL;
	
	queue_t *qp; //queue to store webpage_t addresses
	int pos = 0;
	char *URLresult; //pointer to character pointer, used to store each embedded URL within a webpage during getNextURL
	
	hashtable_t *hp; //hash table to store URLs already fetched
	uint32_t hsize = 6;
	

	page = (webpage_new("https://thayer.github.io/engs50/", 0, NULL)); //create new webpage_t of depth 0
	
	
	/* 
	****************
	Insert step 5 code to save page
	****************
	*/
	
	
	qp = qopen();
	hp = hopen(hsize);
	
	printf("Internal and External URLs: \n");

	if(webpage_fetch(page)) { //fetch webpage HTML of new webpage_t, returns true if success
	
    	while((pos = webpage_getNextURL(page, pos, &URLresult)) > 0) { //go through HTML code looking for URLs
    	//assign each URL string found to URLresult, continue until no more embedded URLs
    	
    		//STEP 4
    		searchResult = hsearch(hp, fn, URLresult, strlen(URLresult)); //search for URL in hashtable, return NULL if not found
    		if (searchResult == NULL) { //if URL is not in hashtable
    			
    			if (IsInternalURL(URLresult) && (searchResult == NULL)) { //if URL is internal
    				printf("Internal url: %s\n", URLresult);
    				hput(hp, URLresult, URLresult, strlen(URLresult)); //add new URL into hashtable (use URL itself as hash key bc key must be a char)
					
					//STEP 3
    				intPage = (webpage_new(URLresult, 1, NULL)); //create new webpage_t for each internal URL, assign correct depth
    				qput(qp, intPage); //place new webpage_t into queue
	    		} 
    			else {	//if URL is external
    				printf("External url: %s\n", URLresult);
    				free(URLresult);
    			}
    		}

    	}
    	
    	printf("\n\n\nStep 3 Verification: \n");
    	
    	//use qget to remove each webpage_t address from the queue and print each associated URL
    	while((tmpPage = (webpage_t*)qget(qp)) != NULL){ //while queue is not empty
    		printf("%s\n", webpage_getURL(tmpPage));  //print URL
    		webpage_delete((void*)tmpPage); //delete webpage_t referenced by tmpPage address
    	}
    	
    	//DEALLOCATE all memory used
    	qclose(qp); //close queue
    	hclose(hp); //close hashtable
    	webpage_delete((void*)page); //delete original webpage_t
    	
    	exit(EXIT_SUCCESS); 
  	}
  	
  	else exit(EXIT_FAILURE); //EXIT_FAILURE if fetch of seed URL does not succeed
	
}




    
