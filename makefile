all: client server

client: client.o pipe_networking.o
	gcc -o client client.o pipe_networking.o -lm

server: basic_server.o pipe_networking.o 
	gcc -o server basic_server.o pipe_networking.o -lm

client.o: client.c pipe_networking.h
	gcc -c client.c

basic_server.o: basic_server.c pipe_networking.h
	gcc -c basic_server.c

pipe_networking.o: pipe_networking.c pipe_networking.h
	gcc -c pipe_networking.c -lm

clean:
	rm *.o
	rm *~
