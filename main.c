//---------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <winsock2.h>
#include <windows.h> 
//---------------------------------------------------------------------------
void checkArgs(int argc) {	//Num of args
	if (argc < 2) {
		printf("Enter argument: 0 for client (Second player), 1 for server (First player)");
		exit(-1);
	}
}
//---------------------------------------------------------------------------
void initClient() {
	char serverIP[16];
	printf("Enter server's IP (example): ");
	scanf("%s", &serverIP);
}
//---------------------------------------------------------------------------
void initServer() {
	
}
//---------------------------------------------------------------------------
int main (int argc, char *argv[]) { //Server: 1 serverPort; Client: 0 serverIP serverPort
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