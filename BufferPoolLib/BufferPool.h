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

	char* RetrieveDataPosition(int num) {
		char *pos = p;
		for (int i = 0; i < length; i++) {
			if (status[i] == num) {
				pos += i * unitSize;
				return pos;
			}
		}
		return NULL;
	}

	void ReleaseBufferPool() {
		free(p);
		free(status);
	}

	void SaveData(char* data, int num) {
		char* emptySpace = NULL;
		for (int i = 0; i < length; i++) {
			if (status[i] == 0) {
				emptySpace = p + (unitSize * i);
				status[i] = num;
				break;
			}
		}
		if (emptySpace == NULL) {
			int oldLength = length;
			length *= 2;
			p = (char*)realloc(p, length*unitSize);
			status = (int*)realloc(status, length*sizeof(int));
			for (int j = oldLength; j < length; j++) {
				status[j] = 0;
			}
			memcpy(p + oldLength * unitSize, data, unitSize);
			status[oldLength] = num;
		}
		else {
			memcpy(emptySpace, data, unitSize);
		}
	}

	bool ReleaseData(int num) {
		if (num <= 0) {
			return false;
		}
		for (int i = 0; i < length; i++) {
			if (status[i] == num) {
				status[i] = 0;
				return true;
			}
		}
		return false;
	}
};

