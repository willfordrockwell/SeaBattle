all: main.c
	gcc -o main.exe main.c -lws2_32
debug: main.c
	gcc -o main.exe main.c -lws2_32 -g3