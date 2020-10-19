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

	webpage_t *page; //initial page we fetch
	webpage_t *intPage; //internal page
	webpage_t *tmpPage; //used to delete the webpages in the queue
	webpage_t *searchResult = NULL;
	queue_t *qp; //queue
	int pos = 0;
	char *URLresult;
	//char *currURL;
	
	uint32_t hsize = 6;
	hashtable_t *hp;
	

	page = (webpage_new("https://thayer.github.io/engs50/", 0, NULL)); //create new webpage of depth 0
	
	
	/* 
	****************
	Insert step 5 code to save page
	****************
	*/
	
	
	qp = qopen();
	hp = hopen(hsize);
	
	printf("Internal and External URLs: \n");

	if(webpage_fetch(page)) { //fetch the webpage HTML to local computer
    	while((pos = webpage_getNextURL(page, pos, &URLresult)) > 0) { //loop through HTML looking for URLs
    	
    		searchResult = hsearch(hp, fn, URLresult, strlen(URLresult)); //search for URL in hashtable
    	
    		if (IsInternalURL(URLresult) && (searchResult == NULL)) { //check to see if URL is internal or not
    			printf("Internal url: %s\n", URLresult);
    			
    			intPage = (webpage_new(URLresult, 0, NULL)); //create new webpage for internal URL
    				
    			hput(hp, URLresult, URLresult, strlen(URLresult)); //place new webpage into hashtable
    			qput(qp, intPage); //place new webpage into queue
	
    		} else {
    			printf("External url: %s\n", URLresult);
    			free(URLresult);
    		}
    	}
    	
    	printf("\n\n\nStep 4: \n");
    	
    	//use qget to print the URLs and delete all the webpages in the queue
    	while((tmpPage = (webpage_t*)qget(qp)) != NULL){
    		printf("%s\n", webpage_getURL(tmpPage));
    		webpage_delete((void*)tmpPage);
    	}

    	qclose(qp); //close queue
    	hclose(hp); //close hashtable
    	webpage_delete((void*)page); //delete original webpage
    	exit(EXIT_SUCCESS);
  	}
  	else exit(EXIT_FAILURE);
	
}




    
