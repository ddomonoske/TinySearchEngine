CFLAGS=-Wall -pedantic -std=c11 -I. -g

all:		 testPut testGet testApply testRemove

list.o: 		list.c list.h
				gcc $(CFLAGS) -c list.c

testPut.o:  	testPut.c list.h
				gcc $(CFLAGS) -c testPut.c

testPut:		testPut.o list.o
				gcc $(CFLAGS) testPut.o list.o -o testPut

testGet.o:  	testGet.c list.h
				gcc $(CFLAGS) -c testGet.c

testGet:		testGet.o list.o
				gcc $(CFLAGS) testGet.o list.o -o testGet

testApply.o:  	testApply.c list.h
				gcc $(CFLAGS) -c testApply.c

testApply:		testApply.o list.o
				gcc $(CFLAGS) testApply.o list.o -o testApply

testRemove.o: testRemove.c list.h
				gcc $(CFLAGS) -c testRemove.c

testRemove:		testRemove.o list.o
				gcc $(CFLAGS) testRemove.o list.o -o testRemove

clean:			
				rm -f *.o testPut testGet testApply testRemove
