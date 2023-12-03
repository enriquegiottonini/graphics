CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS = -lGL -lGLU -lglut


.PHONY: all build run test clean

all: build

build: main

main: main.c utils.c
	$(CC) $(CFLAGS) $^ -o main $(LDFLAGS)

tests: test.c utils.c
	$(CC) $(CFLAGS) $^ -o test $(LDFLAGS)

run: build
	./main

test: tests
	./test

clean:
	rm -f main test