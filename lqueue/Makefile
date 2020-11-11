CFLAGS=-Wall -pedantic -std=c11 -I../utils -L../lib -g
LIBS=-lutils -lcurl

%.o: %.c
			gcc $(CFLAGS) -c $<

lqThreads:	lqThreads.o
				gcc $(CFLAGS) lqThreads.o $(LIBS) -o $@ -lpthread

clean:
				rm -f *.o lqThreads


