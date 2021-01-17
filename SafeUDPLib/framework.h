#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <winsock2.h>
#include <string.h>

int SafeUDPSend(SOCKET *clientSocket, char* sendingData, int sizeOfData, LPSOCKADDR sendingAddress, int addressLength);
int SafeUDPReceive(SOCKET *serverSocket, char* receivingData, int sizeOfBuffer, LPSOCKADDR recievingAddress, int addressLength);