/* testLCrawler.c --- 
 * 
 * 
 * Author: David W. Domonoske
 * Created: Wed Nov 25 21:56:44 2020 (-0500)
 * Version: 
 * 
 * Description: 
 * 
 */


#include "lcounter.h"
#include <stdio.h>

int main(void) {
	lcounter_t *cp;

	cp = openLCount();

	printf("begin:\t%d\n", getLCount(cp));
	printf("inc:\t%d\n", incLCount(cp));
	printf("inc:\t%d\n", incLCount(cp));
	for (int i=0; i<4; i++) {
		printf("dec:\t%d\n", decLCount(cp));
	}
	printf("set10:\t%d\n", setLCount(cp,10));
	printf("reset:\t%d\n", resetLCount(cp));
	for (int i=0; i<4; i++) {
		printf("add3:\t%d\n", addLCount(cp,3));
	}
	printf("closing\n");
	closeLCount(cp);
	return 0;
}
