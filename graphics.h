#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#define FIELDSIDE 10
#define MOVESIZE 11

enum battleField 
{
	EMPTY, 
	MISS, 
	HIT, 
	SHIP
};

enum turn 
{
	PLAYER, 
	ENEMY
};
void drawBattleField	
(	enum battleField playerBattleField[][FIELDSIDE], 
	enum battleField enemyBattleField[][FIELDSIDE], 
	int isSet, 
	unsigned int playerScore, 
	unsigned int enemyScore, 
	enum turn playerTurn, 
	char *move
);
#endif