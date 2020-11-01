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
	
	for (i=0; i<n; i++) {
		if (isalpha(word[i]) == 0) { //returns false if non-alphabetic characters
			printf("returning false\n");
			return false;
		}
		word[i] = tolower(word[i]); //converts all alphabetic characters to lowercase
	}
	return true;
	printf("returning true\n");

}


int main (void) {

	char input[200]; //to store unparsed user input
	char *words[10]; //array of words to store input (max input = 10 words)
	char *token;
	
	char delimits[] = " \t"; //set delimiters as space and tab
	int i = 0;
	
	printf(">");	

	//note: use fgets, not scanf, in order to read entire line. 
	while (fgets(input, MAX, stdin) != NULL) { //loops until user enters EOF (ctrl+d)
	
		printf("The input is: %s\n",input);
 		token = strtok(input,delimits); //strtok() splits input according to delimits and returns next token
 		
 		//if (NormalizeWord(token)) { //STILL NEED TO CHECK THAT QUERY WORDS CONTAIN NO NON-ALPHA CHARACTERS
 			words[i] = token;
 		//}
  
    	while (words[i] != NULL) { //continue splitting input until strktok returns NULL (no more tokens)
    		printf("%s ",words[i]);
    		i++;
    		words[i] = strtok(NULL,delimits); //stores all words into words array
    	}
		
		//else {
		//	printf("[invalid query]\n"); //reject queries containing non-alphabetic characters. Nothing stored
		//}
		
    	printf(">");
    } 


	
	/*	
	if (NormalizeWord(input)) {
		printf("converted to lower\n");
		printf("%s\n",input);	
	}
	
	else {
		
		printf("%s\n",input);
		printf("[invalid query]\n");
	}
	*/
	
	return(0);

}

