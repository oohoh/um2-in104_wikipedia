all:		clean main

main:		client.o server.o sock.o sockdist.o
		g++ -lpthread -o ./client/client ./client/client.o ./lib/sock.o ./lib/sockdist.o
		g++ -lpthread -o ./server/server ./server/server.o ./lib/sock.o ./lib/sockdist.o

sock.o:		./lib/sock.cc
		g++ -o ./lib/sock.o -c ./lib/sock.cc

sockdist.o:	./lib/sockdist.cc
		g++ -o ./lib/sockdist.o -c ./lib/sockdist.cc

client.o:	./client/client.cc
		g++ -o ./client/client.o -c ./client/client.cc

server.o:	./server/server.cc
		g++ -o ./server/server.o -c ./server/server.cc

clean:
		rm -rf ./lib/*.o ./client/*.o ./server/*.o
