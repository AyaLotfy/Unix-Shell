all: Shell9

Shell9: main.o command_parser.o file_processing.o
	gcc main.o command_parser.o file_processing.o -o Shell9

main.o: main.c
	gcc -c main.c
command_parser.o: command_parser.c
	gcc -c command_parser.c

file_processing.o: file_processing.c
	gcc -c file_processing.c

clean:
	rm -rf *o Shell7

