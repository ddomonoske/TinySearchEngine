CFLAGS=-Wall -pedantic -std=c11 -I../utils -L../lib -g
LIBS=-lutils -lcurl

all:  testLCounter #testH testQ

%.o: %.c
			gcc $(CFLAGS) -c $<

testLCounter: testLCounter.o
				gcc $(CFLAGS) testLCounter.o $(LIBS) -o $@

testH:  
				gcc $(CFLAGS) testH.o $(LIBS) -o $@ -lpthread

testQ:  
				gcc $(CFLAGS) testQ.o $(LIBS) -o $@ -lpthread

clean:
				rm -f testH testQ testLCounter
