CFLAGS=-Wall -pedantic -std=c11 -I../utils -L../lib -g
LIBS=-lutils -lcurl

%.o: %.c
			gcc $(CFLAGS) -c $<

crawler:	crawler.o
				gcc $(CFLAGS) crawler.o $(LIBS) -o $@

clean:
				rm -f *.o crawler


