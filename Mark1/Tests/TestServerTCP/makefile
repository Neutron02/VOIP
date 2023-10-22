# Compiler to use
CC = g++
# Compiler flags
CFLAGS = -g
# Libraries for linking (if any, for instance -lpthread for threads)
LIBS =
testServer:
	-@kill -9 $$(lsof -t -i :8080) 2>/dev/null || true
	@$(CC) $(CFLAGS) -o server_main_test server_main.cpp -lpthread
	@./server_main_test &
	@sleep 1
	@echo "----------------------------Starting test -------------------------------------"
	@echo "My message to server" | netcat localhost 8080
	@wait

testServerMultipleConnections:


clean:
	rm -f client server *.o
