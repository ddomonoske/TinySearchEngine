/* pageio.c-- implementation of pageio.h
 *
 * Author: Eric Chen
 * Created: Sat, Oct 24
 * Version:
 *
 * Description:
*/ 

#include "pageio.h"


/* to check if the directory under string beginning at *dir exists
 * returns true if it does, false if not
 * used for STEP 6
 */
bool isDirExist(char *dir) {
	stat_t s;
	bool b;
	
	if(!stat(dir,&s)) {
		if (S_ISDIR(s.st_mode))
			b =  true;
		else
			b = false;
	} else
		b = false;
	
	return b;
}


 /*
 * pagesave -- save the webpage in filename id in directory dirname
 *
 * returns: 0 for success; nonzero otherwise
 *
 * The suggested format for the file is:
 *   <url>
 *   <depth>
 *   <html-length>
 *   <html>
 */
int32_t pagesave(webpage_t *pagep, int id, char *dirname) {
	
	FILE *fp;
	char filename[10];

	sprintf(filename, "%d", id); //convert int to string
	
	chdir(dirname); //changes over to right directory
	
	fp = fopen (filename,"w");
	fprintf(fp,"%s\n%d\n%d\n%s", webpage_getURL(pagep), webpage_getDepth(pagep),
					webpage_getHTMLlen(pagep), webpage_getHTML(pagep)); //prints URL, depth, HTML length, and HTML into file
	printf("pagesave: %s\n", webpage_getURL(pagep));

	fclose(fp);
	return 0; //will always return 0
}


/* 
 * pageload -- loads the numbered filename <id> in directory <dirnm>
 * into a new webpage
 *
 * returns: non-NULL for success; NULL otherwise
 */
webpage_t *pageload(int id, char *dirnm) {

	FILE *fp;
	char filename[10];
	char URL[200]; 	//200 is long enough
	int depth, htmlLength;
	char cwd[MAX_PATH_LENGTH]; 
	
	getcwd(cwd, sizeof(cwd)); //tracks the current working directory (would be crawler in this case)	
	chdir(dirnm); //changes over to directory from which we are loading filenames from (../pages in this case)
	
	sprintf(filename, "%d", id); //convert int to string
	fp = fopen(filename,"r"); //open id file saved in pages
	
	if (fp == NULL) {
		printf("File is NULL");
		return NULL;
	}
	
	fscanf(fp, "%s %d %d", URL, &depth, &htmlLength); //captures URL, depth, and HTML Length info frome file
	//printf("URL is %s \nDepth is %d\nHtml Length is %d\n", URL, depth, htmlLength);

	//CAPTURING HTML FROM FILE
	char *html = (char*)malloc(sizeof(char)*htmlLength+1); 	//need to allocate memory for HTML of new wegpage_t
	char c;
	int i = 0;

   	while ( (c = fgetc(fp)) != EOF) { //scans each remaining character in the file until hits EOF
   		html[i] = c; 
   		i++;
   	}
   	html[i-1] = '\0'; //rewrite end of html string (change from EOF to '\0')
   	
   	fclose(fp);
	chdir(cwd);  // changes back to the original cwd
   	  
   	return (webpage_new(URL, depth, html)); //returns new webpage_t, or NULL if any error
}
















