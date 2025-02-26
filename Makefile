build:
	gcc -Wall -std=c99 -lm ./src/*.c -o renderer -lSDL2main -lSDL2

run:
	./renderer

clear:
	rm ./renderer