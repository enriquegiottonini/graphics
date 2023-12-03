CC = gcc
CFLAGS = -Wall -Wextra

.PHONY: all build run test clean

all: build

build: main

main: main.c utils.c
	$(CC) $(CFLAGS) $^ -o main

tests: test.c utils.c
	$(CC) $(CFLAGS) $^ -o test

run: build
	./main

test: tests
	./test

clean:
	rm -f main test