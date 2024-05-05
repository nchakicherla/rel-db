CC = gcc
CFLAGS = -Wall -Wpedantic -Werror
LDFLAGS = #-lncurses
OS := $(shell uname)

mkBinDir := $(shell mkdir -p bin)
mkObjDir := $(shell mkdir -p obj)

OBJS = 		./obj/main.o 		\
								\
			./obj/memory.o 		\


final-run: final-link
ifeq ($(OS),Darwin) 
		./bin/practice.run
else 
		valgrind --track-origins=yes ./bin/practice.run  
endif

final-link: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o ./bin/practice.run

./obj/%.o: ./%.c ./%.h
	$(CC) $(CFLAGS) -c $< -o $@
