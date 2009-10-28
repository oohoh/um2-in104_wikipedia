all:		main clean

main:		server.o sock.o sockdist.o
		g++ -lpthread -o server server.o sock.o sockdist.o

sock.o:		sock.cc
		g++ -o sock.o -c sock.cc

sockdist.o:	sockdist.cc
		g++ -o sockdist.o -c sockdist.cc 

server.o:	server.cc
		g++ -o server.o -c server.cc

clean:
		rm -rf *.o
