/* crawler.c --- simply prints hello for now
 * 
 * 
 * Author: Eric Chen
 * Created: Thu Oct 15 13:31:31 2020 (-0400)
 * Modified: Chris Lyke
 *           David Domonoske
 * Version: 
 * 
 * Description: 
 * 
 */


#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h> //needed for malloc
#include <unistd.h>
#include <sys/stat.h>
#include "pageio.h" //do we need any of these?
#include "webpage.h"
#include "lqueue.h"
#include "lhash.h"
#include <pthread.h>


static uint32_t id = 1; //GLOBAL VARIABLE so all threads can access

/* new structure w/ queue and hash table for crawling - passed as 4th argument for threading */
typedef struct crawler {
	lqueue_t *lqp;
	lhashtable_t *lhp;
	uint32_t maxdepth; //to track depth of crawl
	char *pagedir; //to save all crawled ids
} crawler_t;


/* create crawler_t object with lqp and hqp
 */
crawler_t* make_crawler(lqueue_t *lqp, lhashtable_t *lhp, int maxdepth, char *pagedir) {
	
	crawler_t *cp;
	
	if ( !(cp = (crawler_t*)malloc(sizeof(crawler_t))) ) { 	//check if enough space in memory for crawler_t*
		printf("Error: malloc failed allocating crawler object\n");
		return NULL;
	}
	cp->lqp = lqp;
	cp->lhp = lhp;
	cp->maxdepth = maxdepth;
	cp->pagedir = pagedir;
	return cp;
} 


/* printPage -- used for debugging
 *  - passed into locked queue of pages at end of each while to see what's in queue
 */
void printPage(void *page) {
	if (page == NULL) {
		printf("page = NULL???\n");
	} else {
		printf("%s\n",webpage_getURL((webpage_t*)page));
	}
}


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


/* 
	function to pass into threading
*/
void *tcrawl(void *argp) { //function that calls LQPUT
	
	crawler_t *cp;
	
	webpage_t *page; 	//pages fetched
	webpage_t *intPage; //internal page
	webpage_t *searchResult = NULL;
	uint32_t currdepth;
	uint32_t maxdepth;
	int pos; //used for webpage_getNextURL function
	char *URLresult; //pointer to string, used to store each embedded URL during getNextURL
	char *pagedir;
	
	cp = (crawler_t*)argp;
	maxdepth = cp->maxdepth;
	pagedir = cp->pagedir;

	page = lqget(cp->lqp); //takes out seedURL from lqp. Now we can do the crawling as normal

	do {  // STEP 6: loop through all the pages
		currdepth = webpage_getDepth(page);
		if (webpage_fetch(page)) { //fetch webpage HTML of new webpage_t, returns true if success
			
			//STEP 5
			pagesave(page, id++, pagedir);  // STEP 6: save to pagedir

			if (currdepth < maxdepth) {  // STEP 6: only search page if not at maxdepth
				pos = 0;
				while ((pos = webpage_getNextURL(page, pos, &URLresult)) > 0) { //go through webpage looking for embedded URLs
					//assign each URL string found to URLresult, continue until no more embedded URLs

					if (NormalizeURL(URLresult)) { //normalize URL found
						
						//STEP 4
						searchResult = lhsearch(cp->lhp, fn, URLresult, strlen(URLresult)); //search for URL in hashtable, return NULL if not found
				
						if (IsInternalURL(URLresult) && searchResult == NULL) { //check to see if URL is internal and has not been checked
							printf("Internal url: %s\n", URLresult);
							lhput(cp->lhp, URLresult, URLresult, strlen(URLresult)); //add new URL into hashtable (use URL itself as hash key bc key must be a char)
					
							//STEP 3
							intPage = (webpage_new(URLresult, currdepth+1, NULL)); //create new webpage_t for each internal URL, assign correct depth
							lqput(cp->lqp, intPage);
						} 
						else if ( ! IsInternalURL(URLresult) ) {	//if URL is not internal
							printf("External url: %s\n", URLresult);
							free(URLresult);
							URLresult = NULL;
						}
						else {	//if URL is has already been checked (aka found in hash table)
							free(URLresult);
							URLresult = NULL;
						}
						
					}
					else {
						printf("Embedded URL cannot be normalized\n"); //and moves into next embedded URL
					}

				}
			}
		}
		webpage_delete((void*)page);
	} while ((page = lqget(cp->lqp)) != NULL) ; //finishes adding crawled pages into crawler directory

	return(cp);
}


