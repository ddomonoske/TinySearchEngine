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
	wc_t *wcp = (wc_t*)elementp;
	qapply(wcp->qp, sumwordsQueue);
}


/* look in queue for element with matching doc ID
 */
bool QueueSearchFn(void* elementp, const void* docID) { //requires (void* elementp, const void* keyp)
	counters_t *doc = (counters_t*)elementp;
	int *id = (int*)docID;
	return (doc->id == *id); //if match return true
}


int main(int argc, char *argv[] ) {
	webpage_t *page;
	char *word;
	int pos, idmax, id;

	hashtable_t *hp;
	wc_t *wc_found;
	
	queue_t *qp;
	counters_t *doc_found;

	//Check that their are the write number of arguments
	if (argc!=2) {
		printf("usage: indexer <id>\n");
		exit(EXIT_FAILURE);
	}
	
	idmax = strtod(argv[1], argv); //get input argument from user
	if (idmax < 1){
		printf("usage: indexer <id>\n");
		exit(EXIT_FAILURE);
	}
  
	/*
    page 1 = 141 words
    page 2 = 73 words
    page 3 = 109 words
    page 4 = 365 words
	*/
	
	//chdir("../indexer");  // for Step 2
	//fp = fopen("output1","w"); // for Step 2
	
	hp = hopen(HTABLE_SIZE);  //only open after args are valid
	
	for (id=1; id<=idmax; id++){
		
		page = pageload(id ,"../pages");
		pos = 0;
		
		while ((pos = webpage_getNextWord(page, pos, &word)) > 0) { //go through every word in html
			if (NormalizeWord(word)) {  // only add to hash table if normalized
				if ((wc_found = hsearch(hp, wordmatch, word, strlen(word))) == NULL) { //if word not found in hash table
					printf("adding '%s' to hash table\n", word);
					qp = qopen(); //make new queue
					qput(qp, make_doc(id,1)); //place doc element in queue
					hput(hp, make_wc(word, qp), word, strlen(word)); //put queue in hash table
					
				} else { //if word is in hash table
					printf("increasing the count for '%s' by 1 for doc %d\n", word, id);
					
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
	
	//Calculate sum of all counts in the hash table and internal queues
	happly(hp, sumwordsHash); //call sumwordsHash for each word in the hashtable
	
	printf("*** %d words ***\n", sumGlobal);
	
	indexsave(hp,"indexName"); //only need hash table and name to create index file
	
	happly(hp, freeWord); //Use freeWord function to delete all the word strings in the hashtable
	hclose(hp);
	//fclose(fp);	
	
	exit(EXIT_SUCCESS);
}
