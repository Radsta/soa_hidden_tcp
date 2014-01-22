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

client_run:
	@echo "sudo ./client 127.0.0.1 12345 coveropts2.pwnz.org 12346"
	@echo "> ls"
	@echo "manga_brewer.sh  p3rs0n41  pdf_merge.sh  personal_transcript.zip  public_html"

server_run:
	@echo "sudo ./server 127.0.0.1 12346 covertopts1.pwnz.org 12345"
	@echo "Command received and validated: ls"
