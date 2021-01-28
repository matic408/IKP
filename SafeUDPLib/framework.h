#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <winsock2.h>
#include <string.h>

/**
 * Sends data the same way simple UDP sendto does but with ACK confirmation
 *
 * @param Same params as UDP sendto func
 * @return 0 if everything is OK and in case of SOCKET_ERROR it returns same as UDP sendto func
 */
int SafeUDPSend(SOCKET *clientSocket, char* sendingData, int sizeOfData, LPSOCKADDR sendingAddress, int addressLength);

/**
 * Recieves data the same way simple UDP recvfrom does but also sends ACKs for recieved data
 *
 * @param Same params as UDP sendto func
 * @return Amount of data recieved if everything is OK and in case of SOCKET_ERROR it returns same as UDP recvfrom func
 */
int SafeUDPReceive(SOCKET *serverSocket, char* receivingData, int sizeOfBuffer, LPSOCKADDR recievingAddress, int addressLength);