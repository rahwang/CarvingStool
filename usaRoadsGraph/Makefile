all: graph-test

graph-test: hash.h hash.c linked-list.h linked-list.c type.h type.c main.c
	gcc -o graph-test linked-list.c hash.c type.c main.c

clean:
	rm -rf a.out graph-test