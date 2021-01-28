#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include "BufferPool.h"

/**
 * Generates BufferPool Instance
 * 
 * @param Size and amount of data needed
 * @return Instance of BufferPool
 */
BufferPool CreateBufferPool(int size, int amount);

/**
 * Free the memory used by BufferPool instance
 */
void ReleaseBufferPool(BufferPool* b);

/**
 * Gets the data from certain position in BufferPool
 *
 * @param BufferPool pointer and position from which is data
 * @return data extracted from BufferPool
 */
char* RetrieveDataPosition(BufferPool* b, int num);

/**
 * Saves data in BufferPool
 *
 * @param BufferPool pointer, data which is stored and id of data
 */
void SaveData(BufferPool* b, char* data, int num);

/**
 * Data under that id in BufferPool is no longer important
 *
 * @param BufferPool pointerd and id of data
 * @return true if data under that id exists and false if it doesn't exist
 */
bool ReleaseData(BufferPool* b, int num);