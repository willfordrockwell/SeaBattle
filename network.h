#ifndef __NETWORK_H__
#define __NETWORK_H__

#include <stdio.h>
#include <winsock2.h>
#include <windows.h>

SOCKET initClient();
SOCKET initServer();
SOCKET initConnection(char *argv[]);
#endif