/* indexer_test.c --- 
 * 
 * 
 * Author: David W. Domonoske
 * Created: Wed Oct 28 14:24:14 2020 (-0400)
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

int main(int argc, char *argv[] ) {
	hashtable_t *hp;

/*	
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
*/  
	hp = indexload("indexName");
	
	indexsave(hp,"indexName2"); //only need hash table and name to create index file
	
	happly(hp, freeWord); //Use freeWord function to delete all the word strings in the hashtable
	hclose(hp);
		
	exit(EXIT_SUCCESS);
}
