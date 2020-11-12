CFLAGS=-Wall -pedantic -std=c11 -I../utils -L../lib -g
LIBS=-lutils -lcurl

all:  lhThreads lhashTest testH testQ

%.o: %.c
			gcc $(CFLAGS) -c $<

lhThreads:	lhThreads.o
				gcc $(CFLAGS) lhThreads.o $(LIBS) -o $@ -lpthread

lhashTest:  lhashTest.o
				gcc $(CFLAGS) lhashTest.o $(LIBS) -o $@ -lpthread

testH:  
				gcc $(CFLAGS) testH.o $(LIBS) -o $@ -lpthread

testQ:  
				gcc $(CFLAGS) testQ.o $(LIBS) -o $@ -lpthread


clean:
				rm -f lhThreads lhashTest testH testQ
