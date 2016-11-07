COMPILER_FLAGS = -O3 -Wall -std=c++11 -ggdb
LINKER_FLAGS = -lreadline

all:
	make bish

bish: bish.o parse.o util_fns.o Makefile
	g++ *.o -o bish $(LINKER_FLAGS)

bish.o: bish.cc Makefile
	g++ bish.cc -c -o bish.o $(COMPILER_FLAGS)

parse.o: parse.cc Makefile
	g++ parse.cc -c -o parse.o $(COMPILER_FLAGS)

util_fns.cc: util_fns.cc Makefile
	g++ util_fns.cc -c -o util_fns.o $(COMPILER_FLAGS)

clean:
	$(RM) *.o bish; touch Makefile; make
