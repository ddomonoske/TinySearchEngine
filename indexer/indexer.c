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

typedef struct wc {
	char *word;
	uint32_t count;
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
wc_t* make_wc(char *word, uint32_t count) {
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
	wcp->count = count;
	return wcp;
}


/* free all the words in the hash table
 *   -used with happly
 */
void freeWord(void* elementp) {
	wc_t *wcp = (wc_t*)elementp;
	free(wcp->word);
}


/* add the count of words in each element to the sum (should be 141)
 *   -used with happly
 */
void sumWords(void* elementp) {
	wc_t *wcp = (wc_t*)elementp;
	sumGlobal += wcp->count;
}


int main(void) {
	webpage_t *page;
	char *word;
	int pos = 0;
	
	//FILE *fp;
	hashtable_t *hp = hopen(HTABLE_SIZE);  // Step 3
	wc_t *wc_found;

	page = pageload(1,"../pages");

	//chdir("../indexer");  // for Step 2
	//fp = fopen("output1","w"); // for Step 2
		
	while ((pos = webpage_getNextWord(page, pos, &word)) > 0) { //go through every word in html
		if (NormalizeWord(word) == true) {  // only add to hash table if normalized
			if ((wc_found = hsearch(hp, wordmatch, word, strlen(word))) == NULL) {
				printf("adding new word to hash table\n");
				hput(hp, make_wc(word,1), word, strlen(word));
			} else {
				printf("increasing the count for '%s' by 1\n", word);
				(wc_found->count)++;
			}
			//fprintf(fp, "%s\n", word);  // remnant from Step 2
		}
		free(word); 
	}
	
	//Calculate sum of all counts in the hash table
	happly(hp, sumWords);
	printf("*** %d words ***\n", sumGlobal);
	
	
	webpage_delete(page);
	
	happly(hp, freeWord);
	hclose(hp);
	//fclose(fp);
	
	return 0;
}
