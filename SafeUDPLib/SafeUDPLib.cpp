// SafeUDPLib.cpp : Defines the functions for the static library.
//
#include "pch.h"
#include "framework.h"
#include "..\BufferPoolLib\\framework.h"

#define SEGMENT_SIZE 5
#define SLIDING_WINDOW_WIDTH 4

int SafeUDPSend(SOCKET *clientSocket, char* sendingData, int sizeOfData, LPSOCKADDR address, int addressLength) {

	sockaddr_in clientAddress;
	memset(&clientAddress, 0, sizeof(sockaddr_in));
	int sockAddrLen = sizeof(struct sockaddr);
	//u jednom momentu dovoljno za 2 slanja + 1
	BufferPool bufferPool = CreateBufferPool(SEGMENT_SIZE, SLIDING_WINDOW_WIDTH * 2);

	int SavedSequenceNum = 0;
	int SequenceNumToSend = 1;
	int TotalAmountToSend = sizeOfData;
	int RemovedFromPoolCount = SLIDING_WINDOW_WIDTH * 2;
	int lastAck = 0;
	int leftovers;
	char* sendData = (char*)malloc(sizeof(int) + SEGMENT_SIZE);
	int* recievedAck = (int*)malloc(sizeof(int));
	char *p = NULL;

	FD_SET set;
	timeval timeVal;
	timeVal.tv_sec = 1;
	timeVal.tv_usec = 0;

	while (TotalAmountToSend > 0) {
		//popuniti bufferpool koliko je moguce
		for (int i = 0; i < RemovedFromPoolCount; i++) {
			if (SavedSequenceNum < sizeOfData) {
				SavedSequenceNum++;
				SaveData(&bufferPool, sendingData + (i*SEGMENT_SIZE), SavedSequenceNum);
			}
			else {
				break;
			}

		}
		RemovedFromPoolCount = 0;

		//poslati maksimalan broj segmenata koji je dozvoljen
		for (int i = 0; i < SLIDING_WINDOW_WIDTH; i++) {
			p = RetrieveDataPosition(&bufferPool, SequenceNumToSend);
			if (p == NULL) {
				leftovers = i;
				break;
			}
			*sendData = SequenceNumToSend;
			leftovers = SLIDING_WINDOW_WIDTH;
			memcpy(sendData + sizeof(int), p, SEGMENT_SIZE);
			SequenceNumToSend++;
			sendto(*clientSocket,
				sendData,
				(sizeof(int) + SEGMENT_SIZE),
				0,
				address,
				addressLength);
			//maybe provera

		}

		FD_ZERO(&set);
		FD_SET(*clientSocket, &set);

		int iResult = select(0 /* ignored */, &set, NULL, NULL, &timeVal);
		if (iResult == SOCKET_ERROR)
		{
			return -1;
		}

		if (iResult == 0)
		{
			SequenceNumToSend -= leftovers;
			continue;
		}

		iResult = recvfrom(*clientSocket,
			(char*)recievedAck,
			sizeof(int),
			0,
			(LPSOCKADDR)&clientAddress,
			&sockAddrLen);

		for (int i = *recievedAck - 1; i >= lastAck; i--) {
			if (ReleaseData(&bufferPool, i)) {
				RemovedFromPoolCount++;
			}
		}

		lastAck = *recievedAck;
		SequenceNumToSend = lastAck;
		TotalAmountToSend -= RemovedFromPoolCount;

		if (iResult == SOCKET_ERROR)
		{
			return -1;
		}

	}
	ReleaseBufferPool(&bufferPool);
	free(sendData);
	free(recievedAck);

	return 0;
}

int SafeUDPReceive(SOCKET *serverSocket, char * receivingData, int sizeOfBuffer, LPSOCKADDR address, int addressLength)
{
	int iResult = 0;
	int expectedSeqNum = 1;
	char* received = (char*)malloc(sizeof(SEGMENT_SIZE) + 4);

	//BufferPool pool = CreateBufferPool(SEGMENT_SIZE, SLIDING_WINDOW_WIDTH * 2);

	FD_SET set;
	timeval timeVal;

	FD_ZERO(&set);
	// Add socket we will wait to read from
	FD_SET(*serverSocket, &set);

	timeVal.tv_sec = 1;
	timeVal.tv_usec = 0;

	int countForEnd = 0;


	while (1) {

		for (int i = 0; i < SLIDING_WINDOW_WIDTH; i++) {

			iResult = select(0 /* ignored */, &set, NULL, NULL, &timeVal);

			if (iResult == 0) {


				countForEnd++;
				break;
			}
			countForEnd = 0;

			iResult = recvfrom(*serverSocket,
				received,
				sizeof(SEGMENT_SIZE) + 4,
				0,
				(LPSOCKADDR)&address,
				&addressLength);

			if (expectedSeqNum == (int)(*receivingData)) {
				memcpy((receivingData + SEGMENT_SIZE * (expectedSeqNum - 1)), received + 4, SEGMENT_SIZE);
				expectedSeqNum++;
			}
			else {
				break;
			}

		}

		if (countForEnd == 3) {
			break;
		}

		sendto(*serverSocket,
			(char*)&expectedSeqNum,
			(sizeof(int)),
			0,
			address,
			addressLength);

	}

	free(received);
	//ReleaseBufferPool(&pool);
	//?
	return expectedSeqNum * SEGMENT_SIZE;
}


