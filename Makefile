all: seaBattle.c
	gcc -o seaBattle.exe seaBattle.c -lws2_32
debug: seaBattle.c
	gcc -o seaBattle.exe seaBattle.c -lws2_32 -g3