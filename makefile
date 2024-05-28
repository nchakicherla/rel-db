CC = gcc
CFLAGS = -std=c99 -Wall -Wpedantic -luuid -Werror
OS := $(shell uname)

mkBinDir := $(shell mkdir -p bin)
mkObjDir := $(shell mkdir -p obj)

MAIN = 	./obj/main.o

OBJS = 		./obj/nc_memory.o \
		./obj/nc_error.o \
		./obj/nc_string.o \
		./obj/nc_inter.o \
		./obj/nc_file.o \
		./obj/nc_db.o \
		./obj/nc_csv.o \

run-default: link
ifeq ($(OS),Darwin) 
		./bin/main.run
else 
		valgrind --track-origins=yes --leak-check=full ./bin/main.run  
endif

run: link
	./bin/main.run

link: $(OBJS) $(MAIN)
	$(CC) $(CFLAGS) $(OBJS) $(MAIN) -o ./bin/main.run

./obj/%.o: ./src/%.c ./src/%.h ./src/nc_common.h
	$(CC) $(CFLAGS) -c $< -o $@

./obj/main.o: ./src/main.c
	$(CC) $(CFLAGS) -c ./src/main.c -o ./obj/main.o

clear-obj:
	trash-put ./obj/*
	
clear: clear-obj
	trash-put ./bin/*
