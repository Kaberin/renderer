build:
	gcc -Wall -std=c99 ./src/*.c -o renderer -lSDL2 -lm -g

run:
	./renderer

clear:
	rm ./renderer