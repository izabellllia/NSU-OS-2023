all: main create_file permissions

main: main.c
	gcc -o main main.c -Wall

create_file:
	touch some_file

permissions:
	chmod 600 some_file
	chmod 4755 main

clean:
	rm main
	rm some_file
