/* querier.c --- querier part of tse
 * 
 * 
 * Author: Eric Chen
 * Created: Sun Nov  1 11:42:00 2020 (-0500)
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
#include <limits.h>
#include "pageio.h" 
#include "webpage.h"
#include "queue.h"
#include "hash.h"
#include "indexio.h"

#define MAX 200


/* checks for non-alphabetic characters
 *   - if non-alphabetic character found, returns false
 *   - otherwise, converts all characters to lowercase, and return true
 */
bool NormalizeWord(char *word) {

	int i, n = strlen(word);
	char c;
	
	for (i=0; i<n; i++) {
		c = word[i];
		if ((isalpha(c) == 0) && (isspace(c) == 0)){ //returns false if non-alphabetic character AND not white space
			//printf("returning false\n");
			return false;
		}
		word[i] = tolower(c); //converts all alphabetic characters to lowercase
	}
	//printf("returning true\n"); 
	return true;
}


/* fn() compares words[i] with indexed words
 * returns bool
 */
bool fn(void* elementp, const void* word) {
	wc_t *ep = (wc_t*)elementp;
	char *wp = (char*)word;
	if (strcmp(ep->word,wp) == 0)  // true if the word is a match
		return true;
	else
		return false;
}


/* look in queue for element with matching doc ID
 */
bool DocSearch(void* elementp, const void* docID) { //requires (void* elementp, const void* keyp)
	counters_t *doc = (counters_t*)elementp;
	int *id = (int*)docID;
	return (doc->id == *id); //if match return true
}


void update (queue_t *sortqp, queue_t *indexqp){
	queue_t *backupq = qopen();
	counters_t *curr;
	
	while ((curr = qget(sortqp)) != NULL) { //loop through each doc element in sortqp
		counters_t *found_doc;
		
		if ((found_doc = qsearch(indexqp, DocSearch, &curr->id)) == NULL){ //if doc element does not exist in indexqp 
																		   //(implies doc does not have all query words)
			free(curr); //delete doc element from sortqp
		}
		else {
			if (curr->count > found_doc->count){ //if count of new query word is less than current doc rank
				curr->count = found_doc->count; //update the rank
			}
			qput(backupq, curr); //make copy of curr in backup (because curr was qget from sortqp)
		}
	}
	qconcat(sortqp, backupq); //re add all qualifying docs into sortqp
	
}


void ranking (char **words, hashtable_t *hp, int wc, queue_t *sortqp) {
	wc_t *wc_found;

	if ((wc_found = hsearch(hp, fn, words[0], strlen(words[0]))) != NULL) { //if first word found in the hashtable index
		//if first word not found, the rest should not run
		queue_t *backupq = qopen();
		counters_t *curr;
		
		while ((curr = qget(wc_found->qp)) != NULL) { //loop through each doc element in first word queue
			counters_t *doc = malloc(sizeof(counters_t)); //duplicate curr object
			doc->count = curr->count;
			doc->id = curr->id;
			
			qput(sortqp, doc); //add duplicate to sortqp
			qput(backupq, curr); //add original to backupqp
		
		}
		qconcat(wc_found->qp, backupq); //concatenate backup to wc_found so wc_found is like how it started
		
		for (int i=1; i<wc; i++){ //loop through the rest of the queried words
			char *curr_word = words[i];
			if((strcmp(curr_word,"and")!=0) && (strlen(curr_word)>=3)){ //filter out ands
				wc_t *found_word;
				if ((found_word = hsearch(hp, fn, curr_word, strlen(curr_word))) != NULL) { //search for remaining query words in hashtable index
					update(sortqp, found_word->qp); //if found, need to update sortqp
				}
				else {
					printf("word is not in the document \n");
					return;
				}
			}
		}
	}
}


void swap(counters_t** xp, counters_t** yp) {
	counters_t* tmp = *xp;
	*xp = *yp;
	*yp = tmp;
}	
			
			
	
void sortArray (counters_t* qarray[], int qsize){
	int max_index;
		
	for (int i=0; i<qsize-1; i++){
		
		max_index = i;
		for (int j=i+1; j<qsize; j++){
			if (qarray[j]->count > qarray[max_index]->count){
				max_index = j;
			}
		}
		swap(&qarray[max_index], &qarray[i]);
	}
}


int main (void) {

	char input[200]; //to store unparsed user input
	char **words = malloc(20*sizeof(char*)); //array to store input (max input = 10 words)
	
	char delimits[] = " \t\n"; //set delimiters as space and tab
	int wc; //to store and print words from words array
	
	hashtable_t *hp; //to store index hashtable
	queue_t *sortqp; //queue to store docs containing ALL words in query
	
	int qsize;
	
	printf(" > ");	

	//note: use fgets, not scanf, in order to read entire line 
	while (fgets(input, MAX, stdin) != NULL) { //loops until user enters EOF (ctrl+d)
	
		if (NormalizeWord(input)) { //input can only contain alphabetic-characters and white space
    		
    		//*** STEP 1 ***
			wc=0; //rewrite words array for each new input 
			words[wc] = strtok(input,delimits); //strtok() splits input according to delimits and returns next token
			
			while (words[wc] != NULL) { //stores all input words into words array
				//printf("%s ",words[i]); 
				wc++;
				words[wc] = strtok(NULL,delimits); //continue splitting input until strktok returns NULL (no more tokens)
			}
			
			for(int j=0; j<wc; j++) { //print words in words array
				printf("%s ", words[j]);
			}
			printf("\n\n");
			
			//  *** STEP 2 (RANKING) ***
			hp = indexload("indexForQuery2"); //creates index hashtable from indexed file
			
			
			sortqp = qopen();
			ranking(words, hp, wc, sortqp); //add all qualifying docs into sortqp with correct rank
			
			
			//  *** SORTING STEP ***

			//Check for qsize
			qsize = 0;		
			queue_t *backupq = qopen();
			counters_t *curr;
			while ((curr = qget(sortqp)) != NULL){
				printf("checking sortqp: id = %d, rank = %d \n", curr->id, curr->count);
				qput(backupq, curr); //add original to backupqp
				qsize++;
			}	
			qconcat(sortqp, backupq);	
			
			
			//Then we can sort the array with bubble sort and recreate the queue with qput for printing
			counters_t* qarray[qsize];
			for (int i=0; i<qsize; i++){
				qarray[i] = qget(sortqp); //copy all of the sortqp elements into qarray
				qput(sortqp,qarray[i]);
			}
			
			sortArray(qarray, qsize); //sort qarray
			
			for (int k=0; k<qsize; k++){
				qput(sortqp, qarray[k]); //put all the qarray elements into the back of sortqp
				qget(sortqp); //remove the original unsorted elements in sortqp
			}

			counters_t *tmpForPrint;
			while ((tmpForPrint = qget(sortqp)) != NULL){
				printf("rank:%d doc:%d \n", tmpForPrint->count, tmpForPrint->id);
			}	
			
			qclose(sortqp);
			
		
			
		} // end of valid query search
		else printf("[invalid query]"); //reject queries containing non-alphabetic/non-whitespace characters
			
		printf("\n > ");
		happly(hp, freeWord);
		hclose(hp);
	}
		
	printf("\n"); //add new line after user terminates with ctrl+d
	exit(EXIT_SUCCESS);
	
}

