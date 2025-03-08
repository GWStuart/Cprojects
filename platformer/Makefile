CC = gcc
TARGETS = platformer.out

all:
	$(CC) platformer.c $(shell pkg-config --cflags --libs sdl3) -o platformer.out

clean:
	rm -f $(TARGETS) *.o
