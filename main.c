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
	if (FAILED(WSAStartup(0x202, (WSADATA *)&WS))){
		//Error
		printf("Client can NOT initialize WSAStartup, error: %d\n", WSAGetLastError());
		exit(-3);
	}
	
	//Create tcp socket
	SOCKET sockTCP;
	if ((sockTCP = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET){
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
	printf("Connect with %s succed\nType quit for quit\n\n", serverIP);
}
//---------------------------------------------------------------------------
void initServer() {
	char serverPort[PORTLENGTH];
	const int queueLength = 1;
	printf("Enter server's port (example: 12345): ");
	scanf("%s", &serverPort);
	
	//Init Winsock
	struct WSAData WS;
	if (FAILED(WSAStartup(0x202, (WSADATA *)&WS))){
		//Error
		printf("Client can NOT initialize WSAStartup, error: %d\n", WSAGetLastError());
		exit(-3);
	}
	
	//Create tcp socket
	SOCKET sockTCP;
	if ((sockTCP = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET){
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
	if (bind(sockTCP,(struct sockaddr *) &local_addr,sizeof(local_addr)))	{
		//Error
		printf("Error bind %d\n",WSAGetLastError());
		closesocket(sockTCP);  							//Close socket
		WSACleanup();
		exit(-5);
    }
	//Init queue to listen from clients
	if (listen(sockTCP, queueLength))	{
		//Error
		printf("Error listen %d\n",WSAGetLastError());
		closesocket(sockTCP);
		WSACleanup();
		exit(-6);
    }
}
//---------------------------------------------------------------------------
int main (int argc, char *argv[]) {	//Server: 1 serverPort; Client: 0 serverIP serverPort
	checkArgs(argc);
	
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
		return -2;
	}
	return 0;
}