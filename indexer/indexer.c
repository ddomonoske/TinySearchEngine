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
//#include <unistd.h>
#include <sys/stat.h>
#include "pageio.h" //do we need any of these?
#include "webpage.h"
#include "queue.h"
#include "hash.h"


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

int main(void) {
	webpage_t *page;
	char *word;
	int pos = 0;
	FILE *fp;
	page = pageload(1,"../pages");

	chdir("../indexer");
	fp = fopen("output1","w");
		
	while ((pos = webpage_getNextWord(page, pos, &word)) > 0) { //go through webpage looking for embedded URLs
		if (NormalizeWord(word) == true)
			fprintf(fp, "%s\n", word);
	}

	fclose(fp);
	
	return 0;
}
