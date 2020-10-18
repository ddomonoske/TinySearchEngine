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

int main (void) {

	webpage_t *page;
	webpage_t *intPage;
	webpage_t *tmpPage;
	queue_t *qp;
	char *urlStr;
	
	
	int pos = 0;
	char *URLresult;
	
	page = (webpage_new("https://thayer.github.io/engs50/", 0, NULL)); //create new webpage of depth 0
	qp = qopen();
	
	printf("Step 2: \n");

	if(webpage_fetch(page)) { //fetch the webpage HTML to local computer
    	while((pos = webpage_getNextURL(page, pos, &URLresult)) > 0) { //loop through HTML looking for URLs
    	
    		if (IsInternalURL(URLresult)) { //check to see if URL is internal or not
    			printf("Internal url: %s\n", URLresult);
    			
    			intPage = (webpage_new(URLresult, 0, NULL)); //create new webpage for internal URL
    			qput(qp, (void*)intPage); //place new webpage into a queue
    			
				
    		} else {
    			printf("External url: %s\n", URLresult);
    		}

			free(URLresult); 
    	}
    	
    	printf("\n\n\nStep 3: \n");
    	
    	//use qget to print the URLs and delete all the webpages in the queue
    	while((tmpPage = (webpage_t*)qget(qp)) != NULL){
    		urlStr = webpage_getURL(tmpPage);
    		printf("%s\n", urlStr);
    		webpage_delete((void*)tmpPage);
    	}

    	qclose(qp); //close queue
    	webpage_delete((void*)page); //delete original webpage
    	exit(EXIT_SUCCESS);
  	}
  	else exit(EXIT_FAILURE);
	
}



    
