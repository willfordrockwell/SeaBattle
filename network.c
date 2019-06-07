#include "network.h"

#define IPLENGTH 16
#define PORTLENGTH 6

SOCKET initClient() 
{
	char serverIP[IPLENGTH], serverPort[PORTLENGTH];
	printf("Enter server's IP (auto: 127.0.0.1): ");
	fgets(serverIP, IPLENGTH, stdin);
	if (strncmp(serverIP, "", 1)) {
		strcpy(serverIP, "127.0.0.1");
	}
	printf("Enter server's port (auto: 12345): ");
	fgets(serverPort, PORTLENGTH, stdin);
	if (strncmp(serverPort, "", 1)) {
		strcpy(serverPort, "12345");
	}
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
	printf("Enter server's port (auto: 12345): ");
	fgets(serverPort, PORTLENGTH, stdin);
	if (strncmp(serverPort, "", 1)) {
		strcpy(serverPort, "12345");
	}
	
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
SOCKET initConnection(char *argv[])
{
	if (atoi(argv[1]) == 0) {
		//Client code
		return initClient();
	} else if (atoi(argv[1]) == 1) {
		//Server code
		return initServer();
	} else {
		//WTF code
		printf("Enter argument: 0 for client (Second player), 1 for server (First player)");
		exit(-2);
	}
}
//---------------------------------------------------------------------------