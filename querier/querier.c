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

#define MAX_INPUT 200
#define MAX_QUERY_WORD 20

/* object to store the start and length of a substring */
typedef struct substring {
	uint32_t b;
	uint32_t l;
} substring_t;

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


/* 
 * combine two queues together into mainqp through OR operation
 *  - if a queue is in either list it will end up in the final one
 *  - if a queue is in a single list the larger of the two ranks is taken
 */
void combine(queue_t *mainqp, queue_t *newqp) {
	counters_t *curr;

	while ((curr = qget(newqp)) != NULL) { //loop through each doc element in newqp
		counters_t *found_doc;
		
		if ((found_doc = qsearch(mainqp, DocSearch, &curr->id)) == NULL){ //if doc element does not exist in mainqp 
																		   //(implies doc does not have all query words)
			qput(mainqp,curr); //delete doc element from sortqp
		}
		else {
			found_doc->count += curr->count;
			free(curr);
		}
	}
	qclose(newqp);
}


void update(queue_t *sortqp, queue_t *indexqp) {
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
	qconcat(sortqp, backupq);
	//qclose(backupq);
	//qclose(sortqp);
	//sortqp = backupq;
	
}


void ranking (char **words, hashtable_t *hp, int wc, queue_t *sortqp) {
	wc_t *wc_found;

	
	//need to check if first word is valid

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
		qclose(wc_found->qp);
		wc_found->qp = backupq;	
		
		
		for (int i=1; i<wc; i++){ //loop through the rest of the queried words
			char *curr_word = words[i];
			if((strcmp(curr_word,"and")!=0) && (strlen(curr_word)>=3)){ //filter out ands
			
				//need to check if word is "or" (fork here based on if it is or not)
				//need to also check if or is the last word
				//need to also check if you have "and or", "and and", etc. 
				
			
				wc_t *found_word;
				if ((found_word = hsearch(hp, fn, curr_word, strlen(curr_word))) != NULL) { //search for remaining query words in hashtable index
					//					printf("about to add THIS QUEUE to sortqp:\n");
					//qapply(found_word->qp,printQueue);
					update(sortqp, found_word->qp); //if found, need to update sortqp
					printf("word found \n");
				}
				else {
					printf("word is not in the document \n");
					update(sortqp, NULL);
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


/* check if a string is "and" or "or"
 * used as shorthand inside validateQuery
 */
bool isAO(char *s) {
	return ( (strcmp(s,"and")==0) || (strcmp(s,"or")==0) );
}

/* validate that AND and OR have been used correctly in the search queue
 *  - cannot begin or end with AND/OR
 *  - cannot have two AND/ORs in a row
 */
bool validateQuery(char *words[],int wc) {
	bool prev,curr;
	
	if (isAO(words[0]) || isAO(words[wc-1]))
		return false;

	prev = false;
	for (int i=1; i<wc-1; i++) {
		curr = isAO(words[i]);
		if (curr && prev) return false;
		prev = curr;
	}
	return true;
}


int main (void) {

	char input[MAX_INPUT+1]; //to store unparsed user input
	char *words[MAX_QUERY_WORD];
	
	char delimits[] = " \t\n"; //set delimiters as space and tab
	int wc; //to store and print words from words array

	int or_count=0;
	substring_t or_subs[MAX_QUERY_WORD];  // count of instances of "or" in words[]
	
	hashtable_t *hp; //to store index hashtable
	queue_t *sortqp; //queue to store docs containing ALL words in query
	
	int qsize;
	
	printf(" > ");

	hp = indexload("indexForQuery2"); //creates index hashtable from indexed file

	//note: use fgets, not scanf, in order to read entire line 
	while (fgets(input, MAX_INPUT, stdin) != NULL) { //loops until user enters EOF (ctrl+d)
	
		if (strlen(input)<=1){
			printf(" > ");
			continue;
		}
		
		input[strlen(input)-1] = '\0';
		
		if (NormalizeWord(input)) { //input can only contain alphabetic-characters and white space
    		
    		// *** STEP 1 ***
			wc=0; //rewrite words array for each new input 
			words[0] = strtok(input,delimits); //strtok() splits input according to delimits and returns next token
			
			while ((words[wc] != NULL) && (wc<20)) { //stores all input words into words array
				//printf("%s ",words[i]);
				wc++;
				words[wc] = strtok(NULL,delimits); //continue splitting input until strktok returns NULL (no more tokens)
			}

			if (validateQuery(words,wc)) {
			
				// remove all instances of "and" in words[]
				int i=0;
				for (int j=0; j<wc; j++) {
					if ( strcmp(words[j],"and")!=0 ) {
						words[i++] = words[j];
					}
				}
				wc=i;
				
				// track beginning location and length of each substring separated by "or"
				i=0;
				or_count=0;
				or_subs[0].b = 0; // first address begins at 0
				for (int j=0; j<wc; j++) {
					if ( strcmp(words[j],"or")==0 ) {
						or_subs[or_count++].l=i; // i is the length of last substring
						or_subs[or_count].b=j+1; // j+1 is the address of the next substring
						i=0;
					} else i++;
				}
				or_subs[or_count].l=i;
				or_count++;
				
				printf("or_count: %d\n",or_count);
				for (int j=0; j<or_count; j++) {
					printf("%d %d\n",or_subs[j].b, or_subs[j].l);
				}
				printf("\n\n");
				
				//  *** STEP 2 (RANKING) ***
				//hp = indexload("indexForQuery2"); //creates index hashtable from indexed file
				
				
				sortqp = qopen();
				i=0;
				do {
					printf("i: %d\n", i);
					if (or_subs[i].l > 0) {
						queue_t *newqp = qopen();
						ranking(&(words[or_subs[i].b]), hp, or_subs[i].l, newqp); //add all qualifying docs into sortqp with correct rank
						combine(sortqp,newqp);
					}
					i++;
				} while ( i<or_count );
				
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
				qclose(sortqp);
				sortqp = backupq;
				
				
				
				if(qsize > 1){
					//make array that we can use to sort
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
				}
				
				char cwd[MAX_PATH_LENGTH];
				char *pagedir = "../pages2";
				getcwd(cwd, sizeof(cwd));
				chdir(pagedir);
				
				counters_t *tmpForPrint;
				while ((tmpForPrint = qget(sortqp)) != NULL){
					printf("rank:%d doc:%d : ", tmpForPrint->count, tmpForPrint->id);
					
					char doc_id[10];
					if (access(itoa(doc_id, tmpForPrint->id), F_OK) != -1) {
						FILE *fp = fopen(doc_id, "r");
						char url[MAX_PATH_LENGTH];
						fgets(url, MAX_PATH_LENGTH, fp);
						
						printf("%s", url);
						
						fclose(fp);
						
					} else {
						printf("ERROR- no doc found\n");
					}
					free(tmpForPrint);
				}	
				
				chdir(cwd);
				
				qclose(sortqp);
				
			}
			else printf("[invalid query]"); //reject queries containing improper AND/OR usage
		} // end of valid query search
		else printf("[invalid query]"); //reject queries containing non-alphabetic/non-whitespace characters
		
		printf("\n > ");
		
		//SEG FAULT WHEN INVALID QUERY	
	}
	
	happly(hp, freeWord);
	hclose(hp);
	
	
	printf("\n"); //add new line after user terminates with ctrl+d
	exit(EXIT_SUCCESS);
	
}

