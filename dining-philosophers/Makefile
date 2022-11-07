C_FILES=$(shell find ./src/*.c ./src/**/*.c)
H_FILES=$(shell find ./src/**/*.h)

all:
	gcc $(C_FILES) -o ./dist/main

run:
	./dist/main

valgrind:
	valgrind ./dist/main

zip:
	zip -r dinphil.zip $(C_FILES) $(H_FILES) .gitignore Makefile *.docx