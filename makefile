CC = gcc
CFLAGS = -Wall -Wpedantic -Werror
LDFLAGS = #-lncurses

OS := $(shell uname)
mkBinDir := $(shell mkdir -p bin)

main-run: main-build
	./bin/practice.run

main-build: main.c
	$(CC) $(CFLAGS) main.c -o ./bin/practice.run