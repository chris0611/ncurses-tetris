CC = gcc
CFLAGS = -std=c99 -O2 -march=native -Wall -Wextra -Wpedantic #-DDEBUG
OBJS = main.o tetris.o ui.o util.o
LIBS = -lcurses -lm

.PHONY: all clean

all: main

main: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

clean:
	rm -f main *.o
