#include <winsock2.h>
#include <stdio.h>
#include <conio.h>
#include "..\SafeUDPLib\framework.h"

#define SERVER_PORT 15000

#define OUTGOING_BUFFER_SIZE 2000
// for demonstration purposes we will hard code
// local host ip adderss
#define SERVER_IP_ADDERESS "127.0.0.1"

// Initializes WinSock2 library
// Returns true if succeeded, false otherwise.
bool InitializeWindowsSockets();

int main(int argc,char* argv[])
{
    // Server address
    sockaddr_in sendingAddress;
    //sockaddr_in recievingAddress;
    // size of sockaddr structure    
	int sockAddrLen = sizeof(struct sockaddr);
	// buffer we will use to store message
	char *outgoingBuffer = (char*)malloc(OUTGOING_BUFFER_SIZE);
	if (!outgoingBuffer) {
		printf("Too much characters\n");
		return 0;
	}
    // port used for communication with server
    int serverPort = SERVER_PORT;
	// variable used to store function return value
	int iResult;

    // Initialize windows sockets for this process
    InitializeWindowsSockets();

    //radice send na portu 15000
    memset((char*)&sendingAddress,0,sizeof(sendingAddress));
    sendingAddress.sin_family = AF_INET;
    sendingAddress.sin_addr.s_addr = inet_addr(SERVER_IP_ADDERESS);
    sendingAddress.sin_port = htons((u_short)serverPort);

	// create a socket
    SOCKET clientSocket = socket(AF_INET,      // IPv4 address famly
								 SOCK_DGRAM,   // datagram socket
								 IPPROTO_UDP); // UDP

    // check if socket creation succeeded
    if (clientSocket == INVALID_SOCKET)
    {
        printf("Creating socket failed with error: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

	printf("Enter message from server:\n");

	// Read string from user into outgoing buffer
    //gets_s(outgoingBuffer, OUTGOING_BUFFER_SIZE);

	//stres test za mnogo podataka

	memset(outgoingBuffer, 'a', OUTGOING_BUFFER_SIZE);

	memset(outgoingBuffer + OUTGOING_BUFFER_SIZE - 2, 'c', 1);
		 //Set socket to nonblocking mode

	unsigned long int nonBlockingMode = 1;
	iResult = ioctlsocket(clientSocket, FIONBIO, &nonBlockingMode);


	iResult = SafeUDPSend(&clientSocket,
		outgoingBuffer,
		strlen(outgoingBuffer),
		(LPSOCKADDR)&sendingAddress,
		sockAddrLen);

    if (iResult == SOCKET_ERROR)
    {
        printf("sendto failed with error: %d\n", WSAGetLastError());
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

	free(outgoingBuffer);
	printf("Message sent to server, press any key to exit.\n");
	_getch();

	
    iResult = closesocket(clientSocket);
    if (iResult == SOCKET_ERROR)
    {
        printf("closesocket failed with error: %d\n", WSAGetLastError());
        return 1;
    }

    iResult = WSACleanup();
    if (iResult == SOCKET_ERROR)
    {
        printf("WSACleanup failed with error: %ld\n", WSAGetLastError());
        return 1;
    }

    return 0;
}

bool InitializeWindowsSockets()
{
    WSADATA wsaData;
	// Initialize windows sockets library for this process
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0)
    {
        printf("WSAStartup failed with error: %d\n", iResult);
        return false;
    }
	return true;
}
