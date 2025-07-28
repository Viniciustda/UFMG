CC = gcc
CFLAGS = -Wall -pthread

all: server sensor

server: server.c common.c
	$(CC) $(CFLAGS) -o server server.c common.c

sensor: sensor.c common.c
	$(CC) $(CFLAGS) -o sensor sensor.c common.c

clean:
	rm -f server sensor