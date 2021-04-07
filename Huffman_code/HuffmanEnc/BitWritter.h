#pragma once

#include <stdio.h> 
#include <stdint.h>

struct BitWritter
{
	FILE* file;
	uint8_t buffer;
	int_fast8_t bit_idx;
};

void writeBit(struct BitWritter* bw, uint8_t bit);

void flushBitWritter(struct BitWritter* bw);



