// SafeUDPLib.cpp : Defines the functions for the static library.
//
#include "pch.h"
#include "framework.h"
#include "..\BufferPoolLib\\framework.h"

#define SEGMENT_SIZE 5
#define SLIDING_WINDOW_WIDTH 4

int SafeUDPSend(SOCKET clientSocket, char* sendingData, int sizeOfData, LPSOCKADDR address, int addressLength) {

	
	//u jednom momentu dovoljno za 2 slanja + 1
	BufferPool bufferPool = CreateBufferPool(SEGMENT_SIZE, SLIDING_WINDOW_WIDTH * 2);

	int SavedSequenceNum = 0;
	int SequenceNumToSend = 0;

	int freeSpace = SLIDING_WINDOW_WIDTH * 2;
	int result = 0;
	char* sendData = (char*)malloc(sizeof(int) + SEGMENT_SIZE);

	while (sizeOfData != 0) {
		
		

		for (int i = 0; i < freeSpace; i++) {
			SaveData(&bufferPool, sendingData + (i*SEGMENT_SIZE), SavedSequenceNum++);
		}

		for (int i = 0; i < SLIDING_WINDOW_WIDTH; i++) {
			*sendData = ++SequenceNumToSend;
			memcpy(sendData + sizeof(int), RetrieveDataPosition(&bufferPool, SequenceNumToSend), SEGMENT_SIZE);
			
			sendto(clientSocket,
				sendData,
				(sizeof(int) + SEGMENT_SIZE),
				0,
				address,
				addressLength);
			//maybe provera
			
		}







	}
	//nes za kraj

}
