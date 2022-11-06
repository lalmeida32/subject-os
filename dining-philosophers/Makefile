FILES=$(shell find ./src/*.c ./src/**/*.c)

all:
	gcc $(FILES) -o ./dist/main

run:
	./dist/main

valgrind:
	valgrind ./dist/main