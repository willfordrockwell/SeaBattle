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

enum battleField {EMPTY, MISS, HIT};
enum turn {PLAYER, ENEMY};
//---------------------------------------------------------------------------
void checkArgs(int argc) {	//Num of args
	if (argc < 2) {
		printf("Enter argument: 0 for client (Second player), 1 for server (First player)");
		exit(-1);
	}
}
//---------------------------------------------------------------------------
void initClient() {
	char serverIP[IPLENGTH], serverPort[PORTLENGTH];
	printf("Enter server's IP (example: 127.0.0.1): ");
	scanf("%s", &serverIP);
	printf("Enter server's port (example: 12345): ");
	scanf("%s", &serverPort);
	//TODO: VERIFY STRINGS
	
	//Init Winsock
	struct WSAData WS;
	if (FAILED(WSAStartup(0x202, (WSADATA *)&WS))) {
		//Error
		printf("Client can NOT initialize WSAStartup, error: %d\n", WSAGetLastError());
		exit(-3);
	}
	
	//Create tcp socket
	SOCKET sockTCP;
	if ((sockTCP = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
		//Error
		printf("Client can NOT create socket, error: %d\n", WSAGetLastError());
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
        printf("Connect error %d\n", WSAGetLastError());
        exit(-5);
    }
	printf("Connect with %s succed\n\n", serverIP);
}
//---------------------------------------------------------------------------
void initServer() {
	char serverPort[PORTLENGTH];
	const int queueLength = 1;
	printf("Enter server's port (example: 12345): ");
	scanf("%s", &serverPort);
	
	//Init Winsock
	struct WSAData WS;
	if (FAILED(WSAStartup(0x202, (WSADATA *)&WS))) {
		//Error
		printf("Client can NOT initialize WSAStartup, error: %d\n", WSAGetLastError());
		exit(-3);
	}
	
	//Create tcp socket
	SOCKET sockTCP;
	if ((sockTCP = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
		//Error
		printf("Client can NOT create socket, error: %d\n", WSAGetLastError());
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
	if (bind(sockTCP,(struct sockaddr *) &local_addr,sizeof(local_addr))) {
		//Error
		printf("Error bind %d\n",WSAGetLastError());
		closesocket(sockTCP);  							//Close socket
		WSACleanup();
		exit(-5);
    }
	//Init queue to listen from clients
	if (listen(sockTCP, queueLength)) {
		//Error
		printf("Error listen %d\n",WSAGetLastError());
		closesocket(sockTCP);
		WSACleanup();
		exit(-6);
    }
}
//---------------------------------------------------------------------------
void initConnection(char *argv[]){
	if (atoi(argv[1]) == 0) {
		//Client code
		initClient();
	}
	else if (atoi(argv[1]) == 1) {
		//Server code
		initServer();
	}
	else {
		//WTF code
		printf("Enter argument: 0 for client (Second player), 1 for server (First player)");
		exit(-2);
	}
}
//---------------------------------------------------------------------------
void drawBattleField(
					enum battleField playerBattleField[][FIELDSIDE], 
					enum battleField enemyBattleField[][FIELDSIDE], 
					unsigned int playerScore, 
					unsigned int enemyScore, 
					enum turn playerTurn, 
					char *move){
	system("cls");
	printf("   0|1|2|3|4|5|6|7|8|9|   0|1|2|3|4|5|6|7|8|9|\n");
	for (int i = 0; i < FIELDSIDE; i++){
		printf("  ---------------------  ---------------------\n");
		//printf("  # # # # # # # # # # #  # # # # # # # # # # #\n");
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
				if (enemyBattleField[i][j - FIELDSIDE] == EMPTY)		{ printf(" |");}
				else if (enemyBattleField[i][j - FIELDSIDE] == MISS)	{ printf("*|");}
				else 													{ printf("X|");}
			}
		}
		//printf("\n  # # # # # # # # # # #  # # # # # # # # # # #\n");
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

//---------------------------------------------------------------------------
int main (int argc, char *argv[]) {	//Server: 1; Client: 0
	checkArgs(argc);
	initConnection(argv);
	enum battleField playerBattleField[FIELDSIDE][FIELDSIDE], enemyBattleField[FIELDSIDE][FIELDSIDE];
	for(int i = 0; i < FIELDSIDE; i++)	{		//Init battlefields with EMPTY
		for(int j = 0; j < FIELDSIDE; j++)	{
			playerBattleField[i][j] = EMPTY;
			enemyBattleField[i][j] = EMPTY;
		}
	}
	enum turn playerTurn;
	if (atoi(argv[1]) == 1)	playerTurn = PLAYER;
	else				playerTurn = ENEMY;
	int endGame = 0;
	unsigned int playerScore, enemyScore = 0;
	char move[MOVESIZE];
	while(!endGame) {
		drawBattleField(playerBattleField, enemyBattleField, playerScore, enemyScore, playerTurn, move);
	}
	return 0;
}