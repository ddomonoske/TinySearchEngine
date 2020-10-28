CFLAGS=-Wall -pedantic -std=c11 -I../utils -L../lib -g
LIBS=-lutils -lcurl

all:		indexer indexer_test

%.o: %.c
			gcc $(CFLAGS) -c $<

indexer:	indexer.o
				gcc $(CFLAGS) indexer.o $(LIBS) -o $@

indexer_test: indexer_test.o
				gcc $(CFLAGS) indexer_test.o $(LIBS) -o $@

clean:
				rm -f *.o indexer indexer_test
