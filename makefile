CC = gcc
CFLAGS = -std=c99 -Wall -Wpedantic -luuid -Werror
OS := $(shell uname)

mkBinDir := $(shell mkdir -p bin)
mkObjDir := $(shell mkdir -p obj)

MAIN = 	./obj/main.o

OBJS = 	./obj/teal_memory.o		\
		./obj/teal_error.o 		\
		./obj/teal_string.o 		\
		./obj/teal_interpreter.o 	\
		./obj/teal_file.o 		\
		./obj/teal_db.o 			\

run: final-link
ifeq ($(OS),Darwin) 
		./bin/practice.run
else 
		valgrind --track-origins=yes --leak-check=full ./bin/teal_main.run  
endif

final-link: $(OBJS) $(MAIN)
	$(CC) $(CFLAGS) $(OBJS) $(MAIN) -o ./bin/teal_main.run

./obj/%.o: ./src/%.c ./src/%.h ./src/teal_common.h
	$(CC) $(CFLAGS) -c $< -o $@

./obj/main.o: ./src/main.c
	$(CC) $(CFLAGS) -c ./src/main.c -o ./obj/main.o

clean-obj:
	trash-put ./obj/*
	
clean: clean-obj
	trash-put ./bin/*
