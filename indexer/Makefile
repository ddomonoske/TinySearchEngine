CFLAGS=-Wall -pedantic -std=c11 -I../utils -L../lib -g
LIBS=-lutils -lcurl

%.o: %.c
			gcc $(CFLAGS) -c $<

indexer:	indexer.o
				gcc $(CFLAGS) indexer.o $(LIBS) -o $@

clean:
				rm -f *.o indexer


