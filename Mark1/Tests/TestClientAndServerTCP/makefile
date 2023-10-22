# Compiler to use
CC = g++
# Compiler flags
CFLAGS = -g
# Libraries for linking (if any, for instance -lpthread for threads)
LIBS =
testServer:
	@$(CC) $(CFLAGS) -o server server.cpp -lpthread
	sleep 1
	@$(CC) $(CFLAGS) -o client client.cpp -lpthread
	@./server &
	@./client


clean:
	rm -f client server *.o
