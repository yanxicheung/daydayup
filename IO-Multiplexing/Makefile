$(shell if [ -e build ]; then rm -rf build; fi; mkdir build)

all: epoll_server epoll_client \
	fork_server fork_client \
	poll_server poll_client \
	select_server select_client \
	socket_server socket_client

epoll_server: epoll/server.cpp
	g++ -std=c++11 -Wall -O3 epoll/server.cpp -o build/epoll_server

epoll_client: epoll/client.cpp
	g++ -std=c++11 -Wall -O3 epoll/client.cpp -o build/epoll_client

fork_server: fork/server.cpp
	g++ -std=c++11 -Wall -O3 fork/server.cpp -o build/fork_server

fork_client: fork/client.cpp
	g++ -std=c++11 -Wall -O3 fork/client.cpp -o build/fork_client

poll_server: poll/server.cpp
	g++ -std=c++11 -Wall -O3 poll/server.cpp -o build/poll_server

poll_client: poll/client.cpp
	g++ -std=c++11 -Wall -O3 poll/client.cpp -o build/poll_client

select_server: select/server.cpp
	g++ -std=c++11 -Wall -O3 select/server.cpp -o build/select_server

select_client: select/client.cpp
	g++ -std=c++11 -Wall -O3 select/client.cpp -o build/select_client

socket_server: socket/server.cpp
	g++ -std=c++11 -Wall -O3 socket/server.cpp -o build/socket_server

socket_client: socket/client.cpp
	g++ -std=c++11 -Wall -O3 socket/client.cpp -o build/socket_client

clean:
	rm -rf build
