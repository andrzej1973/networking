#g++ ./src/server/server.cpp -o ./server -lpthread
# make file to compile sockets server program

# compiler to use
CC=g++

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings

CFLAGS=-g -Wall

LIBS=-lpthread

SOURCES=./src/

server: server.o
	$(CC)	-o	server	server.o	$(CFLAGS)	$(LIBS)

server.o: $(SOURCES)/server/server.cpp $(SOURCES)/common/sockets_debug.h
	$(CC)	-c	./src/server/server.cpp	$(CFLAGS)

clean:
# -f so this will succeed even if the files don't exist
	rm	-f	server ./server.o