CC=gcc
CCFLAGS=-Wall -Wextra -g -std=c11

.PHONY: all
all: thsort

thsort: thsort.c
	${CC} ${CCFLAGS} ${<} -o ${@}

.PHONY: debug
debug: thsort
	gdb ./thsort

.PHONY: leak-check
leak-check: thsort
	valgrind --leak-check=full ./thsort

.PHONY: clean
clean:
	rm thsort
