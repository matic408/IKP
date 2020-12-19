#pragma once
#include <stdlib.h>
#include <string.h>
class BufferPool
{
private:
	char* p;
	int unitSize;
	int length;
	int* status;
public:
	BufferPool(int size, int amount) {
		p = (char*)malloc(size*amount);
		unitSize = size;
		length = amount;
		status = (int*)malloc(sizeof(int)*amount);
		for (int i = 0; i < amount; i++) {
			status[i] = 0;
		}
	}

	void ReleaseBufferPool() {
		free(p);
		free(status);
	}

	bool SaveData(char* data, int num) {
		char* emptySpace = NULL;
		for (int i = 0; i < length; i++) {
			if (status[i] == 0) {
				emptySpace = p + (unitSize * i);
				status[i] = num;
				break;
			}
		}
		if (emptySpace == NULL) {
			return false;
		}
		else {
			memcpy(emptySpace, data, unitSize);
			return true;
		}
	}

	bool ReleaseData(int num) {
		for (int i = 0; i < length; i++) {
			if (status[i] == num) {
				status[i] = 0;
				return true;
			}
		}
		return false;
	}
};

