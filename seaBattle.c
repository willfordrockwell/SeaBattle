//---------------------------------------------------------------------------
#include <errno.h>
#include <unistd.h>

#include "network.h"
#include "graphics.h"
#include "gameLogic.h"

//---------------------------------------------------------------------------
void checkArgs(int argc) 
{
	if (argc < 2) {
		printf("Enter argument: 0 for client (Second player), 1 for server (First player)");
		exit(-1);
	}
}
//---------------------------------------------------------------------------

int main (int argc, char *argv[]) 
{	//Server: 1; Client: 0
	checkArgs(argc);
	SOCKET sockTCP = initConnection(argv);
	enum battleField playerBattleField[FIELDSIDE][FIELDSIDE];
	enum battleField enemyBattleField[FIELDSIDE][FIELDSIDE];
	for(int i = 0; i < FIELDSIDE; i++) {		//Init battlefields with EMPTY
		for(int j = 0; j < FIELDSIDE; j++) {
			playerBattleField[i][j] = EMPTY;
			enemyBattleField[i][j] = EMPTY;
		}
	}
	enum turn playerTurn;
	if (atoi(argv[1]) == 1)	{
		playerTurn = PLAYER;
	} else {
		playerTurn = ENEMY;
	}
	int endGame = 0, isSurrender = 0;
	unsigned int playerScore = 0, enemyScore = 0;
	char move[MOVESIZE];
	enum moveResult result;
	struct coord shipPositions[6][4];
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 4; j++) {
			shipPositions[i][j].x = -1;
			shipPositions[i][j].y = -1;
		}
	}
	setBattleField(playerBattleField, shipPositions);
	while(endGame == 0) {
		if (playerTurn == PLAYER) {
			//your turn
			do {
				drawBattleField(playerBattleField, enemyBattleField, 0, playerScore, enemyScore, playerTurn, move);
				result = makeMove(move, enemyBattleField, sockTCP);
				switch (result) {
				case KILL_MOVE:
					fillMissMovesToEnemy(verifyMove(move), enemyBattleField);
				case HIT_MOVE:
					playerScore++;
					break;
				case SURRENDER:
					isSurrender++;
					endGame++;
					break;
				default:
					break;
				}
				
				if (playerScore == 20 || enemyScore == 20) {
					endGame++;
				}
			} while ((result != SURRENDER && result != MISS_MOVE && result != WRONG_MOVE) && endGame == 0);
		} else {
			//enemy turn
			do {
				drawBattleField(playerBattleField, enemyBattleField, 0, playerScore, enemyScore, playerTurn, 0);
				result = recvMove(sockTCP, playerBattleField, shipPositions, move);
				switch(result) {
				case KILL_MOVE:
					fillMissMovesToPlayer(shipPositions, verifyMove(move), playerBattleField);
					strcpy(move, "KILL_MOVE");
				case HIT_MOVE:
					enemyScore++;
					if (result == HIT_MOVE) {
						strcpy(move, "HIT_MOVE");
					}
					break;
				case SURRENDER:
				strcpy(move, "SURRENDER");
					isSurrender++;
					endGame++;
					break;
				case MISS_MOVE:
					strcpy(move, "MISS_MOVE");
					break;
				case WRONG_MOVE:
					strcpy(move, "WRONG_MOVE");
					break;
				}
				send(sockTCP, move, strlen(move), 0);
				
				if (playerScore == 20 || enemyScore == 20) {
					endGame++;
				}
			} while ((result !=SURRENDER && result != MISS_MOVE && result != WRONG_MOVE) && endGame == 0);
		}
		if (isSurrender == 0) {
			playerTurn = changeTurn(playerTurn);
		}
	}
	drawBattleField(playerBattleField, enemyBattleField, 0, playerScore, enemyScore, playerTurn, 0);
	if (playerTurn == PLAYER) {
		printf("\nYou lose\n");
	} else {
		printf("\nYou Win\n");
	}
	
	return 0;
}