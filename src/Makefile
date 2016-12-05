OBJ = mymake.o AdjList.o
CC = gcc
FLAGS = -Wall -fprofile-arcs -ftest-coverage

mymake : $(OBJ)
	$(CC) -Wall $(OBJ) -o mymake

%.o : %.c mymake.h
	$(CC) -Wall -c $*.c

.PHONY: clean coverage
coverage:
	$(CC) $(FLAGS) -o mymake mymake.c AdjList.c

clean:
	rm -f *.o mymake *.gcov *.gcno *.gcda
