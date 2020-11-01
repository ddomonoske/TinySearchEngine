CFLAGS=-Wall -pedantic -std=c11 -I../utils -L../lib -g
LIBS=-lutils -lcurl

%.o: %.c
			gcc $(CFLAGS) -c $<

querier:	querier.o
				gcc $(CFLAGS) querier.o $(LIBS) -o $@

clean:
				rm -f *.o querier


