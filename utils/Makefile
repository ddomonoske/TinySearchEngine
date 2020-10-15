CFLAGS=-Wall -pedantic -std=c11 -I. -g
OFILES=queue.o hash.o webpage.o


all:	$(OFILES)
			ar cr ../lib/libutils.a $(OFILES)

hash.o: 			hash.c hash.h queue.h
					gcc $(CFLAGS) -c hash.c

queue.o:			queue.c queue.h 
					gcc $(CFLAGS) -c queue.c

webpage.o:		webpage.c webpage.h
					gcc $(CFLAGS) -c webpage.c

clean:			
					rm -f *.o ../lib/libutils.a




