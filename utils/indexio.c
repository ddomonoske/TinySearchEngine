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


static FILE *fp;


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


/*
 * indexsave
 */
void indexsave(hashtable_t *hp, char *indexnm) {

	//include NULL checks
	if ( (hp == NULL) || (indexnm == NULL) ) {
		printf("hashtable is empty or indexnm is NULL");
	}
	else {
		chdir("../indexer");  // move from crawler directory into indexer directory
		fp = fopen(indexnm,"w"); 

		happly(hp,saveLine); //happly a function that will print line for each unique word in hp into file
		fclose(fp);
	}

}



/* 
 * indexload 
 */
 
void indexload(char *filename, char *dirnm);






