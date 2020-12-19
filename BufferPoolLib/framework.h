#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include "BufferPool.h"

BufferPool CreateBufferPool(int size, int amount);
void ReleaseBufferPool(BufferPool b);

