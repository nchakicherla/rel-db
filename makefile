CC = gcc
CFLAGS = -std=c99 -Wall -Wpedantic -Werror -luuid
OS := $(shell uname)

mkBinDir := $(shell mkdir -p bin)
mkObjDir := $(shell mkdir -p obj)


MAIN = 	./obj/main.o

OBJS = 	./obj/felt_memory.o		\
		./obj/felt_error.o 		\
		./obj/felt_string.o 		\
		./obj/felt_interpreter.o 	\
		./obj/felt_file.o 		\
		./obj/felt_db.o 			\

final-run: final-link
ifeq ($(OS),Darwin) 
		./bin/practice.run
else 
		valgrind --track-origins=yes ./bin/practice.run  
endif

final-link: $(OBJS) $(MAIN)
	$(CC) $(CFLAGS) $(OBJS) $(MAIN) -o ./bin/practice.run

./obj/%.o: ./src/%.c ./src/%.h ./src/felt_common.h
	$(CC) $(CFLAGS) -c $< -o $@

./obj/main.o: ./src/main.c
	$(CC) $(CFLAGS) -c ./src/main.c -o ./obj/main.o
clean: obj-clean
	trash-put ./bin/*

obj-clean:
	trash-put ./obj/*
