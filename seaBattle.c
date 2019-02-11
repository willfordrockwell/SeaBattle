//---------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <winsock2.h>
#include <windows.h> 

#define IPLENGTH 16
#define PORTLENGTH 5

#define FIELDSIDE 10

#define MOVESIZE 10
#define AXESSIZE 3

enum battleField {EMPTY, MISS, HIT, SHIP};
enum turn {PLAYER, ENEMY};
enum moveResult {WRONG_MOVE, MISS_MOVE, HIT_MOVE, KILL_MOVE, SURRENDER};
struct axes {int x; int y;} axesField;
//---------------------------------------------------------------------------
void checkArgs(int argc) {	//Num of args
	if (argc < 2) {
		printf("Enter argument: 0 for client (Second player), 1 for server (First player)");
		exit(-1);
	}
}
//---------------------------------------------------------------------------
SOCKET initClient() {
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
SOCKET initServer() {
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
	if (bind(sockTCP,(struct sockaddr *) &local_addr,sizeof(local_addr))) {	//Error
		printf("Error bind %d\n",WSAGetLastError());
		closesocket(sockTCP);  							//Close socket
		WSACleanup();
		exit(-5);
    }
	//Init queue to listen from clients
	if (listen(sockTCP, queueLength)) {
		printf("Error listen %d\n",WSAGetLastError());	//Error
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
SOCKET initConnection(char *argv[]){
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
void drawBattleField(enum battleField playerBattleField[][FIELDSIDE], enum battleField enemyBattleField[][FIELDSIDE], unsigned int playerScore, unsigned int enemyScore, enum turn playerTurn, char *move){
	system("cls");
	printf("   0|1|2|3|4|5|6|7|8|9|   0|1|2|3|4|5|6|7|8|9|\n");
	for (int i = 0; i < FIELDSIDE; i++){
		printf("  ---------------------  ---------------------\n");
		printf("%c |", i + 65);
		for (int j = 0; j < 2 * FIELDSIDE + 1; j++){
			if (j < FIELDSIDE)
				if (playerBattleField[i][j] == EMPTY)		{ printf(" |");}
				else if (playerBattleField[i][j] == MISS)	{ printf("*|");}
				else 										{ printf("X|");}
			else if (j == FIELDSIDE) {
				printf(" %c|", i + 65);
			}
			else if (j > FIELDSIDE)	{
				if (enemyBattleField[i][j - FIELDSIDE - 1] == EMPTY)		{ printf(" |");}
				else if (enemyBattleField[i][j - FIELDSIDE - 1] == MISS)	{ printf("*|");}
				else 													{ printf("X|");}
			}
		}
		printf("\n");
	}
	printf("  ---------------------  ---------------------\n");
	printf("Your score: %d\nEnemy score: %d\n", playerScore, enemyScore);
	if (playerTurn == PLAYER) 	{
		printf("Your turn\nEnter position for FIRE like 'A4' or \n'SURRENDER' to SURRENDER:");
		scanf("%s", move);
	}
	else {
		printf("Wait for your enemy to FIRE");
	}
}
//---------------------------------------------------------------------------
enum turn changeTurn(enum turn playerTurn){
	enum turn temp;
	if (playerTurn == ENEMY)
		temp = PLAYER;
	else
		temp = ENEMY;
	return temp;
}
//---------------------------------------------------------------------------
/* void sendMove(char *buffSend, SOCKET sockTCP) {
	send(sockTCP, &buffSend[0], strlen(&buffSend[0]), 0);
} */
//---------------------------------------------------------------------------
struct axes verifyMove(char *move){
	struct axes result;
	result.x = -1;
	result.y = -1;
	if (move[0] >= 'A' && move[0] < 'K')
		result.x = move[0] - 'A';
	else if (move[0] >= 'a' && move[0] < 'k')
		result.x = move[0] - 'a';
	else
		return result;	//Error
	
	if (move[1] >= '0' && move[1] < ':')
		result.y = move[1] - '0';
	else
		return result;	//Error
	return result;
}
//---------------------------------------------------------------------------
int surrendered (char *move, int *endGame) {
	if(strncmp(move, "SURRENDER", strlen("SURRENDER")) == 0) {	
		(*endGame)++;
		return 1;
	}
	else 
		return 0;
}
//---------------------------------------------------------------------------
enum moveResult recvMove(SOCKET sockTCP, enum battleField playerBattleField[][FIELDSIDE], int *endGame) {
	char recvBuff[MOVESIZE];
	enum moveResult result = WRONG_MOVE;
	struct axes axesField;
	recv(sockTCP, &recvBuff[0], sizeof(recvBuff) - 1, 0);
	
	if (surrendered (recvBuff, endGame)) {
		result = SURRENDER;
		return result;
	}
	
	//verivy recv movement
	axesField = verifyMove(recvBuff);
	
	if(axesField.x == -1 || axesField.y == -1)
		return result;	//Error
	
	if (playerBattleField[axesField.x][axesField.y] == EMPTY) {
		playerBattleField[axesField.x][axesField.y] = MISS;
		result = MISS_MOVE;
	}
	else if (playerBattleField[axesField.x][axesField.y] == SHIP) {
		playerBattleField[axesField.x][axesField.y] = HIT;
		result = HIT_MOVE;	//Check is KILL_MOVE?
	}
	
	return result;
}
//---------------------------------------------------------------------------
enum moveResult recvResult(SOCKET sockTCP) {
	char recvBuff[MOVESIZE];
	enum moveResult result = WRONG_MOVE;
	recv(sockTCP, &recvBuff[0], sizeof(recvBuff) - 1, 0);
	if (strncmp(recvBuff, "WRONG_MOVE", strlen("WRONG_MOVE")) == 0)
		result = WRONG_MOVE;
	else if (strncmp(recvBuff, "MISS_MOVE", strlen("MISS_MOVE")) == 0)
		result = MISS_MOVE;
	else if (strncmp(recvBuff, "HIT_MOVE", strlen("HIT_MOVE")) == 0)
		result = HIT_MOVE;
	else if (strncmp(recvBuff, "KILL_MOVE", strlen("KILL_MOVE")) == 0)
		result = KILL_MOVE;
	else if (strncmp(recvBuff, "SURRENDER", strlen("SURRENDER")) == 0)
		result = SURRENDER;
	return result;
}
//---------------------------------------------------------------------------
enum moveResult makeMove(char *move, int* endGame, enum battleField enemyBattleField[][FIELDSIDE], SOCKET sockTCP){
	enum moveResult result = WRONG_MOVE;
	enum battleField field;
	struct axes axesField;
	//if player surrender
	if (surrendered (move, endGame)){
		result = SURRENDER;
		return result;
	}
	
	//Verify movement
	axesField = verifyMove(move);
	
	if (axesField.x == -1 || axesField.y == -1)
		return result;
	
	// Move verified, make it
	if (enemyBattleField[axesField.x][axesField.y] == EMPTY) {
		send(sockTCP, &move[0], strlen(&move[0]), 0);	//sendMove(move, sockTCP);
		result = recvResult(sockTCP);
		switch (result)
		{
			case MISS_MOVE:
				enemyBattleField[axesField.x][axesField.y] = MISS;
				break;
			case HIT_MOVE:
				enemyBattleField[axesField.x][axesField.y] = HIT;
				break;
			case KILL_MOVE:
				enemyBattleField[axesField.x][axesField.y] = HIT;
				//make MISS around
				break;
			/* case SURRENDER :
				surrendered (move, &endGame)
				break; */
			case WRONG_MOVE:
			default:
				break;
		}
	}
	return result;
}
//---------------------------------------------------------------------------
int main (int argc, char *argv[]) {	//Server: 1; Client: 0
	checkArgs(argc);
	SOCKET sockTCP = initConnection(argv);
	enum battleField playerBattleField[FIELDSIDE][FIELDSIDE], enemyBattleField[FIELDSIDE][FIELDSIDE];
	for(int i = 0; i < FIELDSIDE; i++)	{		//Init battlefields with EMPTY
		for(int j = 0; j < FIELDSIDE; j++)	{
			playerBattleField[i][j] = EMPTY;
			enemyBattleField[i][j] = EMPTY;
		}
	}
	enum turn playerTurn;
	if (atoi(argv[1]) == 1)	
		playerTurn = PLAYER;
	else
		playerTurn = ENEMY;
	int endGame = 0;
	unsigned int playerScore = 0, enemyScore = 0;
	char move[MOVESIZE];
	enum moveResult result;
	while(!endGame) {
		if (playerTurn == PLAYER) {
			do {
				drawBattleField(playerBattleField, enemyBattleField, playerScore, enemyScore, playerTurn, move);
				result = makeMove(move, &endGame, enemyBattleField, sockTCP);
			} while (result != SURRENDER && result != MISS_MOVE);
		}
		else {
			do {
				drawBattleField(playerBattleField, enemyBattleField, playerScore, enemyScore, playerTurn, move);
				result = recvMove(sockTCP, playerBattleField, &endGame);
				switch(result)
				{
					case WRONG_MOVE:
						send(sockTCP, "WRONG_MOVE", strlen("WRONG_MOVE"), 0);
						break;
					case MISS_MOVE:
						send(sockTCP, "MISS_MOVE", strlen("MISS_MOVE"), 0);
						break;
					case HIT_MOVE:
						send(sockTCP, "HIT_MOVE", strlen("HIT_MOVE"), 0);
						break;
					case KILL_MOVE:
						send(sockTCP, "KILL_MOVE", strlen("KILL_MOVE"), 0);
						break;
					case SURRENDER:
						send(sockTCP, "SURRENDER", strlen("SURRENDER"), 0);
					break;
					default:
						break;
				}
			} while (result !=SURRENDER && result != MISS_MOVE);
		}
		playerTurn = changeTurn(playerTurn);
	}
	return 0;
}