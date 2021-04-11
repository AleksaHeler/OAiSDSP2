#pragma once

#include "BitWritter.h"

void writeBit(struct BitWritter* bw, uint8_t bit)
{
	bw->buffer |= (bit << bw->bit_idx);
	bw->bit_idx++;
	if (bw->bit_idx == 8)
	{
		bw->file->putChar(bw->buffer);
		bw->bit_idx = 0;
		bw->buffer = 0;
	}
}

void flushBitWritter(struct BitWritter* bw)
{
	bw->file->putChar(bw->buffer);
	bw->bit_idx = 0;
	bw->buffer = 0;
}


