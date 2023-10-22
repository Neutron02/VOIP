CC=g++
CFLAGS=-g -std=c++11 -pthread

server_test_multi: server_test_multi.cpp
	$(CC) $(CFLAGS) $< -o $@ -lpthread

testServer: server_test_multi
	-@kill -9 $$(lsof -t -i :8080) 2>/dev/null || true
	./server_test_multi &
	sleep 2
	@echo "----------------------------------- Starting multiple client tests -----------------------------------"
	@for i in `seq 1 5`; do \
		echo "Message from client $$i" | netcat localhost 8080 & \
		sleep 0.1; \
	done
	wait

clean:
	rm -f server_test_multi


