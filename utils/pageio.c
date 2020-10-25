/* pageio.c-- implementation of pageio.h
 *
 * Author: Eric Chen
 * Created: Sat, Oct 24
 * Version:
 *
 * Description:
*/ 

#include "pageio.h"


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
	fprintf(fp,"%s\n%d\n%d\n%s\n", webpage_getURL(pagep), webpage_getDepth(pagep),
					webpage_getHTMLlen(pagep), webpage_getHTML(pagep)); //prints URL, depth, HTML length, and HTML into file
	printf("pagesave: %s\n", webpage_getURL(pagep));

	fclose(fp);
	return 0; //will always return 0
}


/* 
 * pageload -- loads the numbered filename <id> in direcory <dirnm>
 * into a new webpage
 *
 * returns: non-NULL for success; NULL otherwise
 */
webpage_t *pageload(int id, char *dirnm) {

	FILE *fp;
	webpage_t *page;
	char filename[10];
	char urlstr[100];
	char depthstr[10];
	int depth;
	
	chdir(dirnm); //changes over to right directory
	sprintf(filename, "%d", id); //convert int to string
	
	fp = fopen(filename,"r"); 
	fscanf(fp, "%s %s", urlstr, depthstr); //stores URL and depth info
	printf("URL is %s \n Depth is %s\n", urlstr, depthstr);
	depth = atoi(depthstr);
	fclose(fp);
	
	page = (webpage_new(urlstr, depth, NULL)); //create new webpage_t
	printf("pageload: %s\n", webpage_getURL(page));

	if (webpage_fetch(page)) {
		return page;
	}
	else return NULL; //if webpage_fetch fails
	
}










