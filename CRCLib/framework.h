#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include "stdint.h"

typedef uint8_t crc;

void crcInit();
crc crcFast(uint8_t const message[], int nBytes);