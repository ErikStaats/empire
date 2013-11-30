all: empire

empire: empire.c
	gcc -lncurses -o empire empire.c

