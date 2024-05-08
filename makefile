CC = gcc
CFLAGS = -Wall -Wpedantic -Werror
LDFLAGS = #-lncurses
OS := $(shell uname)

mkBinDir := $(shell mkdir -p bin)
mkObjDir := $(shell mkdir -p obj)

OBJS = 	./obj/pm_memory.o	\
		./obj/pm_error.o 	\
		./obj/pm_string.o 	\

final-run: final-link
ifeq ($(OS),Darwin) 
		./bin/practice.run
else 
		valgrind --track-origins=yes ./bin/practice.run  
endif

final-link: $(OBJS) ./obj/main.o
	$(CC) $(CFLAGS) $(OBJS) ./obj/main.o -o ./bin/practice.run

./obj/%.o: ./%.c ./%.h ./pm_common.h
	$(CC) $(CFLAGS) -c $< -o $@

./obj/main.o: ./main.c ./main.h
	$(CC) $(CFLAGS) -c ./main.c -o ./obj/main.o

obj-clean:
	trash-put ./obj/*