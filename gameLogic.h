#ifndef __GAME_LOGIC_H__
#define __GAME_LOGIC_H__

#define FIELDSIDE 10

#define MOVESIZE 11
#define AXESSIZE 3

#define TOTALSHIPTYPES 4

#include "graphics.h"

enum moveResult 
{
	WRONG_MOVE, 
	MISS_MOVE, 
	HIT_MOVE, 
	KILL_MOVE, 
	SURRENDER
};

enum vector 
{
	UP, 
	RIGHT, 
	DOWN, 
	LEFT
};

struct coord 
{
	int x; 
	int y;
} axesField;

enum turn changeTurn(enum turn playerTurn);

struct coord verifyMove(char *move);

enum moveResult recvMove
(	SOCKET sockTCP, 
	enum battleField playerBattleField[5][FIELDSIDE], 
	struct coord shipPositions[6][4], 
	char move[MOVESIZE]
);

void fillMissMovesToPlayer	
(	struct coord shipPositions[6][4], 
	struct coord lastMove, 
	enum battleField battleField[][FIELDSIDE]
);

void fillMissMovesToEnemy	
(	struct coord lastMove, 
	enum battleField battleField[][FIELDSIDE]
);

enum moveResult makeMove	
(	char *move, 
	enum battleField enemyBattleField[][FIELDSIDE], 
	SOCKET sockTCP
);

void setBattleField	
(	enum battleField playerBattleField[][FIELDSIDE], 
	struct coord shipPositions[6][4]
);
#endif