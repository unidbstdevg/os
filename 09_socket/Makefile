run_server: server
	./server

run_client: client
	./client


server: server.cpp shared.h
	clang++ server.cpp -o ./server

client: client.cpp shared.h
	clang++ client.cpp -lreadline -o ./client
