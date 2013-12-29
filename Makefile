.PHONY: all clean
CC = gcc
CFLAGS = -Wall -g -I./include

all: client server

run: client server
	sudo ./server 127.0.0.3 12346 127.0.0.2 12345 &  sudo ./client 127.0.0.2 12345 127.0.0.3 12346

client: src/peer.o src/net.o
	$(CC) $(CFLAGS) -o $@ $^

server: src/peer.o src/net.o
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm client server src/*.o
