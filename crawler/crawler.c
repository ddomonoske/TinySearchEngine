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
#include <unistd.h>
#include <sys/stat.h>

typedef struct stat stat_t;

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

/* pagesave saves info about a webpage (pagep) into a file (id) within the specified directory (dirname)
 * within file, writes webpage URL, depth, HTML length, and HTML
 * returns 1 upon completion
 */
int32_t pagesave(webpage_t *pagep, int id, char *dirname) {
	
	FILE *fp;
	char filename[10];

	sprintf(filename, "%d", id); //convert int to string
	
	//chdir("..");  // removed because I believe the ../ should be included in the directory name
	chdir(dirname); //changes over to right directory
	
	fp = fopen (filename,"w");
	fprintf(fp,"URL: %s\nDepth: %d\nHTML Length: %d\nHTML: %s\n", webpage_getURL(pagep), webpage_getDepth(pagep), webpage_getHTMLlen(pagep), webpage_getHTML(pagep)); //prints URL, depth, HTML length, and HTML into file
	
	fclose(fp);
	return 0; //will always return 0
}


/* to check if the directory under string beginning at *dir exists
 * returns true if it does, false if not
 * used for STEP 6
 */
bool isDirExist(char *dir) {
	stat_t s;
	bool b;
	if(!stat(dir,&s)) {
		if (S_ISDIR(s.st_mode))
			b =  true;
		else
			b = false;
	} else
		b = false;

	return b;
}


int main (int argc, char *argv[]) {

	webpage_t *page; 	//initial page we fetch
	webpage_t *intPage; //internal page
	webpage_t *tmpPage; //used to delete the webpages in the queue
	webpage_t *searchResult = NULL;

	queue_t *qp; //queue to store webpage_t addresses
	hashtable_t *hp; //hash table to store URLs already fetched
	uint32_t hsize = 6;

	char *seedurl;
	char *pagedir;
	uint32_t maxdepth, currdepth=0;
	
	page = (webpage_new("https://thayer.github.io/engs50/", 0, NULL)); //create new webpage_t of depth 0
	qp = qopen();
	hp = hopen(hsize);
	
	int pos = 0; //used for webpage_getNextURL function
	char *URLresult; //pointer to string, used to store each embedded URL during getNextURL

	// Checking inputs for STEP 6
	if (argc != 4) {
		printf("ERROR: Incorrect number of arguments.\n");
		printf("USAGE: crawler <seedurl> <pagedir> <maxdepth>\n");
		exit(EXIT_FAILURE);
	}
	seedurl = argv[1];
	pagedir = argv[2];
	if ( ! isDirExist(argv[2]) ) {
		printf("The directory '%s' does not exist.\n", argv[2]);
		exit(EXIT_FAILURE);
	}
	if (atoi(argv[3]) < 0) {
		printf("The max depth must be greater than 0.\n");
		exit(EXIT_FAILURE);
	} else
		maxdepth = atoi(argv[3]);
	
	printf("Internal and External URLs: \n");

	if(webpage_fetch(page)) { //fetch webpage HTML of new webpage_t, returns true if success

		//STEP 5
		uint32_t save;
		save = pagesave(page, 1, "../pages");
		printf("\nSave status: %d\n\n", save);

		do {
			while ((pos = webpage_getNextURL(page, pos, &URLresult)) > 0) { //go through webpage looking for embedded URLs
				//assign each URL string found to URLresult, continue until no more embedded URLs

				//STEP 4
				searchResult = hsearch(hp, fn, URLresult, strlen(URLresult)); //search for URL in hashtable, return NULL if not found
				
				if (IsInternalURL(URLresult) && searchResult == NULL) { //check to see if URL is internal
					printf("Internal url: %s\n", URLresult);
					hput(hp, URLresult, URLresult, strlen(URLresult)); //add new URL into hashtable (use URL itself as hash key bc key must be a char)
					
					//STEP 3
					intPage = (webpage_new(URLresult, currdepth+1, NULL)); //create new webpage_t for each internal URL, assign correct depth
					webpage_fetch(intPage); //store HTML into webpage_t as well
					qput(qp, intPage); //place new webpage_t into queue
				} 
				else {	//if URL is external
					//printf("External url: %s\n", URLresult);
					free(URLresult);
				}
			}
			if ((page = qget(qp)) == NULL)
				currdepth = maxdepth + 1;
			else
				currdepth = webpage_getDepth(page);
			printf("next URL: %s\n", webpage_getURL(page));
			printf("currdepth = %d\n", currdepth);
			pos = 0;
		} while (currdepth <= maxdepth); 
		
    
		printf("\n\nStep 3 Verification: \n");
    	
		//use qget to remove each webpage_t address from the queue and print each associated URL
		while((tmpPage = (webpage_t*)qget(qp)) != NULL){ //while queue is not empty
			printf("%s\n", webpage_getURL(tmpPage));  //print URL
			printf("HTML Length: %d\n", webpage_getHTMLlen(tmpPage)); //print HTML length
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




    
