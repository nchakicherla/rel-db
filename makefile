CC = gcc
CFLAGS = -Wall -Wpedantic -Werror
LDFLAGS = #-lncurses
OS := $(shell uname)

mkBinDir := $(shell mkdir -p bin)
mkObjDir := $(shell mkdir -p obj)


MAIN = 	./obj/main.o

OBJS = 	./obj/pm_memory.o	\
		./obj/pm_error.o 	\
		./obj/pm_string.o 	\

final-run: final-link
ifeq ($(OS),Darwin) 
		./bin/practice.run
else 
		valgrind --track-origins=yes ./bin/practice.run  
endif

final-link: $(OBJS) $(MAIN)
	$(CC) $(CFLAGS) $(OBJS) $(MAIN) -o ./bin/practice.run

./obj/%.o: ./src/%.c ./src/%.h ./src/pm_common.h
	$(CC) $(CFLAGS) -c $< -o $@

main-build: ./src/main.c ./src/main.h
	$(CC) $(CFLAGS) -c ./src/main.c -o ./obj/main.o

obj-clean:
	trash-put ./obj/*