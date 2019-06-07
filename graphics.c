#include <stdio.h>
#include <windows.h>
#include "graphics.h"

#define FIELDSIDE 10

#define FOREGROUND_WHITE FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE
#define FOREGROUND_YELLOW FOREGROUND_RED | FOREGROUND_GREEN

void drawBattleField	
(	enum battleField playerBattleField[][FIELDSIDE], 
	enum battleField enemyBattleField[][FIELDSIDE], 
	int isSet, 
	unsigned int playerScore, 
	unsigned int enemyScore, 
	enum turn playerTurn, 
	char *move
)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	system("cls");
	printf("   0|1|2|3|4|5|6|7|8|9|");
	if (isSet == 0)
		printf("   0|1|2|3|4|5|6|7|8|9|");
	printf("\n");
	for (int i = 0; i < FIELDSIDE; i++) {
		printf("  ---------------------");
		if (isSet == 0) 
			printf("  ---------------------");
		printf("\n%c |", i + 65);
		for (int j = 0; j < 2 * FIELDSIDE + 1; j++) {
			if (j < FIELDSIDE) {
				if (playerBattleField[i][j] == EMPTY) { 
					printf(" |");
				} else if (playerBattleField[i][j] == MISS) { 
					SetConsoleTextAttribute(hConsole, FOREGROUND_YELLOW); 
					printf("*"); 
					SetConsoleTextAttribute(hConsole, FOREGROUND_WHITE); 
					printf("|");
				} else if (playerBattleField[i][j] == HIT) { 
					SetConsoleTextAttribute(hConsole, FOREGROUND_RED); 
					printf("X");  
					SetConsoleTextAttribute(hConsole, FOREGROUND_WHITE); 
					printf("|");
				} else { 
					SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN); 
					printf("#");  
					SetConsoleTextAttribute(hConsole, FOREGROUND_WHITE); 
					printf("|");
				}
			} else if (j == FIELDSIDE && isSet == 0) {
				printf(" %c|", i + 65);
			}
			else if (j > FIELDSIDE && isSet == 0) {
				if (enemyBattleField[i][j - FIELDSIDE - 1] == EMPTY) { 
					printf(" |");
				} else if (enemyBattleField[i][j - FIELDSIDE - 1] == MISS) { 
					SetConsoleTextAttribute(hConsole, FOREGROUND_YELLOW); 
					printf("*"); 
					SetConsoleTextAttribute(hConsole, FOREGROUND_WHITE); 
					printf("|");
				} else if (enemyBattleField[i][j - FIELDSIDE - 1] == HIT) { 
					SetConsoleTextAttribute(hConsole, FOREGROUND_RED); 
					printf("X");
					SetConsoleTextAttribute(hConsole, FOREGROUND_WHITE); 
					printf("|");
				} else { 
					SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN); 
					printf("#");  
					SetConsoleTextAttribute(hConsole, FOREGROUND_WHITE); 
					printf("|");
				}
			}
		}
		printf("\n");
	}
	printf("  ---------------------");
	if (isSet == 0) {
		printf("  ---------------------\nYour score: %d\nEnemy score: %d\n", playerScore, enemyScore);
		if (playerScore != 20 && enemyScore != 20) {
			if (playerTurn == PLAYER) {
				printf("Your turn\nEnter position for FIRE like 'A4' or \n'SURRENDER' to SURRENDER:");
				fgets(move, MOVESIZE, stdin);
			} else {
				printf("Wait for your enemy to FIRE\n");
				move = "\n";
			}
		}
	} else {
		printf("\nSet your ships:\nWASD to change position\nr to rotate clockwise\ne to set");
	}
}
//---------------------------------------------------------------------------