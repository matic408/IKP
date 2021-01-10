// SafeUDPLib.cpp : Defines the functions for the static library.
//
#include "pch.h"
#include "framework.h"
#include "..\BufferPoolLib\\framework.h"

#define SEGMENT_SIZE 5
#define SLIDING_WINDOW_WIDTH 4

struct data {
	int num;
	char text[SEGMENT_SIZE];
};

int SafeUDPSend(SOCKET *clientSocket, char* sendingData, int sizeOfData, LPSOCKADDR sendingAddress, LPSOCKADDR recievingAddress, int addressLength) {

	//sockaddr_in clientAddress;
	//memset(&clientAddress, 0, sizeof(sockaddr_in));
	//int sockAddrLen = sizeof(struct sockaddr);
	//u jednom momentu dovoljno za 2 slanja + 1
	BufferPool bufferPool = CreateBufferPool(SEGMENT_SIZE, SLIDING_WINDOW_WIDTH * 2);

	int SegmentsToSend = sizeOfData / SEGMENT_SIZE;
	if (sizeOfData % SEGMENT_SIZE != 0) {
		SegmentsToSend++;
	}
	int HighestSeqNum = SegmentsToSend;

	int SavedSequenceNum = 0;
	int SequenceNumToSend = 1;
	int TotalAmountToSend = sizeOfData;
	int RemovedFromPoolCount = SLIDING_WINDOW_WIDTH * 2;
	int lastAck = 0;
	int SentWithoutAck = 0;
	char* sendData = (char*)malloc(4 + SEGMENT_SIZE);
	int* recievedAck = (int*)malloc(4);
	char *p = NULL;
	int iResult;

	//iResult = bind(*clientSocket, (LPSOCKADDR)&recievingAddress, sizeof(recievingAddress));

	data temp;

	FD_SET set;
	timeval timeVal;
	timeVal.tv_sec = 8;
	timeVal.tv_usec = 0;

	while (SegmentsToSend > 0) {
		//popuniti bufferpool koliko je moguce
		for (int i = 0; i < RemovedFromPoolCount; i++) {
			if (SavedSequenceNum < HighestSeqNum) {
				SavedSequenceNum++;
				SaveData(&bufferPool, sendingData + ((SavedSequenceNum - 1)*SEGMENT_SIZE), SavedSequenceNum);
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
				SentWithoutAck = i;
				break;
			}
			temp.num = htonl(SequenceNumToSend);
			memcpy(temp.text, p, SEGMENT_SIZE);
			SentWithoutAck = SLIDING_WINDOW_WIDTH;
			//memcpy(sendData, &SequenceNumToSend, 4);
			//memcpy(sendData + sizeof(int), p, SEGMENT_SIZE);
			SequenceNumToSend++;
			sendto(*clientSocket,
				(char*)&temp,
				(int)sizeof(data),
				0,
				sendingAddress,
				addressLength);
		}

		//FD_ZERO(&set);
		//FD_SET(*clientSocket, &set);

		//iResult = select(0 /* ignored */, &set, NULL, NULL, &timeVal);
		//if (iResult == SOCKET_ERROR)
		//{
		//	return -1;
		//}

		//if (iResult == 0)
		//{
		//	SequenceNumToSend -= SentWithoutAck;
		//	continue;
		//}
		//ovde udje i posle toga pukne negde nikad ne dodje do sl linije
		iResult = recvfrom(*clientSocket,
			(char*)recievedAck,
			4,
			0,
			(SOCKADDR *)&recievingAddress,
			&addressLength);

		if (iResult == SOCKET_ERROR) {
			int sklj = WSAGetLastError();
			int sklj2 = 0;
		}

		*recievedAck = ntohl(*recievedAck);

		for (int i = *recievedAck - 1; i > lastAck; i--) {
			if (ReleaseData(&bufferPool, i)) {
				RemovedFromPoolCount++;
			}
		}

		lastAck = *recievedAck - 1;
		SequenceNumToSend = *recievedAck;
		SegmentsToSend -= RemovedFromPoolCount;

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

int SafeUDPReceive(SOCKET *serverSocket, char * receivingData, int sizeOfBuffer, LPSOCKADDR sendingAddress, LPSOCKADDR recievingAddress, int addressLength)
{
	int iResult = 0;
	int expectedSeqNum = 1;
	//greska sa upisom zbog kolicine koja se upisuje ne moze override iz nekog razloga
	char* received = (char*)malloc((sizeof(SEGMENT_SIZE) + 4) * SLIDING_WINDOW_WIDTH);
	data* temp;

	FD_SET set;
	timeval timeVal;

	timeVal.tv_sec = 1;
	timeVal.tv_usec = 0;

	int countForEnd = 0;

	iResult = bind(*serverSocket, (LPSOCKADDR)&recievingAddress, sizeof(recievingAddress));

	while (1) {

		for (int i = 0; i < SLIDING_WINDOW_WIDTH; i++) {
			FD_ZERO(&set);
			FD_SET(*serverSocket, &set);

			iResult = select(0 /* ignored */, &set, NULL, NULL, &timeVal);

			if (iResult == 0) {
				countForEnd++;
				break;
			}
			countForEnd = 0;

			iResult = recvfrom(*serverSocket,
				received,
				sizeof(data),
				0,
				(SOCKADDR *)&recievingAddress,
				&addressLength);
			temp = (data*)received;
			//samo prvi put odradi htohl drugi put
			temp->num = ntohl(temp->num);

			if (expectedSeqNum == temp->num) {
				memcpy((receivingData + SEGMENT_SIZE * (expectedSeqNum - 1)), temp->text, SEGMENT_SIZE);
				expectedSeqNum++;
			}
			else {
				break;
			}

		}

		if (countForEnd == 10) {
			break;
		}
		expectedSeqNum = htonl(expectedSeqNum);
		iResult = sendto(*serverSocket,
			(char*)&expectedSeqNum,
			4,
			0,
			(SOCKADDR *)&sendingAddress,
			addressLength);
		expectedSeqNum = ntohl(expectedSeqNum);

	}

	free(received);
	//ReleaseBufferPool(&pool);
	//?
	return expectedSeqNum * SEGMENT_SIZE;
}


