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


int minIndex(queue_t *sortqp, int sortedIndex, int qsize){
	int min_index = -1;
	int minID = INT_MAX;
	counters_t *qfront;
				
	for (int i=0; i<qsize; i++){
		qfront = qget(sortqp);
					
		if ((qfront->id <= minID) && (i <= sortedIndex)){
			min_index = i;
			minID = qfront->id;
		}
		qput(sortqp, qfront);		
	}
	return min_index;
}

			
void insertMinToRear (queue_t *sortqp, int min_index, int qsize){
	counters_t *min;
	counters_t *qfront;
				
	for (int i=0; i<qsize; i++){
		qfront = qget(sortqp);
					
		if (i != min_index)
			qput(sortqp, qfront);
		else
			min = qfront;
	}			
	qput(sortqp, min);
}


void update (queue_t *sortqp, queue_t *check){
	queue_t *backupq = qopen();
	counters_t *curr;
	
	while ((curr = qget(sortqp)) != NULL) { 
		counters_t *found_doc;
		
		if ((found_doc = qsearch(check, DocSearch, &curr->id)) == NULL){
			free(curr);
		}
		else {
			if (curr->count > found_doc->count){
				curr->count = found_doc->count;
			}
			
			qput(backupq, curr);
		}
	}
	qconcat(sortqp, backupq);
}


void ranking (char **words, hashtable_t *hp, int wc, queue_t *sortqp) {
	wc_t *wc_found;

	if ((wc_found = hsearch(hp, fn, words[0], strlen(words[0]))) != NULL) { //if word found in index
		queue_t *backupq = qopen();
		counters_t *curr;
		
		while ((curr = qget(wc_found->qp)) != NULL) { 
			counters_t *doc = malloc(sizeof(counters_t));
			doc->count = curr->count;
			doc->id = curr->id;
			
			qput(sortqp, doc);
			qput(backupq, curr);
		
		}
		qconcat(wc_found->qp, backupq); //concatenate backup to wc_found so wc_found is like how it started
		
		for (int i=1; i<wc; i++){
			char *curr_word = words[i];
			if((strcmp(curr_word,"and")!=0) && (strlen(curr_word)>=3)){
				wc_t *found_word;
				if ((found_word = hsearch(hp, fn, curr_word, strlen(curr_word))) != NULL) { 	
					update(sortqp, found_word->qp);
				}
				else {
					printf("word is not in the document \n");
					return;
				}
			}
		}
	}
}


int main (void) {

	char input[200]; //to store unparsed user input
	char **words; //array to store input (max input = 10 words)
	
	char delimits[] = " \t\n"; //set delimiters as space and tab
	int wc; //to store and print words from words array
	
	hashtable_t *hp; //to store index hashtable
	queue_t *sortqp; //queue to store docs containing ALL words in query
	
	int qsize, min_index;
	counters_t *tmpForSize;
	counters_t *tmpForPrint;
	
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
			
			for(int j = 0; j<wc; j++) { //print words in words array
				printf("%s ", words[j]);
			}
			printf("\n\n");
			
			//*** STEP 2 ***
			hp = indexload("indexForQuery"); //creates index hashtable from indexed file
			
			
			sortqp = qopen();
			ranking(words, hp, wc, sortqp);
				
			qsize = 0;		
			while ((tmpForSize = qget(sortqp)) != NULL){
				qsize++;
				qput(sortqp,tmpForSize);
			}		

			for(int k=0; k<qsize; k++){
				min_index = minIndex(sortqp, qsize-k, qsize);
				insertMinToRear(sortqp, min_index, qsize);
			}
			
			while ((tmpForPrint = qget(sortqp)) != NULL){
				printf("rank:%d doc:%d : \n", tmpForPrint->count, tmpForPrint->id);
			}
			
			//insert processing of sortqp
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

