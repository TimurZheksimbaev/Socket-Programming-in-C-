run_server:
	clang++ server.cpp -o server -pthread
	./server
	rm server

run_client:
	clang++ client.cpp -o client
	./client
	rm client

