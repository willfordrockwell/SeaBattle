//---------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <winsock2.h>
#include <windows.h> 
#include <conio.h>

#define IPLENGTH 16
#define PORTLENGTH 5

#define FIELDSIDE 10

#define MOVESIZE 10
#define AXESSIZE 3

#define TOTALSHIPTYPES 4

#define FOREGROUND_WHITE FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE
#define FOREGROUND_YELLOW FOREGROUND_RED | FOREGROUND_GREEN

/// Все перечисления лучше объявить через typedef  вынестив  загловочный файл
// программу лучше разделить на несколько отдельных модулей
// сетевой
// отрисовка
// игровая логика
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
// typedef
struct coord
{
	int x;
	int y;
} axesField;
//---------------------------------------------------------------------------
void checkArgs(int argc)
{
	if (argc < 2) {
		printf("Enter argument: 0 for client (Second player), 1 for server (First player)");
		exit(-1);
	}
}
//---------------------------------------------------------------------------
SOCKET initClient()
{
	char serverIP[IPLENGTH], serverPort[PORTLENGTH];
	printf("Enter server's IP (example: 127.0.0.1): ");
	scanf("%s", &serverIP);
	printf("Enter server's port (example: 12345): ");
	scanf("%s", &serverPort);
	//TODO: VERIFY STRINGS

	//Init Winsock
	struct WSAData WS;
	if (FAILED(WSAStartup(0x202, (WSADATA *)&WS))) {
		printf("Client can NOT initialize WSAStartup, error: %d\n", WSAGetLastError());	//Error
		exit(-3);
	}

	//Create tcp socket
	SOCKET sockTCP;
	if ((sockTCP = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
		printf("Client can NOT create socket, error: %d\n", WSAGetLastError());	//Error
		exit(-4);
	}

	//Fill struct sockaddr_in
	struct sockaddr_in dest_addr;						//struck holding info about server
	ZeroMemory(&dest_addr, sizeof(dest_addr));
	dest_addr.sin_family = AF_INET;						//Using IPv4
	dest_addr.sin_port = htons(atoi(serverPort));		//convert port from symbol to nework format
	dest_addr.sin_addr.s_addr = inet_addr(serverIP); 	//convert IP from symbol to network format

														//Try to connect
	if (connect(sockTCP, (struct sockaddr *)&dest_addr, sizeof(dest_addr))) {
		printf("Connect error %d\n", WSAGetLastError());	//Error
		exit(-5);
	}

	return sockTCP;
}
//---------------------------------------------------------------------------
SOCKET initServer()
{
	char serverPort[PORTLENGTH];
	const int queueLength = 1;
	printf("Enter server's port (example: 12345): ");
	scanf("%s", &serverPort);

	//Init Winsock
	struct WSAData WS;
	if (FAILED(WSAStartup(0x202, (WSADATA *)&WS))) {
		printf("Client can NOT initialize WSAStartup, error: %d\n", WSAGetLastError());	//Error
		exit(-3);
	}

	//Create tcp socket
	SOCKET sockTCP;
	if ((sockTCP = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
		printf("Client can NOT create socket, error: %d\n", WSAGetLastError());	//Error
		WSACleanup();
		exit(-4);
	}

	//Fill struct sockaddr_in
	struct sockaddr_in local_addr;						//struck holding info about server
	ZeroMemory(&local_addr, sizeof(local_addr));
	local_addr.sin_family = AF_INET;					//Using IPv4
	local_addr.sin_port = htons(atoi(serverPort));		//convert port from symbol to nework format
	local_addr.sin_addr.s_addr = INADDR_ANY; 			//Any connection to server

														//Bind to connect
	if (bind(sockTCP, (struct sockaddr *) &local_addr, sizeof(local_addr))) {	//Error
		printf("Error bind %d\n", WSAGetLastError());
		closesocket(sockTCP);  							//Close socket
		WSACleanup();
		exit(-5);
	}
	//Init queue to listen from clients
	if (listen(sockTCP, queueLength)) {
		printf("Error listen %d\n", WSAGetLastError());	//Error
		closesocket(sockTCP);
		WSACleanup();
		exit(-6);
	}

	//Get client's socket
	SOCKET clientSock;
	struct sockaddr_in clientAddr;
	int clientAddrSize = sizeof(clientAddr);

	clientSock = accept(sockTCP, (struct sockaddr *) &clientAddr, &clientAddrSize);

	return clientSock;
}
//---------------------------------------------------------------------------
SOCKET initConnection(char *argv[])
{
	if (atoi(argv[1]) == 0) {
		//Client code
		return initClient();
	}
	else if (atoi(argv[1]) == 1) {
		//Server code
		return initServer();
	}
	else {
		//WTF code
		printf("Enter argument: 0 for client (Second player), 1 for server (First player)");
		exit(-2);
	}
}
//---------------------------------------------------------------------------
void drawBattleField
(enum battleField playerBattleField[][FIELDSIDE],
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
				}
				else if (playerBattleField[i][j] == MISS) {
					SetConsoleTextAttribute(hConsole, FOREGROUND_YELLOW);
					printf("*");
					SetConsoleTextAttribute(hConsole, FOREGROUND_WHITE);
					printf("|");
				}
				else if (playerBattleField[i][j] == HIT) {
					SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
					printf("X");
					SetConsoleTextAttribute(hConsole, FOREGROUND_WHITE);
					printf("|");
				}
				else {
					SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
					printf("#");
					SetConsoleTextAttribute(hConsole, FOREGROUND_WHITE);
					printf("|");
				}
			}
			else if (j == FIELDSIDE && isSet == 0) {
				printf(" %c|", i + 65);
			}
			else if (j > FIELDSIDE && isSet == 0) {
				if (enemyBattleField[i][j - FIELDSIDE - 1] == EMPTY) {
					printf(" |");
				}
				else if (enemyBattleField[i][j - FIELDSIDE - 1] == MISS) {
					SetConsoleTextAttribute(hConsole, FOREGROUND_YELLOW);
					printf("*");
					SetConsoleTextAttribute(hConsole, FOREGROUND_WHITE);
					printf("|");
				}
				else if (enemyBattleField[i][j - FIELDSIDE - 1] == HIT) {
					SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
					printf("X");  SetConsoleTextAttribute(hConsole, FOREGROUND_WHITE);
					printf("|");
				}
				else {
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
				scanf("%s", move);
			}
			else {
				printf("Wait for your enemy to FIRE\n");
				move = "\n";
			}
		}
	}
	else {
		printf("\nSet your ships:\nWASD to change position\nr to rotate clockwise\ne to set");
	}
}
//---------------------------------------------------------------------------
enum turn changeTurn(enum turn playerTurn)
{
	enum turn temp;
	if (playerTurn == ENEMY) {
		temp = PLAYER;
	}
	else {
		temp = ENEMY;
	}
	return temp;
	// rerurn (playerTurn == ENEMY) ? PLAYER : ENEMY;
}
//---------------------------------------------------------------------------
// название функции не сооотвествует ее работе.
struct coord verifyMove(char *move)
{
	struct coord result;
	result.x = -1;
	result.y = -1;
	if (move[0] >= 'A' && move[0] <= 'J') {
		result.x = move[0] - 'A';
	}
	else if (move[0] >= 'a' && move[0] <= 'j') {
		result.x = move[0] - 'a';
	}
	else {
		return result;	//Error
	}

