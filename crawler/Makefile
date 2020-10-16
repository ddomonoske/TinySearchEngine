CFLAGS=-Wall -pedantic -std=c11 -I../utils/*.h -L../lib/libutils.a -g
LIBS=-Iutils -lcurl

crawler:
					gcc $(CFLAGS) crawler.c $(LIBS) -o $@

clean:
					rm -f crawler
