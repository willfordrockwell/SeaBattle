#include <string.h>
#include <stdlib.h>
#include <windows.h> 
#include <conio.h>

#include "gameLogic.h"


enum turn changeTurn(enum turn playerTurn)
{
	enum turn temp;
	if (playerTurn == ENEMY) {
		temp = PLAYER;
	} else {
		temp = ENEMY;
	}
	return temp;
}
//---------------------------------------------------------------------------
struct coord verifyMove(char *move)
{
	struct coord result;
	result.x = -1;
	result.y = -1;
	if (move[0] >= 'A' && move[0] <= 'J') {
		result.x = move[0] - 'A';
	} else if (move[0] >= 'a' && move[0] <= 'j') {
		result.x = move[0] - 'a';
	} else {
		return result;	//Error
	}
	
	if (move[1] >= '0' && move[1] <= '9') {
		result.y = move[1] - '0';
	} else {
		return result;	//Error
	}
	
	return result;
}
//---------------------------------------------------------------------------
int surrendered(char *move)
{
	if(strncmp(move, "SURRENDER", strlen("SURRENDER")) == 0) {
		return 1;
	} else {
		return 0;
	}
}
//---------------------------------------------------------------------------
enum moveResult isKill	
(	struct coord shipPositions[6][4], 
	struct coord lastMove, 
	enum battleField playerBattleField[][FIELDSIDE]
) 
{
	enum moveResult result = HIT_MOVE;
	int currentI = 0, currentJ = 0, hittedShips = 0, stopSearch = 0;
	for (int i = 0; i < 4 && stopSearch < 1; i++) {
		for (int j = 0; j < 6 && stopSearch < 1; j++) {
			if (shipPositions[j][i].x == -1 && shipPositions[j][i].y == -1) {
				continue;
			}
			if (shipPositions[j][i].x == lastMove.x && shipPositions[j][i].y == lastMove.y) {
				currentI = i;
				currentJ = j; 
				stopSearch++;
			}
		}
	}
	currentJ = currentJ - (currentJ % (currentI + 1));
	for (int j = 0; j < currentI + 1; j++) {
		if (playerBattleField[shipPositions[currentJ + j][currentI].x][shipPositions[currentJ + j][currentI].y] == HIT) {
			hittedShips++;
		}
	}
	if (hittedShips == currentI + 1) {
		result = KILL_MOVE;
	}
	
	return result;
}
//---------------------------------------------------------------------------
enum moveResult recvMove
(	SOCKET sockTCP, 
	enum battleField playerBattleField[][FIELDSIDE], 
	struct coord shipPositions[6][4], 
	char move[MOVESIZE]
) 
{
	enum moveResult result = WRONG_MOVE;
	struct coord axesField;
	recv(sockTCP, &move[0], MOVESIZE, 0);
	
	if (surrendered(move) == 1) {
		result = SURRENDER;
		return result;
	}
	
	//verify recv movement
	axesField = verifyMove(move);
	
	if(axesField.x == -1 || axesField.y == -1) {
		return result;	//Error
	}
	
	if (playerBattleField[axesField.x][axesField.y] == EMPTY) {
		playerBattleField[axesField.x][axesField.y] = MISS;
		result = MISS_MOVE;
	}
	else if (playerBattleField[axesField.x][axesField.y] == SHIP) {
		playerBattleField[axesField.x][axesField.y] = HIT;
		result = isKill(shipPositions, axesField, playerBattleField);
	}
	
	return result;
}
//---------------------------------------------------------------------------
enum moveResult recvResult(SOCKET sockTCP) 
{
	char recvBuff[MOVESIZE];
	enum moveResult result = WRONG_MOVE;
	recv(sockTCP, &recvBuff[0], sizeof(recvBuff) - 1, 0);
	if (strncmp(recvBuff, "WRONG_MOVE", strlen("WRONG_MOVE")) == 0) {
		result = WRONG_MOVE;
	} else if (strncmp(recvBuff, "MISS_MOVE", strlen("MISS_MOVE")) == 0) {
		result = MISS_MOVE;
	} else if (strncmp(recvBuff, "HIT_MOVE", strlen("HIT_MOVE")) == 0) {
		result = HIT_MOVE;
	} else if (strncmp(recvBuff, "KILL_MOVE", strlen("KILL_MOVE")) == 0) {
		result = KILL_MOVE;
	} else if (strncmp(recvBuff, "SURRENDER", strlen("SURRENDER")) == 0) {
		result = SURRENDER;
	}
	
