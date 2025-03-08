build:
	gcc -Wall -std=c99 ./src/*.c -o renderer -lSDL2 -lm

run:
	./renderer

clear:
	rm ./renderer