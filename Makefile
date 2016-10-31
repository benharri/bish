CSW = -O3 -Wall -std=c++11 -ggdb
LSW = -std=c++11

all:
	make bish

bish: bish.o Makefile
	g++ bish.o -o bish $(LSW)

bish.o: bish.cc Makefile
	g++ bish.cc -c -o bish.o $(CSW)

clean:
	touch Makefile; make
