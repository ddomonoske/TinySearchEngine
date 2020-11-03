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

/*
void rank_fn(void *p){
	char *unique;
	int count;
	
	unique = p->word;
	count = p->count
	
	printf("rank: 
}
*/



/* look in queue for element with matching doc ID
 */
bool QueueSearchFn(void* elementp, const void* docID) { //requires (void* elementp, const void* keyp)
	counters_t *doc = (counters_t*)elementp;
	int *id = (int*)docID;
	return (doc->id == *id); //if match return true
}


int main (void) {

	char input[200]; //to store unparsed user input
	char *words[10]; //array to store input (max input = 10 words)
	
	char delimits[] = " \t\n"; //set delimiters as space and tab
	int i,j,wc; //to store and print words from words array
	
	hashtable_t *hp; //to store index hashtable
	wc_t *wc_found; //to store wc_t objects with *words that match query words
	///int id = 1;
	
	//NOTE: each mention of doc refers to counter_t (docID, count) object
	queue_t *sortqp; //queue to store docs containing ALL words in query
	queue_t *indexqp; //queue associated with each word in the index, contains docs
	counters_t *doc; //to hold each doc from the indexqp (might make more sense to name this docIndex)
	counters_t *docSort; //to hold each doc from the sortqp
	
	counters_t *docMatchInSort; //to hold matching doc if sortqp already contains the doc
	counters_t *docMatchInIndex; //to hold matching doc if indexqp contains the doc
	
	int flag = 0;
	
	
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
			
			for(j = 0; j<wc; j++) { //print words in words array
				printf("%s ", words[j]);
			}
			printf("\n\n");
			
			//*** STEP 2 ***
			hp = indexload("indexForQuery"); //creates index hashtable from indexed file
			
			//*** STEP 3 ***
			sortqp = qopen(); //create sortQueue

			
			for(i=0; i<wc; i++) { //for each query word
				
				if((strcmp(words[i],"and")!=0) && (strcmp(words[i],"or")!=0) && (strlen(words[i])>=3)){ //filters our and, or, and len<3 words
					
					if ((wc_found = hsearch(hp, fn, words[i], strlen(words[i]))) != NULL) { //if word found in index
						
						printf("\nword '%s' found in hastable index\n",wc_found->word);
						//*** STEP 2 ***
						//doc = qsearch(wc_found->qp, QueueSearchFn, &id);
						//printf("%s:%d ", wc_found->word, doc->count);
						
						
						//*** STEP 3 ***
						/* Antony
						- Create sortQueue
						- for first query word, add all docs and count (counter_t objects) to a sortQueue 
						- for next query word, get each doc from in indexQueue associated with the word
							- If docID already exists in sortQueue, only update sortQueue if new docID->count is less than current docID->count 
							- If docID does NOT exist in sortQueue, don't add to sortQueue(bc it means the doc didn't contain first query word, and thus doesn't satisfy ALL query words) 
							- For each doc in sortqp, only keep if also appears in indexqp. If not appear in indexqp, I remove from sortqp
	
						Queues required
						- indexQueue - queue from hashtable associated with each matching query word
						- sortQueue - to store docs as we go through each indexQueue
							-> for each new query word (after the first query), qget from this sortQueue, compare with docs in indexQueue 
							-> If appears in indexQueue, then all good, and do nothing (OR add to backupQueue/)
							-> If does not appear, means that docID does not contain ALL query words, so remove from sortQueue (don't put into backupQueue??)
						- backupQueue?? - queue that is eventually turned into real queue (dont really understand this queue)

						*/
						
						//NOTE: each mention of doc refers to counter_t object
						
						indexqp = wc_found->qp; //returns the indexQueue of docs for the matching word
						
						while ((doc = qget(indexqp)) != NULL) { //get each doc from indexqp						
							printf("indexqp not empty yet\n");
							
							if (flag==0) { //if first query word
								printf("First query word. Adding all docs into sortqp\n");
								qput(sortqp,doc); //put each doc into sortqp
								flag=1;
							}
							
							else { //if NOT first query word, only put select docs into sortqp, and also need to remove some docs from sortqp
								printf("not first query word\n");
								//PART 1: Update count for each doc in sortqp if it appears in indexqp
								if((docMatchInSort = qsearch(sortqp, QueueSearchFn, &(doc->id))) != NULL) { //if doc already exists in sortqp				
									printf("docID already exists in sortqp\n");
									if(doc->count < docMatchInSort->count) { 
										printf("updated docID count\n");
										docMatchInSort->count = doc->count; //update count of doc IF the frequency of new word is lower
									}
								} //if doc does NOT exist in sortqp, then we can ignore it
								
								//PART 2: For each doc in sortqp, only keep if also appears in indexqp
								while ((docSort = qget(sortqp)) != NULL) { //for each doc already in sortqp, check to make sure it also appears in current indexqp
									printf("retrieving doc from sortqp\n");
									if((docMatchInIndex = qsearch(indexqp, QueueSearchFn, &(docSort->id))) == NULL) { //if docSort NOT found in indexqp, 
										//do nothing bc get already removed doc from sortqp
										printf("removing doc from sortqp bc doesnt appear in indexqp\n");
									}
									else {
										qput(sortqp,docSort); //if docSort IS found in indexqp, then put back into sortqp
										printf("sortqp appears in indexqp, putting back into sortqp\n");
									}
								}
								
								//NOTE: Is there a way to combine PART 1 and PART 2 to increase run time? Or are they two very separate steps?
								
							} //end of sortqp updating
						
						} //end of while loop for a specific indexqp
						qclose(indexqp); 
					}
				}
				
			} //end of query word, move onto next query word
			qclose(sortqp);
		
		} // end of valid query search
		else printf("[invalid query]"); //reject queries containing non-alphabetic/non-whitespace characters
			
		printf("\n > ");
	}
		
	printf("\n"); //add new line after user terminates with ctrl+d
	return(0);
	
}

