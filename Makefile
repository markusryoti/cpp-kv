build-server:
	g++ -o server kv.cpp command.cpp server.cpp -std=c++17

build-client:
	g++ -o client client.cpp -std=c++17
