CC = gcc
CFLAGS = -std=c99 -Wall -Wpedantic -luuid -Werror -o3
OS := $(shell uname)

mkBinDir := $(shell mkdir -p bin)
mkObjDir := $(shell mkdir -p obj)

EXEC = ./bin/main.run

MAIN = 	./obj/main.o

OBJS = 		./obj/nc_memory.o \
		./obj/nc_error.o \
		./obj/nc_string.o \
		./obj/nc_repl.o \
		./obj/nc_file.o \
		./obj/nc_db.o \
		./obj/nc_csv.o \

default: reset $(EXEC)
ifeq ($(OS),Darwin) 
	$(EXEC)
else 
	valgrind --track-origins=yes --leak-check=full $(EXEC)  
endif

reset: clear link

run: reset $(EXEC)
	$(EXEC)

link: $(OBJS) $(MAIN)
	$(CC) $(CFLAGS) $(OBJS) $(MAIN) -o $(EXEC)

./obj/%.o: ./src/%.c ./src/%.h ./src/nc_common.h
	$(CC) $(CFLAGS) -c $< -o $@

./obj/main.o: ./src/main.c
	$(CC) $(CFLAGS) -c ./src/main.c -o ./obj/main.o
	
clear: clear-bin clear-obj

clear-bin:
	-rm ./bin/*

clear-obj:
	-rm ./obj/*
