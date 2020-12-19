// BufferPoolLib.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "framework.h"
#include <stdlib.h>

BufferPool CreateBufferPool(int size, int amount)
{
	return BufferPool(size,amount);
}

void ReleaseBufferPool(BufferPool b)
{
	b.ReleaseBufferPool();
}
