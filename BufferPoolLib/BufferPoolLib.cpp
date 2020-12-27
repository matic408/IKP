// BufferPoolLib.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "framework.h"
#include <stdlib.h>

BufferPool CreateBufferPool(int size, int amount)
{
	return BufferPool(size,amount);
}

void ReleaseBufferPool(BufferPool* b)
{
	b->ReleaseBufferPool();
}

char * RetrieveDataPosition(BufferPool* b, int num)
{
	return b->RetrieveDataPosition(num);
}

void SaveData(BufferPool* b, char * data, int num)
{
	b->SaveData(data, num);
}

bool ReleaseData(BufferPool * b, int num)
{
	return b->ReleaseData(num);
}

