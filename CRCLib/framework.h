#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include "stdint.h"

typedef uint8_t crc;

/**
 * Initialize the CRC table
 */
void crcInit();

/**
 * Function that uses the lookup table contents to compute a CRC 
 *
 * @param Message to get CRC code from and length of the same message in bytes
 * @return CRC code
 */
crc crcFast(uint8_t const message[], int nBytes);