	return result;
}
//---------------------------------------------------------------------------
void fillMissMovesToPlayer	
(	struct coord shipPositions[6][4], 
	struct coord lastMove, 
	enum battleField battleField[][FIELDSIDE]
) 
{
	int currentI = 0, currentJ = 0, currentX = 0, currentY = 0, stopSearch = 0;
	for (int i = 0; i < 4 && stopSearch < 1; i++) {
		for (int j = 0; j < 6 && stopSearch < 1; j++) {
			if (shipPositions[j][i].x == -1 && shipPositions[j][i].y == -1) {
				continue;
			}
			if (shipPositions[j][i].x == lastMove.x && shipPositions[j][i].y == lastMove.y) {
				currentI = i;
				currentJ = j;	
				stopSearch++;
			}
		}
	}
	//go to begin of ship in shipPositions
	currentJ = currentJ - (currentJ % (currentI + 1));
	for (int j = 0; j < currentI + 1; j++) {
		//moving around x'es of killed ship
		for (int x = -1; x <= 1; x++) {
			currentX = shipPositions[currentJ + j][currentI].x + x;
			if (currentX <= 9 && currentX >= 0) {
				//moving aroud y'es of killed ships
				for (int y = -1; y <= 1; y++) {
					currentY = shipPositions[currentJ + j][currentI].y + y;
					if (currentY <= 9 && currentY >=0) {
						if (battleField[currentX][currentY] == EMPTY) {
							battleField[currentX][currentY] = MISS;
						}
					}
				}
			}
		}
	}
}
//---------------------------------------------------------------------------
enum vector changeVector (enum vector input) 
{
	enum vector result;
	switch (input) {
	case UP:
		result = RIGHT;
		break;
	case RIGHT:
		result = DOWN;
		break;
	case DOWN:
		result = LEFT;
		break;
	case LEFT:
		result = UP;
		break;
	}
	
