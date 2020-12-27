#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include "BufferPool.h"

BufferPool CreateBufferPool(int size, int amount);
void ReleaseBufferPool(BufferPool* b);
char* RetrieveDataPosition(BufferPool* b, int num);
void SaveData(BufferPool* b, char* data, int num);
bool ReleaseData(BufferPool* b, int num);