int main (int argc, char *argv[]) {

	pthread_t tid1,tid2; //to do multithreading
	
	// ** ADD SEEDURL TO QUEUE AND HASH IN CRAWLER_T OBJECT***
	crawler_t *cp; // single crawler_t object to store lqp and lhp
	uint32_t hsize = 6;

	webpage_t *page; //to hold initial page we fetch
	char *seedurl;
	char *URLresult; //pointer to string, used to store each embedded URL during getNextURL
	char *pagedir;
	uint32_t maxdepth;
		
	FILE *crawlerfp; //pointer to a .crawler file, to indicate that the pagedir is a crawler dir

	// Checking inputs for STEP 6
	if (argc != 4) {
		printf("ERROR: Incorrect number of arguments.\n");
		printf("USAGE: crawler <seedurl> <pagedir> <maxdepth>\n");
		exit(EXIT_FAILURE);
	}
	seedurl = argv[1]; //root URL of crawl
	if ( ! IsInternalURL(seedurl) ) {
		printf("The seed URL is not internal\n");
		exit(EXIT_FAILURE);
	}
	pagedir = argv[2]; //crawler directory to save all ID files
	if ( ! isDirExist(argv[2]) ) {
		printf("The directory '%s' does not exist.\n", argv[2]);
		exit(EXIT_FAILURE);
	}
	if (atoi(argv[3]) < 0) {
		printf("The max depth must be greater than 0.\n");
		exit(EXIT_FAILURE);
	} else
		maxdepth = atoi(argv[3]); //depth of crawl
	
	cp = make_crawler(lqopen(),lhopen(hsize),maxdepth,pagedir); //creates crawler object, lqopen and lhopen return pointers to the newly created objects!

	
	if (NormalizeURL(seedurl)) { //normalize URL	
		page = (webpage_new(seedurl, 0, NULL)); //create new webpage_t of depth 0
		URLresult = (char*)malloc(strlen(seedurl)*(sizeof(char)+1));
		strcpy(URLresult,seedurl);
		lqput(cp->lqp,page); //add seed URL to lqueue (later taken out by lqget in tcrawl function)
		lhput(cp->lhp,URLresult, URLresult, strlen(URLresult)); // add seed URL to hash table, but not queue
	}
	else {
		printf("Seed URL cannot be normalized\n");
		exit(EXIT_FAILURE);
	} 

	
	if(pthread_create(&tid1,NULL,tcrawl,cp) != 0 ) {
		exit(EXIT_FAILURE);
	}
	if(pthread_create(&tid2,NULL,tcrawl,cp) != 0 ) {
		exit(EXIT_FAILURE);
	}

	if(pthread_join(tid1,NULL) != 0 ) {
		exit(EXIT_FAILURE);
	}
	if(pthread_join(tid2,NULL) != 0 ) {
		exit(EXIT_FAILURE);
	}

	
	printf("\nTotal: %d web pages stored\n",id-1);
	
	chdir(pagedir); //enter crawler dir
	crawlerfp = fopen(".crawler","w"); //created hidden .crawler file within crawler dir
	fclose(crawlerfp);
	
	//DEALLOCATE all memory used
	lqclose(cp->lqp); //close lqueue
	lhclose(cp->lhp); //close lhashtable
	free(cp);
	
	webpage_t *retrieve = pageload(1,"../pages"); //load first saved webpage
	pagesave(retrieve,1,"../pages1"); //save it to a different directory
	webpage_delete((void*)retrieve);
	
	exit(EXIT_SUCCESS);  
}
