# Compiler to use
CC = g++
# Compiler flags
CFLAGS = -g
# Libraries for linking (if any, for instance -lpthread for threads)
LIBS =

all: client server

client: client.cpp TCPConnection.o
	$(CC) $(CFLAGS) -o client client.cpp TCPConnection.o $(LIBS)

server: server.cpp TCPConnection.o
	$(CC) $(CFLAGS) -o server server.cpp TCPConnection.o $(LIBS)

TCPConnection.o: TCPConnection.cpp TCPConnection.h
	$(CC) $(CFLAGS) -c TCPConnection.cpp

testClient:
	$(CC) $(CFLAGS) -o client_test client_test.cpp
	$(CC) $(CFLAGS) -o server_test server_test.cpp

clean:
	rm -f client server *.o
