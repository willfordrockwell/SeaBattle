//---------------------------------------------------------------------------
#include "stdafx.h"
#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include <errno.h>
#include <unistd.h> 
#include <string.h>
#include <sys/types.h> 
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>

//---------------------------------------------------------------------------
int main (int argc, char *argv[]) {
	if (argc < 2) {
		printf("Enter argument: 0 for client (Second player), 1 for server (First player)");
		return -1;
	}
	
	if (atoi(argv[1]) == 0) {
		//Client code
		printf("Enter server's IP (example): ");
		scanf();
		
		//Init socket
		struct WSAData WS;
		if (FAILED(WSAStartup(0x202, (WSADATA *)&WS))){
			//Error
			printf("Client can NOT initialize WSAStartup, error: %d\n", WSAGetLastError());
			return -3;
		}
		
		//Create windows socket
		SOCKET sockTCP;
		if ((sockTCP = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET){
			//Error
			printf("Client can NOT create socket, error: %d\n", WSAGetLastError());
			return -4;
		}
	}
	else if (atoi(argv[1]) == 1) {
		//Server code
		printf("1");
	}
	else {
		//WTF code
		printf("Enter argument: 0 for client (Second player), 1 for server (First player)");
		return -2;
	}
	return 0;
}