CFLAGS=-Wall -pedantic -std=c11 -I../utils -L../lib -g
LIBS=-lutils -lcurl

all: crawler multiCrawler

%.o: %.c
			gcc $(CFLAGS) -c $<

crawler: crawler.o
		    gcc $(CFLAGS) crawler.o $(LIBS) -o $@

multiCrawler:	multiCrawler.o
				gcc $(CFLAGS) multiCrawler.o $(LIBS) -o $@ -lpthread

clean:
				rm -f *.o crawler multiCrawler