	if (move[1] >= '0' && move[1] <= '9') {
		result.y = move[1] - '0';
	}
	else {
		return result;	//Error
	}

	return result;
}
//---------------------------------------------------------------------------
// лучше назвать isSurrended 
int surrendered(char *move)
{
	if (strncmp(move, "SURRENDER", strlen("SURRENDER")) == 0) {
		return 1;
	}
	else {
		return 0;
	}
}
//---------------------------------------------------------------------------
// не совсем разобрался что функция делает.
enum moveResult isKill
(struct coord shipPositions[6][4],
	struct coord lastMove,
	enum battleField playerBattleField[][FIELDSIDE]
)
{
	enum moveResult result = HIT_MOVE;
	int currentI = 0, currentJ = 0, hittedShips = 0, stopSearch = 0;
	///Небольшая оптимизация
	for (int i = 0; i < 4 && stopSearch < 1; i++) {
		for (int j = 0; j < 6; j++) {
			if (shipPositions[j][i].x == -1 && shipPositions[j][i].y == -1) {
				continue;
			}
			if (shipPositions[j][i].x == lastMove.x && shipPositions[j][i].y == lastMove.y) {
				currentI = i;
				currentJ = j;
				stopSearch++;
				break;
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
(SOCKET sockTCP,
	enum battleField playerBattleField[][FIELDSIDE],
	struct coord shipPositions[6][4],
	char move[MOVESIZE]
)
{
	enum moveResult result = WRONG_MOVE;
	struct coord axesField;
	recv(sockTCP, &move[0], MOVESIZE, 0);

	// if (surrendered(move)), сравнение лишнее
	if (surrendered(move) == 1) {
		result = SURRENDER;
		return result;
	}

	//verify recv movement
	axesField = verifyMove(move);

	if (axesField.x == -1 || axesField.y == -1) {
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
	}
	else if (strncmp(recvBuff, "MISS_MOVE", strlen("MISS_MOVE")) == 0) {
		result = MISS_MOVE;
	}
	else if (strncmp(recvBuff, "HIT_MOVE", strlen("HIT_MOVE")) == 0) {
		result = HIT_MOVE;
	}
	else if (strncmp(recvBuff, "KILL_MOVE", strlen("KILL_MOVE")) == 0) {
		result = KILL_MOVE;
	}
	else if (strncmp(recvBuff, "SURRENDER", strlen("SURRENDER")) == 0) {
		result = SURRENDER;
	}

	return result;
}
//---------------------------------------------------------------------------
void fillMissMovesToPlayer
(struct coord shipPositions[6][4],
	struct coord lastMove,
	enum battleField battleField[][FIELDSIDE]
)
{
	// этот алгоритм уже был в isKill - лучше вывести в отдельню функцию
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
	currentJ = currentJ - (currentJ % (currentI + 1));
	for (int j = 0; j < currentI + 1; j++) {
		for (int x = -1; x <= 1; x++) {
			currentX = shipPositions[currentJ + j][currentI].x + x;
			if (currentX <= 9 && currentX >= 0) {
				// размеры поля жестко захардожены. А если придется поменять. Задать макроопредениями лучше
				for (int y = -1; y <= 1; y++) {
					currentY = shipPositions[currentJ + j][currentI].y + y;
					if (currentY <= 9 && currentY >= 0) {
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
enum vector changeVector(enum vector input)
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
(struct coord lastMove,
	enum battleField battleField[][FIELDSIDE]
)
{
	enum vector vector = UP;
	int currentX = lastMove.x, currentY = lastMove.y;
	do {
		for (int i = 0; i < TOTALSHIPTYPES; i++) {
			currentX = lastMove.x;
			currentY = lastMove.y;
			switch (vector) {
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
			// это условие никогда не выполнитсся
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
						if (currentY <= 9 && currentY >= 0) {
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
(char *move,
	enum battleField enemyBattleField[][FIELDSIDE],
	struct coord shipPositions[6][4],
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
	// после верификации, у идет дополнительная проверка?
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
			break;
		case WRONG_MOVE:
			break;
		default:
			break;
		}
	}
	return result;
}
//---------------------------------------------------------------------------
void moveUp
(enum battleField playerBattleField[][FIELDSIDE],
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
(enum battleField playerBattleField[][FIELDSIDE],
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
(enum battleField playerBattleField[][FIELDSIDE],
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
(enum battleField playerBattleField[][FIELDSIDE],
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
(enum battleField playerBattleField[][FIELDSIDE],
	struct coord shipPositions[6][4],
	const int shipType,
	const int shipCount
)
{
	if (shipType > 0) {
		if (shipPositions[(shipType + 1) * shipCount][shipType].x == shipPositions[(shipType + 1) * shipCount + 1][shipType].x) {
			if (shipPositions[(shipType + 1) * shipCount][shipType].y > shipPositions[(shipType + 1) * shipCount + 1][shipType].y) {
				if (shipPositions[(shipType + 1) * shipCount + shipType][shipType].x + (shipType + 1) < FIELDSIDE) {
					for (int k = 0; k <= shipType; k++) {
						shipPositions[(shipType + 1) * shipCount + k][shipType].y = shipPositions[(shipType + 1) * shipCount][shipType].y;
						shipPositions[(shipType + 1) * shipCount + k][shipType].x += k;
					}
				}
			}
			else {
				if (shipPositions[(shipType + 1) * shipCount + shipType][shipType].x - (shipType + 1)< FIELDSIDE) {
					for (int k = 0; k <= shipType; k++) {
						shipPositions[(shipType + 1) * shipCount + k][shipType].y = shipPositions[(shipType + 1) * shipCount][shipType].y;
						shipPositions[(shipType + 1) * shipCount + k][shipType].x -= k;
					}
				}
			}
		}
		else {
			if (shipPositions[(shipType + 1) * shipCount][shipType].x <= shipPositions[(shipType + 1) * shipCount + 1][shipType].x) {
				if (shipPositions[(shipType + 1) * shipCount + shipType][shipType].y + (shipType + 1) < FIELDSIDE) {
					for (int k = 0; k <= shipType; k++) {
						shipPositions[(shipType + 1) * shipCount + k][shipType].x = shipPositions[(shipType + 1) * shipCount][shipType].x;
						shipPositions[(shipType + 1) * shipCount + k][shipType].y += k;
					}
				}
			}
			else {
				if (shipPositions[(shipType + 1) * shipCount + shipType][shipType].y - (shipType + 1)< FIELDSIDE) {
					for (int k = 0; k <= shipType; k++) {
						shipPositions[(shipType + 1) * shipCount + k][shipType].x = shipPositions[(shipType + 1) * shipCount][shipType].x;
						shipPositions[(shipType + 1) * shipCount + k][shipType].y -= k;
					}
				}
			}
		}
	}
}
//---------------------------------------------------------------------------
void writeShipPositions
(enum battleField playerBattleField[][FIELDSIDE],
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
			if (shipPositions[i][j].x >= 0 && shipPositions[i][j].y >= 0 && shipPositions[i][j].x < 10 && shipPositions[i][j].y < 10) {
				playerBattleField[shipPositions[i][j].x][shipPositions[i][j].y] = SHIP;
			}
		}
	}
}
//---------------------------------------------------------------------------
int anyCollision
(struct coord shipPositions[6][4],
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
			}
			else {
				return result;
			}
		}
	}
}
//---------------------------------------------------------------------------
void setBattleField
(enum battleField playerBattleField[][FIELDSIDE],
	struct coord shipPositions[6][4]
)
{
	char getLetter;
	int shipCount = 0;
	int isNewShip = 1;
	for (int shipType = 0; shipType < TOTALSHIPTYPES; shipType++) {		//0 - one-deck, 1 - two-deck, 2 - three-deck, 3 - four-deck
		while (shipType + shipCount < TOTALSHIPTYPES) {
			if (isNewShip == 1) {
				isNewShip = 0;
				for (int k = 0; k <= shipType; k++) {
					shipPositions[(shipType + 1) * shipCount + k][shipType].x = k;
					shipPositions[(shipType + 1) * shipCount + k][shipType].y = 0;
				}
			}
			writeShipPositions(playerBattleField, shipPositions);
			drawBattleField(playerBattleField, 0, 1, 0, 0, 0, 0);
			while (!kbhit());
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
int main(int argc, char *argv[])
{	//Server: 1; Client: 0
	checkArgs(argc);
	SOCKET sockTCP = initConnection(argv);
	enum battleField playerBattleField[FIELDSIDE][FIELDSIDE];
	enum battleField enemyBattleField[FIELDSIDE][FIELDSIDE];
	// вывести в отдельную функцию
	for (int i = 0; i < FIELDSIDE; i++) {		//Init battlefields with EMPTY
		for (int j = 0; j < FIELDSIDE; j++) {
			playerBattleField[i][j] = EMPTY;
			enemyBattleField[i][j] = EMPTY;
		}
	}
	enum turn playerTurn;
	if (atoi(argv[1]) == 1) {
		playerTurn = PLAYER;
	}
	else {
		playerTurn = ENEMY;
	}
	int endGame = 0, isSurrender = 0;
	unsigned int playerScore = 0, enemyScore = 0;
	char move[MOVESIZE];
	enum moveResult result;
	// позиции корблей - можно задать как структуру координата + оринтация
	// сам корбаль струкура - тип(1,2,3,4) , позиция, вектор состояния.
	struct coord shipPositions[6][4];
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 4; j++) {
			shipPositions[i][j].x = -1;
			shipPositions[i][j].y = -1;
		}
	}
	setBattleField(playerBattleField, shipPositions);
	while (endGame == 0) {
		if (playerTurn == PLAYER) {
			//your turn
			do {
				drawBattleField(playerBattleField, enemyBattleField, 0, playerScore, enemyScore, playerTurn, move);
				result = makeMove(move, enemyBattleField, shipPositions, sockTCP);
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
			} while ((result != SURRENDER && result != MISS_MOVE) && endGame == 0);
		}
		else {
			//enemy turn
			do {
				drawBattleField(playerBattleField, enemyBattleField, 0, playerScore, enemyScore, playerTurn, move);
				result = recvMove(sockTCP, playerBattleField, shipPositions, move);
				switch (result) {
				case KILL_MOVE:
					fillMissMovesToPlayer(shipPositions, verifyMove(move), playerBattleField);
				case HIT_MOVE:
					enemyScore++;
					break;
				case SURRENDER:
					isSurrender++;
					endGame++;
					break;
				default:	//WRONG_MOVE, MISS_MOVE
					break;
				}
				send(sockTCP, result, strlen(result), 0);

				if (playerScore == 20 || enemyScore == 20) {
					endGame++;
				}
			} while ((result != SURRENDER && result != MISS_MOVE) && endGame == 0);
		}
		drawBattleField(playerBattleField, enemyBattleField, 0, playerScore, enemyScore, playerTurn, 0);
		if (isSurrender == 0) {
			playerTurn = changeTurn(playerTurn);
		}
	}
	if (playerTurn == PLAYER) {
		printf("\nYou lose\n");
	}
	else {
		printf("\nYou Win\n");
	}

	return 0;
}