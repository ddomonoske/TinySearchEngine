/* 
 * indexio.c --- save and load an index to a file indexnm
 * 
 * Author: Eric Chen
 * Created: Tues Oct 27
 * Version: 1.0
 * 
 * Description: 
 */

#include "indexio.h"

#define MAX_WORD_LENGTH 50

static FILE *fp;


/* used to make the incrementing file names
 */
char* itoa(char *a, int i) {
	sprintf(a, "%d", i);
	return a;
}


/*
 * saveCount
 */
static void saveCount(void *elementp) {
	counters_t *doc = (counters_t*)elementp;
	
	if (elementp != NULL) { //check inputs
		fprintf(fp," %d %d",doc->id, doc->count);
	}
	else printf("counter_t in queue is null");
}


/*
 * saveLine
 */
static void saveLine(void *elementp) {

	if ( (elementp != NULL) && (fp != NULL) ) { //check inputs
		wc_t *uniqueword = (wc_t*)elementp; 

		fprintf(fp,"%s",uniqueword->word);
		qapply(uniqueword->qp,saveCount);	
		fprintf(fp,"\n");
	}
	else printf("File is NULL or wc_t in hashtable is null");	
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


/*
 * free all the words in the hash table
 *   - used with happly
 *   - necessary because wcp->word and wcp->qp must be freed before wcp
 */
void freeWord(void* elementp) {
	wc_t *wcp = (wc_t*)elementp;
	free(wcp->word);
	qclose(wcp->qp); //step 4
}


/*
 * indexsave
 */
void indexsave(hashtable_t *hp, char *indexnm) {

	//include NULL checks
	if ( (hp == NULL) || (indexnm == NULL) ) {
		printf("hashtable is empty or indexnm is NULL");
	}
	else {
		//chdir("../indexer");  // move from crawler directory into indexer directory
		fp = fopen(indexnm,"w"); 

		happly(hp,saveLine); //happly a function that will print line for each unique word in hp into file
		fclose(fp);
	}
}



/* 
 * indexload 
 */ 
hashtable_t* indexload(char *filename) {
	hashtable_t *hp;
	char *word;      // word for word counter
	queue_t *qp;     // queue for word counter
	int id, count;   // doc id and count for queue

	int i;       // track whether current doc is first doc for each word
	int status;  // store return value of fscanf()
	
	FILE *fp;
	
	if (filename == NULL) {
		printf("Error: filename is NULL.\n");
		return NULL;
	}

	fp = fopen(filename, "r");
	if (fp == NULL) {
		printf("Error: file does not exist\n");
		return NULL;
	}
	
	hp = hopen(HTABLE_SIZE);
	word = (char*)malloc(sizeof(char)*(MAX_WORD_LENGTH+1));

	do {
		fscanf(fp, "%50s", word);  // get word, no longer than 50 characters
		i = 0;
		
		// loop until end of line. EOF determined by failing to match two integers
		while ((status = fscanf(fp, "%d %d", &id, &count)) == 2) {
			if (i==0) {  // if first id and count for word
				qp = qopen();   // qp is new object if new word, otherwise it stays the same
				hput(hp, make_wc(word, qp), word, strlen(word));  // place new wc_t in table
			}
			qput(qp, make_doc(id,count));  // add new doc to queue
			i++;
		}
	} while (status != EOF);
	
	fclose(fp);
	free(word);
	
	return(hp);
}
