all:		client

client:		client.o sock.o sockdist.o
		g++ -o client client.o sock.o sockdist.o

sock.o:		sock.cc
		g++ -o sock.o -c sock.cc

sockdist.o:	sockdist.cc
		g++ -o sockdist.o -c sockdist.cc 

client.o:	client.cc
		g++ -o client.o -c client.cc

clean:
		rm -rf *.o
