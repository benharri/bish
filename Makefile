CSW = -O3 -Wall -std=c++11 -ggdb
LSW = -lreadline

all:
	make bish

bish: bish.o parse.o util_fns.o Makefile
	g++ bish.o parse.o util_fns.o -o bish $(LSW)

bish.o: bish.cc Makefile
	g++ bish.cc -c -o bish.o $(CSW)

parse.o: parse.cc Makefile
	g++ parse.cc -c -o parse.o $(CSW)

util_fns.cc: util_fns.cc Makefile
	g++ util_fns.cc -c -o util_fns.o $(CSW)

clean:
	touch Makefile; make
