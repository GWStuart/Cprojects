CC = gcc
TARGETS = barnsley-fern.out

all:
	$(CC) barnsley-fern.c $(shell pkg-config --cflags --libs sdl3) -o barnsley-fern.out

clean:
	rm -f $(TARGETS) *.o
