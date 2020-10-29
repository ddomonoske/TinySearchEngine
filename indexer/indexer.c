/* indexer.c --- 
 * 
 * 
 * Author: David W. Domonoske
 * Created: Sun Oct 25 12:27:25 2020 (-0400)
 * Version: 
 * 
 * Description: 
 * 
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/stat.h>
#include "pageio.h"
#include "webpage.h"
#include "queue.h"
#include "hash.h"
#include "indexio.h"

static int sumGlobal = 0;

//structures defined in indexio.h

/* Determine if a word from a page's html is valid. Also makes all character lowercase
 * Valid is defined as:
 *   - longer than 3 characters
 *   - only contains alphabetic characters
 * Returns true if the word is valid, false if not
 */
bool NormalizeWord(char *word) {
	int i, n = strlen(word);

	if (n < 3)
		return false;
	
	for (i=0; i<n; i++) {
		if (isalpha(word[i]) == 0)
			return false;
		word[i] = tolower(word[i]);
	}
	return true;
}


/* check if a word matches with a wc_t word
 */
bool wordmatch(void* elementp, const void* word) {
	wc_t *ep = (wc_t*)elementp;
	char *wp = (char*)word;
	if (strcmp(ep->word,wp) == 0)  // true if the word is a match
		return true;
	else
		return false;
}


/* add the count of words in each queue element to the sum (should be 141)
 *   --used with qapply for step4
 */
void sumwordsQueue(void* elementp) {
	counters_t *docp = (counters_t*)elementp;
	sumGlobal += docp->count;
}


/* call sumwordsQueue for each queue in the hashtable
 *  --used with happly
 */
void sumwordsHash(void* elementp) {
	//wc_t *wcp = (wc_t*)elementp;
	//qapply(wcp->qp, sumwordsQueue);
	sumGlobal++;
}


/* look in queue for element with matching doc ID
 */
bool QueueSearchFn(void* elementp, const void* docID) { //requires (void* elementp, const void* keyp)
	counters_t *doc = (counters_t*)elementp;
	int *id = (int*)docID;
	return (doc->id == *id); //if match return true
}


/* used to make the incrementing file names
 */
char* itoa(char *a, int i) {
	sprintf(a, "%d", i);
	return a;
}


int main(int argc, char *argv[] ) {
	char *pagedir, *indexnm;
	char doc_id[10], cwd[MAX_PATH_LENGTH];

	webpage_t *page;
	char *word;
	int pos, id=0;

	hashtable_t *hp;
	wc_t *wc_found;
	
	queue_t *qp;
	counters_t *doc_found;

	// Check that there are correct number of arguments
	if (argc!=3) {
		printf("USAGE: indexer <pagedir> <indexnm>\n");
		exit(EXIT_FAILURE);
	}
	pagedir = argv[1];
	if ( ! isDirExist(argv[1]) ) {
		printf("The directory '%s' does not exist.\n", argv[1]);
		exit(EXIT_FAILURE);
	}
	getcwd(cwd, sizeof(cwd));
	chdir(pagedir);
	indexnm = argv[2];

	/*
    page 1 = 141 words
    page 2 = 73 words
    page 3 = 109 words
    page 4 = 365 words
	*/
	
	//chdir("../indexer");  // for Step 2
	//fp = fopen("output1","w"); // for Step 2
	
	hp = hopen(HTABLE_SIZE);  //only open after args are valid

	while ( access(itoa(doc_id, ++id), F_OK) != -1 ) {
		page = pageload(id ,pagedir);
		pos = 0;
		
		while ((pos = webpage_getNextWord(page, pos, &word)) > 0) { //go through every word in html
			if (NormalizeWord(word)) {  // only add to hash table if normalized
				if ((wc_found = hsearch(hp, wordmatch, word, strlen(word))) == NULL) { //if word not found in hash table
					//printf("adding '%s' to hash table\n", word);
					qp = qopen(); //make new queue
					qput(qp, make_doc(id,1)); //place doc element in queue
					hput(hp, make_wc(word, qp), word, strlen(word)); //put queue in hash table
					
				} else { //if word is in hash table
					//printf("increasing the count for '%s' by 1 for doc %d\n", word, id);
					
					//look for element in queue with matching doc ID
					doc_found = qsearch(wc_found->qp, QueueSearchFn, &id); 
					if((doc_found) != NULL){ //if doc element is in queue
						(doc_found->count)++; //add to count inside doc element in queue		
					}	
					else { //if no doc element in the queue, make one
						qput(wc_found->qp, make_doc(id,1));  //place new doc in queue
					}
				}
				//fprintf(fp, "%s\n", word);  // remnant from Step 2
			}
			free(word); 
		}
		webpage_delete(page); 
	}
	
	chdir(cwd);
	
	//Calculate sum of all counts in the hash table and internal queues
	happly(hp, sumwordsHash); //call sumwordsHash for each word in the hashtable
	
	printf("*** %d words ***\n", sumGlobal);
	
	indexsave(hp,indexnm); //only need hash table and name to create index file
	
	happly(hp, freeWord); //Use freeWord function to delete all the word strings in the hashtable
	hclose(hp);
	
	exit(EXIT_SUCCESS);
}
