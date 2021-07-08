all:
	gcc -g -Wall -Werror src/*.c -lm -o exec

run:
	./exec

clear:
	rm *.bin exec

zip:
	zip exercicio.zip src/*.c src/*.h Makefile