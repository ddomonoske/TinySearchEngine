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



int main (void) {

	char input[200]; //to store unparsed user input
	char *words[10]; //array to store input (max input = 10 words)
	
	char delimits[] = " \t\n"; //set delimiters as space and tab
	int i,j; //to store and print words from words array
	
	printf(" > ");	

	//note: use fgets, not scanf, in order to read entire line 
	while (fgets(input, MAX, stdin) != NULL) { //loops until user enters EOF (ctrl+d)
	
		if (NormalizeWord(input)) { //input can only contain alphabetic-characters and white space
    		
			i=0; //rewrite words array for each new input 
			words[i] = strtok(input,delimits); //strtok() splits input according to delimits and returns next token
			
			while (words[i] != NULL) { //stores all input words into words array
				//printf("%s ",words[i]); 
				i++;
				words[i] = strtok(NULL,delimits); //continue splitting input until strktok returns NULL (no more tokens)
			}
			
			for(j = 0; j<i; j++) { //print words in words array
				printf("%s ", words[j]);
			}
		}
		else printf("[invalid query]"); //reject queries containing non-alphabetic/non-whitespace characters
		
		printf("\n > ");
	}
	
	printf("\n"); //add new line after user terminates with ctrl+d
	return(0);
	
}

