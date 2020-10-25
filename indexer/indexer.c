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
#include "pageio.h" //do we need any of these?
#include "webpage.h"
#include "queue.h"
#include "hash.h"

#define HTABLE_SIZE 128

int sumGlobal = 0;

/* the representation of a doc for the queue (step 4) */
typedef struct counters {
	int id;
	uint32_t count;
} counters_t;


typedef struct wc {
	char *word;
	queue_t *qp;
} wc_t;


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


/* make a wc_t object
 *   - malloc space for object and for word
 */
wc_t* make_wc(char *word, counters_t *qp) {
	wc_t *wcp;
	uint32_t wordLength;
	if ((wcp = (wc_t*)malloc(sizeof(wc_t))) == NULL) { 	//check if enough space in memory for car_t*
		printf("Error: malloc failed allocating wc object\n");
		return NULL;
	}

	wordLength = strlen(word);
	if ((wcp->word = (char*)malloc((wordLength+1)*sizeof(char))) == NULL) { 	//check if enough space in memory for wc_t*
		printf("Error: malloc failed allocating word\n");
		return NULL;
	}
	strcpy(wcp->word, word);
	wcp->qp = qp;
	return wcp;
}


/* free all the words in the hash table
 *   -used with happly
 */
void freeWord(void* elementp) {
	wc_t *wcp = (wc_t*)elementp;
	free(wcp->word);
	qclose(wcp->qp); //step 4
}

/* add the count of words in each queue element to the sum (should be 141)
 *   -used with qapply for step4
 */
void sumwordsQueue(void* elementp) {
	counters_t *docp = (counters_t*)elementp;
	sumGlobal += docp->count;
}

/* call sumwordsQueue for each queue in the hashtable
 */
void sumwordsHash(void* elementp) {
	wc_t *wcp = (wc_t*)elementp;
	qapply(wcp->qp, sumwordsQueue);
}


/* make_doc(): creates a doc object for queue(step 4) */
counters_t* make_doc(int id, uint32_t count) {
	counters_t *doc;
	if ((doc = (counters_t*)malloc(sizeof(counters_t))) == NULL) { //check if enough space in memory for counters_t*
		printf("Error: malloc failed allocating car\n");
		return NULL;
	}
	doc->id = id;
	doc->count = count;
	return doc;
}

/* look in queue for element with matching doc ID
 */
bool QueueSearchFn(void* elementp, const void* docID) { //requires (void* elementp, const void* keyp)
	counters_t *doc = (counters_t*)elementp;
	int *id = (int*)docID;
	return (doc->id == *id); //if match return true
}


int main(void) {
	webpage_t *page;
	char *word;
	int pos = 0;
	int id = 1;
	
	//FILE *fp;
	hashtable_t *hp = hopen(HTABLE_SIZE);  // Step 3
	wc_t *wc_found;
	queue_t *qp;
	counters_t *doc_found;

	page = pageload(id,"../pages");

	//chdir("../indexer");  // for Step 2
	//fp = fopen("output1","w"); // for Step 2
		
	while ((pos = webpage_getNextWord(page, pos, &word)) > 0) { //go through every word in html
		if (NormalizeWord(word)) {  // only add to hash table if normalized
			if ((wc_found = hsearch(hp, wordmatch, word, strlen(word))) == NULL) {
				printf("adding new word to hash table\n");
				qp = qopen(); //make new queue
				qput(qp, make_doc(id,1)); //place doc element in queue
				hput(hp, make_wc(word, qp), word, strlen(word)); //put queue in word element in hashtable
				
			} else {
				printf("increasing the count for '%s' by 1\n", word);
				
				//look for element in queue with matching doc ID
				doc_found = qsearch(wc_found->qp, QueueSearchFn, &id); 
				if((doc_found) != NULL){
					(doc_found->count)++; //add to count inside doc element in queue		
				}	
				else { //if no element in the queue, make a new doc element
					qp = qopen();
					qput(qp, make_doc(id,1));  //place doc in queue
					hput(hp, make_wc(word, qp), word, strlen(word)); //place queue in hashtable
				}
			}
			//fprintf(fp, "%s\n", word);  // remnant from Step 2
		}
		free(word); 
	}
	
	
	//Step 4: Calculate sum of all counts in the hash table and internal queues
	happly(hp, sumwordsHash);
	printf("*** %d words ***\n", sumGlobal);

	
	webpage_delete(page);
	
	happly(hp, freeWord);
	hclose(hp);
	//fclose(fp);
	
	return 0;
}
