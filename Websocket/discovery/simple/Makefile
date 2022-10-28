all:server client
.PHONY: all

server:
	g++ -g -o server server.c -I. -L. -lwebsockets -lpthread
client:
	g++ -g -o client client.c -I. -L. -lwebsockets -lpthread

clean:
	rm -rf server client