	return result;
}
//---------------------------------------------------------------------------
void fillMissMovesToEnemy	
(	struct coord lastMove, 
	enum battleField battleField[][FIELDSIDE]
) 
{
	enum vector vector = UP;
	int currentX = lastMove.x, currentY = lastMove.y;
	do {
		for (int i = 0; i < TOTALSHIPTYPES; i++) {
			currentX = lastMove.x; 
			currentY = lastMove.y;
			switch(vector) {
			case UP:
				currentY -= i;
				break;
			case RIGHT:
				currentX += i;
				break;
			case DOWN:
				currentY += i;
				break;
			case LEFT:
				currentX -= i;
				break;
			}
			
			if (currentY < 0 && currentY > 9 && currentX < 0 && currentX > 9) {
				i = TOTALSHIPTYPES;
				break;
			}
			if (battleField[currentX][currentY] != HIT) {
				i = TOTALSHIPTYPES;
				break;
			}
			for (int x = -1; x <= 1; x++) {
				currentX += x;
				if (currentX <= 9 && currentX >= 0) {
					for (int y = -1; y <= 1; y++) {
						currentY += y;
						if (currentY <= 9 && currentY >=0) {
							if (battleField[currentX][currentY] == EMPTY) {
								battleField[currentX][currentY] = MISS;
							}
						}
						currentY -= y;
					}
				}
				currentX -= x;
			}
		}
		vector = changeVector(vector);
	} while (vector != UP);
}
//---------------------------------------------------------------------------
enum moveResult makeMove	
(	char *move, 
	enum battleField enemyBattleField[][FIELDSIDE], 
	SOCKET sockTCP
)
{
	enum moveResult result = WRONG_MOVE;
	struct coord axesField;
	//if player surrender
	if (surrendered(move) == 1) {
		result = SURRENDER;
		send(sockTCP, &move[0], strlen(move), 0);
		return result;
	}
	
	//Verify movement
	axesField = verifyMove(move);
	
	if (axesField.x == -1 || axesField.y == -1) {
		return result;
	}
	
	// Move verified, make it
	if (enemyBattleField[axesField.x][axesField.y] == EMPTY) {
		send(sockTCP, &move[0], strlen(move), 0);
		result = recvResult(sockTCP);
		switch (result) {
		case MISS_MOVE:
			enemyBattleField[axesField.x][axesField.y] = MISS;
			break;
		case HIT_MOVE:
			enemyBattleField[axesField.x][axesField.y] = HIT;
			break;
		case KILL_MOVE:
			enemyBattleField[axesField.x][axesField.y] = HIT;
			break;
		case SURRENDER:
		case WRONG_MOVE:
		default:
			break;
		}
	}
	return result;
}
//---------------------------------------------------------------------------
void moveUp	
(	enum battleField playerBattleField[][FIELDSIDE], 
	struct coord shipPositions[6][4], 
	const int shipType, 
	const int shipCount
) 
{
	int collision = 0;
	for (int i = 0; i <= shipType; i++) {
		if (shipPositions[(shipType + 1) * shipCount + i][shipType].x == 0) {
			collision++;
		}
	}
	if (!collision) {
		for (int i = 0; i <= shipType; i++) {
			shipPositions[(shipType + 1) * shipCount + i][shipType].x--;
		}
	}
}
//---------------------------------------------------------------------------
void moveLeft	
(	enum battleField playerBattleField[][FIELDSIDE], 
	struct coord shipPositions[6][4], 
	const int shipType, 
	const int shipCount
) 
{
	int collision = 0;
	for (int i = 0; i <= shipType; i++) {
		if (shipPositions[(shipType + 1) * shipCount + i][shipType].y == 0) {
			collision++;
		}
	}
	if (!collision) {
		for (int i = 0; i <= shipType; i++) {
			shipPositions[(shipType + 1) * shipCount + i][shipType].y--;
		}
	}
	
}
//---------------------------------------------------------------------------
void moveDown	
(	enum battleField playerBattleField[][FIELDSIDE], 
	struct coord shipPositions[6][4], 
	const int shipType, 
	const int shipCount
) 
{
	int collision = 0;
	for (int i = 0; i <= shipType; i++) {
		if (shipPositions[(shipType + 1) * shipCount + i][shipType].x == 9) {
			collision++;
		}
	}
	if (!collision) {
		for (int i = 0; i <= shipType; i++) {
			shipPositions[(shipType + 1) * shipCount + i][shipType].x++;
		}
	}
}
//---------------------------------------------------------------------------
void moveRight	
(	enum battleField playerBattleField[][FIELDSIDE], 
	struct coord shipPositions[6][4], 
	const int shipType, 
	const int shipCount
) 
{
	int collision = 0;
	for (int i = 0; i <= shipType; i++) {
		if (shipPositions[(shipType + 1) * shipCount + i][shipType].y == 9) {
			collision++;
		}
	}
	if (!collision) {
		for (int i = 0; i <= shipType; i++) {
			shipPositions[(shipType + 1) * shipCount + i][shipType].y++;
		}
	}
	
}
//---------------------------------------------------------------------------
void rotateShip	
(	enum battleField playerBattleField[][FIELDSIDE], 
	struct coord shipPositions[6][4], 
	const int shipType, 
	const int shipCount
) 
{
	struct coord firstDeck = shipPositions[(shipType + 1) * shipCount][shipType];
	struct coord lastDeck = shipPositions[(shipType + 1) * shipCount + shipType][shipType];
	//not rotate one-deck ships
	if (shipType > 0) {
		if (firstDeck.x == lastDeck.x) {
			if (firstDeck.y > lastDeck.y) {
				if (lastDeck.x  + shipType < FIELDSIDE) {
					for (int k = 0; k <= shipType; k++) {
						shipPositions[(shipType + 1) * shipCount + k][shipType].y = firstDeck.y;
						shipPositions[(shipType + 1) * shipCount + k][shipType].x += k;
					}
				}
			} 
			else {
				if (lastDeck.x - shipType < FIELDSIDE) {
					for (int k = 0; k <= shipType; k++) {
						shipPositions[(shipType + 1) * shipCount + k][shipType].y = firstDeck.y;
						shipPositions[(shipType + 1) * shipCount + k][shipType].x -= k;
					}
				}
			}
		} 
		else {
			if (firstDeck.x <= lastDeck.x) {
				if (lastDeck.y + shipType < FIELDSIDE) {
					for (int k = 0; k <= shipType; k++) {
						shipPositions[(shipType + 1) * shipCount + k][shipType].x = firstDeck.x;
						shipPositions[(shipType + 1) * shipCount + k][shipType].y += k;
					}
				}
			} 
			else {
				if (lastDeck.y - shipType < FIELDSIDE) {
					for (int k = 0; k <= shipType; k++) {
						shipPositions[(shipType + 1) * shipCount + k][shipType].x = firstDeck.x;
						shipPositions[(shipType + 1) * shipCount + k][shipType].y -= k;
					}
				}
			}
		}
	}
}
//---------------------------------------------------------------------------
void writeShipPositions	
(	enum battleField playerBattleField[][FIELDSIDE], 
	struct coord shipPositions[6][4]
) 
{
	for (int i = 0; i < FIELDSIDE; i++) {
		for (int j = 0; j < FIELDSIDE; j++) {
			if (playerBattleField[i][j] == SHIP) {
				playerBattleField[i][j] = EMPTY;
			}
		}
	}
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 4; j++) {
			if(shipPositions[i][j].x >= 0 && shipPositions[i][j].y >= 0 && shipPositions[i][j].x < 10 && shipPositions[i][j].y < 10) {
				playerBattleField[shipPositions[i][j].x][shipPositions[i][j].y] = SHIP;
			}
		}
	}
}
//---------------------------------------------------------------------------
int anyCollision
(	struct coord shipPositions[6][4], 
	const int currentShipType, 
	const int currentShipCount
) 
{
	int result = 0;
	const int currentI = (currentShipType + 1) * currentShipCount;
	const int currentJ = currentShipType;
	struct coord positonToSearch;
	for (int j = 0; j < 4; j++) {
		for (int i = 0; i < 6; i++) {
			if (shipPositions[i][j].x == -1 && shipPositions[i][j].y == -1) {
				continue;
			}
			if (!(j == currentJ && i == currentI)) {
				for (int k = 0; k <= currentShipType; k++) {
					for (int x = -1; x <= 1; x++) {
						for (int y = -1; y <= 1; y++) {
							positonToSearch.x = shipPositions[currentI + k][currentJ].x + x;
							positonToSearch.y = shipPositions[currentI + k][currentJ].y + y;
							if (positonToSearch.x == shipPositions[i][j].x && positonToSearch.y == shipPositions[i][j].y) {
								result++;
							}
						}
					}
				}
			} else {
				return result;
			}
		}
	}
}
//---------------------------------------------------------------------------
void setBattleField	
(	enum battleField playerBattleField[][FIELDSIDE], 
	struct coord shipPositions[6][4]
)
{
	char getLetter;
	int shipCount = 0;
	int isNewShip = 1;
	for (int shipType = 0; shipType < TOTALSHIPTYPES; shipType++) {		//0 - one-deck, 1 - two-deck, 2 - three-deck, 3 - four-deck
		while(shipType + shipCount < TOTALSHIPTYPES) {	
			if (isNewShip == 1) {
				isNewShip = 0;
				for (int k = 0; k <= shipType; k++) {
					shipPositions[(shipType + 1) * shipCount + k][shipType].x = k;
					shipPositions[(shipType + 1) * shipCount + k][shipType].y = 0;
				}
			}
			writeShipPositions(playerBattleField, shipPositions);
			drawBattleField(playerBattleField, 0, 1, 0, 0, 0, 0);
			while(!kbhit());
			getLetter = getche();
			switch (getLetter) {
			case 'w':
				moveUp(playerBattleField, shipPositions, shipType, shipCount);
				break;
			case 'a':
				moveLeft(playerBattleField, shipPositions, shipType, shipCount);
				break;
			case 's':
				moveDown(playerBattleField, shipPositions, shipType, shipCount);
				break;
			case 'd':
				moveRight(playerBattleField, shipPositions, shipType, shipCount);
				break;
			case 'r':
				rotateShip(playerBattleField, shipPositions, shipType, shipCount);
				break;
			case 'e':
				if (anyCollision(shipPositions, shipType, shipCount) == 0) {
					shipCount++;
					isNewShip++;
				}
				break;
			default:
				break;
			}
		}
		shipCount = 0;
	}
}
//---------------------------------------------------------------------------