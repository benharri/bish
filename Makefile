COMPILER_FLAGS = -O3 -Wall -std=c++11 -ggdb
LINKER_FLAGS = -lreadline
EXECUTABLE_NAME = bish
OBJS = \
	bish.o \
	parse.o \
	util_fns.o

all:
	make bish

bish: $(OBJS) Makefile
	g++ $(OBJS) -o $(EXECUTABLE_NAME) $(LINKER_FLAGS)

bish.o: bish.cc Makefile
	g++ bish.cc -c -o bish.o $(COMPILER_FLAGS)

parse.o: parse.cc Makefile
	g++ parse.cc -c -o parse.o $(COMPILER_FLAGS)

util_fns.o: util_fns.cc Makefile
	g++ util_fns.cc -c -o util_fns.o $(COMPILER_FLAGS)

clean:
	$(RM) $(OBJS) $(EXECUTABLE_NAME); touch Makefile; make
